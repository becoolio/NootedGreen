#include "RcsEngineTrace.hpp"

#include "kern_gen11.hpp"
#include "kern_green.hpp"

#include <Headers/kern_api.hpp>

static int getTraceSchedulerType() {
	int schedType = 5;
	int bootArgSched = 0;
	if (PE_parse_boot_argn("ngreenSched", &bootArgSched, sizeof(bootArgSched)) &&
	    bootArgSched >= 3 && bootArgSched <= 5) {
		schedType = bootArgSched;
	}

	return schedType;
}

static uint32_t readTraceReg(uint32_t reg) {
	return Gen11::tReadRegister32(reg);
}

static uint32_t readMem32(const volatile uint32_t *base, size_t dwordOffset) {
	return base ? base[dwordOffset] : 0;
}

static bool looksLikeKernelPtr(uint64_t value) {
	return value >= 0xffffff8000000000ULL;
}

static bool vtableMatches(mach_vm_address_t objectVtable, mach_vm_address_t symbolVtable) {
	return symbolVtable != 0 && (objectVtable == symbolVtable || objectVtable == symbolVtable + 0x10);
}

static const char *candidateTypeName(void *candidate) {
	if (!candidate) return "null";
	auto objectVtable = *reinterpret_cast<mach_vm_address_t *>(candidate);
	if (vtableMatches(objectVtable, Gen11::tGetIGSharedMappedBufferVtable())) return "IGSharedMappedBuffer";
	if (vtableMatches(objectVtable, Gen11::tGetIGMappedBufferVtable())) return "IGMappedBuffer";
	return "unknown";
}

static void dumpObjectQwords(const char *prefix, const char *stage, void *object) {
	if (!object) {
		SYSLOG("ngreen", "%s[%s]: object=null", prefix, stage ? stage : "<null>");
		return;
	}
	auto *q = reinterpret_cast<uint64_t *>(object);
	for (uint32_t i = 0; i < 0x120 / 8; i++) {
		SYSLOG("ngreen", "%s[%s]: qword[0x%x]=0x%016llx", prefix, stage ? stage : "<null>", i * 8,
		       static_cast<unsigned long long>(q[i]));
	}
}

static void dumpCandidateMappedBuffers(const char *prefix, const char *stage, void *object) {
	if (!object) return;
	auto *q = reinterpret_cast<uint64_t *>(object);
	for (uint32_t i = 0; i < 0x120 / 8; i++) {
		uint64_t value = q[i];
		if (!looksLikeKernelPtr(value)) continue;
		void *candidate = reinterpret_cast<void *>(value);
		const char *type = candidateTypeName(candidate);
		auto objectVtable = *reinterpret_cast<mach_vm_address_t *>(candidate);
		SYSLOG("ngreen", "%s[%s]: candidate[0x%x]=%p vtable=0x%llx type=%s",
		       prefix, stage ? stage : "<null>", i * 8, candidate,
		       static_cast<unsigned long long>(objectVtable), type);
		if (strcmp(type, "IGSharedMappedBuffer") == 0) {
			void *cpu = Gen11::tGetSharedMappedBufferVirtualAddress(candidate);
			SYSLOG("ngreen", "%s[%s]: candidate[0x%x] sharedCpu=%p",
			       prefix, stage ? stage : "<null>", i * 8, cpu);
		}
		if (strcmp(type, "IGMappedBuffer") == 0) {
			uint64_t gpu = Gen11::tGetMappedBufferGPUVirtualAddress(candidate);
			SYSLOG("ngreen", "%s[%s]: candidate[0x%x] mappedGpu=0x%llx",
			       prefix, stage ? stage : "<null>", i * 8, static_cast<unsigned long long>(gpu));
		}
	}
}

void dumpContextImageKnownFields(const char *stage, void *context) {
	if (!isRcsEngineTraceEnabled() || !context) {
		return;
	}

	dumpObjectQwords("NG_CONTEXT_OBJ", stage, context);
	dumpCandidateMappedBuffers("NG_CONTEXT_OBJ", stage, context);

	auto *ctxImage = reinterpret_cast<volatile uint32_t *>(Gen11::tGetSharedMappedBufferVirtualAddress(getMember<void *>(context, 0x98)));
	uint64_t lrcaGpu = Gen11::tGetMappedBufferGPUVirtualAddress(getMember<void *>(context, 0xb0));
	void *hwsCpu = Gen11::tGetSharedMappedBufferVirtualAddress(getMember<void *>(context, 0xa8));
	void *auxCpu = Gen11::tGetSharedMappedBufferVirtualAddress(getMember<void *>(context, 0xa0));
	uint32_t ringSizeBytes = getMember<void *>(context, 0x50) ? (getMember<uint32_t>(getMember<void *>(context, 0x50), 0x119c) << 10) : 0;

	SYSLOG("ngreen", "NG_CONTEXT_DESC[%s]: ctx=%p ctxImageCpu=%p lrcaGpu=0x%llx ringSize=0x%x hwsCpu=%p auxCpu=%p engineType=%u ctxFlags=0x%x",
	       stage ? stage : "<null>", context, ctxImage, static_cast<unsigned long long>(lrcaGpu), ringSizeBytes,
	       hwsCpu, auxCpu, static_cast<unsigned>(getMember<uint8_t>(context, 0x6c)), static_cast<unsigned>(getMember<uint8_t>(context, 0x6e)));

	if (!ctxImage) {
		return;
	}

	uint32_t headField = readMem32(ctxImage, 0x1014 / sizeof(uint32_t));
	uint32_t tailField = readMem32(ctxImage, 0x101c / sizeof(uint32_t));
	uint32_t ringGpuLo = readMem32(ctxImage, 0x1024 / sizeof(uint32_t));
	uint32_t ringCtlField = readMem32(ctxImage, 0x102c / sizeof(uint32_t));

	SYSLOG("ngreen", "NG_LRCA[%s]: ctx=%p HEAD_FIELD=0x%x TAIL_FIELD=0x%x RING_GPU_LO=0x%x RING_CTL_FIELD=0x%x",
	       stage ? stage : "<null>", context, headField, tailField, ringGpuLo, ringCtlField);

	for (uint32_t i = 0; i < 8; i++) {
		SYSLOG("ngreen", "NG_LRCA[%s]: dword[%u]=0x%08x", stage ? stage : "<null>", i, ctxImage[i]);
	}

	if (hwsCpu) {
		auto *hws = reinterpret_cast<volatile uint32_t *>(hwsCpu);
		for (uint32_t i = 0; i < 8; i++) {
			SYSLOG("ngreen", "NG_STAMP_MEM[%s]: HWS[%u]=0x%08x", stage ? stage : "<null>", i, hws[i]);
		}
		SYSLOG("ngreen", "NG_STAMP_MEM[%s]: stampIdx3Ptr=%p stampIdx3Value=0x%08x", stage ? stage : "<null>", &hws[3], hws[3]);
	}
}

void dumpRingMemoryKnownFields(const char *stage, void *ringBuffer) {
	if (!isRcsEngineTraceEnabled() || !ringBuffer) {
		return;
	}

	dumpObjectQwords("NG_RING_OBJ", stage, ringBuffer);
	dumpCandidateMappedBuffers("NG_RING_OBJ", stage, ringBuffer);

	auto *ringCpu = reinterpret_cast<volatile uint32_t *>(Gen11::tGetSharedMappedBufferVirtualAddress(getMember<void *>(ringBuffer, 0x80)));
	uint32_t tailCursor = getMember<uint32_t>(ringBuffer, 0x64);
	uint32_t oldTail = getMember<uint32_t>(ringBuffer, 0x68);
	uint32_t mask = getMember<uint32_t>(ringBuffer, 0x90);
	uint32_t ringSize = mask + 1;
	uint32_t avail = getMember<uint32_t>(ringBuffer, 0x88);
	uint32_t flags = getMember<uint32_t>(ringBuffer, 0x64);
	uint8_t engineIndex = getMember<uint8_t>(ringBuffer, 0x40);
	uint8_t submitState = getMember<uint8_t>(ringBuffer, 0x48);
	uint32_t submitFlags = getMember<uint32_t>(ringBuffer, 0x64);
	uint32_t descIndex = getMember<uint32_t>(ringBuffer, 0x38);
	uint32_t waitFlags = getMember<uint32_t>(ringBuffer, 0x4c);

	SYSLOG("ngreen", "NG_RING_MEM[%s]: ring=%p cpu=%p engineIndex=%u descIndex=0x%x ringSize=0x%x mask=0x%x tailCursor=0x%x oldTail=0x%x avail=0x%x submitState=0x%x submitFlags=0x%x waitFlags=0x%x",
	       stage ? stage : "<null>", ringBuffer, ringCpu, static_cast<unsigned>(engineIndex), descIndex,
	       ringSize, mask, tailCursor, oldTail, avail, static_cast<unsigned>(submitState), submitFlags, waitFlags);
	SYSLOG("ngreen", "NG_TAIL_WRITE[%s]: oldTail=0x%x newTail=0x%x ringSize=0x%x", stage ? stage : "<null>", oldTail, tailCursor, ringSize);

	if (!ringCpu || ringSize == 0) {
		return;
	}

	uint32_t startDword = (tailCursor & mask) >> 2;
	for (uint32_t i = 0; i < 16; i++) {
		uint32_t idx = (startDword + i) & (mask >> 2);
		SYSLOG("ngreen", "NG_RING_MEM[%s]: dword[%u @0x%x]=0x%08x", stage ? stage : "<null>", i, idx << 2, ringCpu[idx]);
	}
}

struct RcsEngineTransitionState {
	uint32_t ringStart {0};
	uint32_t ringCtl {0};
	uint32_t ringTail {0};
	uint32_t acthd {0};
	uint32_t contextStatusPtr {0};
	uint32_t errorGen6 {0};
	bool initialized {false};
	bool firstCsbNonZeroSeen {false};
	bool firstErrorNonZeroSeen {false};
};

static RcsEngineTransitionState gRcsTransitionState {};

bool isRcsEngineTraceEnabled() {
	int enabled = 0;
	if (PE_parse_boot_argn("ngreen-rcsdeep", &enabled, sizeof(enabled))) {
		return enabled != 0;
	}
	if (PE_parse_boot_argn("ngreenRcsEngineTrace", &enabled, sizeof(enabled))) {
		return enabled != 0;
	}

	if (checkKernelArgument("-ngreen-rcsdeep")) {
		return true;
	}
	return checkKernelArgument("-ngreenRcsEngineTrace");
}

static uint32_t getForceWakeState() {
	return readTraceReg(FORCEWAKE_ACK_RENDER_GEN9);
}

void decodeErrorGen6(uint32_t value) {
	if (!isRcsEngineTraceEnabled()) {
		return;
	}

	uint32_t knownMask = 0;
	if (value & I915_ERROR_INSTRUCTION) {
		knownMask |= I915_ERROR_INSTRUCTION;
		SYSLOG("ngreen", "RCS_TRACE: ERROR_GEN6 decode: INSTRUCTION");
	}

	uint32_t unknownMask = value & ~knownMask;
	if (unknownMask) {
		SYSLOG("ngreen", "RCS_TRACE: ERROR_GEN6 decode: UNKNOWN_MASK=0x%x", unknownMask);
	}
}

void dumpRcsEngineActivationState(const char *stage) {
	if (!isRcsEngineTraceEnabled()) {
		return;
	}

	const bool allow3d = checkKernelArgument("-allow3d");
	const int schedType = getTraceSchedulerType();
	const uint32_t forceWake = Gen11::tmmioReady() ? getForceWakeState() : 0;

	if (!Gen11::tmmioReady()) {
		SYSLOG("ngreen", "RCS_TRACE[%s]: sched=%d allow3d=%d mmio=0 forcewake=0x%x",
		       stage ? stage : "<null>", schedType, allow3d, forceWake);
		SYSLOG("ngreen", "NG_RCS_BOUNDARY[%s]: mmio=0 sched=%d allow3d=%d forcewake=0x%x",
		       stage ? stage : "<null>", schedType, allow3d, forceWake);
		return;
	}

	const uint32_t ringStart = readTraceReg(RING_START(RENDER_RING_BASE));
	const uint32_t ringCtl = readTraceReg(RING_CTL(RENDER_RING_BASE));
	const uint32_t ringHead = readTraceReg(RING_HEAD(RENDER_RING_BASE));
	const uint32_t ringTail = readTraceReg(RING_TAIL(RENDER_RING_BASE));
	const uint32_t ringHws = readTraceReg(RING_HWS_PGA(RENDER_RING_BASE));
	const uint32_t execListStatus = readTraceReg(RING_EXECLIST_STATUS(RENDER_RING_BASE));
	const uint32_t contextStatusPtr = readTraceReg(RING_CONTEXT_STATUS_PTR(RENDER_RING_BASE));
	const uint32_t acthd = readTraceReg(RING_ACTHD(RENDER_RING_BASE));
	const uint32_t acthdUdw = readTraceReg(RING_ACTHD_UDW(RENDER_RING_BASE));
	const uint32_t ipehr = readTraceReg(RING_IPEHR(RENDER_RING_BASE));
	const uint32_t ipeir = readTraceReg(RING_IPEIR(RENDER_RING_BASE));
	const uint32_t instdone = readTraceReg(RING_INSTDONE(RENDER_RING_BASE));
	const uint32_t eir = readTraceReg(RING_EIR(RENDER_RING_BASE));
	const uint32_t esr = readTraceReg(RING_ESR(RENDER_RING_BASE));
	const uint32_t ringFault = readTraceReg(RING_FAULT_REG(RENDER_RING_BASE));
	const uint32_t errorGen6 = readTraceReg(ERROR_GEN6);
	const uint32_t globalRingFault = readTraceReg(GEN12_RING_FAULT_REG);
	const uint32_t rcIntrEnable = readTraceReg(GEN11_RENDER_COPY_INTR_ENABLE);
	const uint32_t rcsMask = readTraceReg(GEN11_RCS0_RSVD_INTR_MASK);
	const uint32_t gfxMstrIrq = readTraceReg(GEN11_GFX_MSTR_IRQ);
	const uint32_t gtIntrDw0 = readTraceReg(GEN11_GT_INTR_DW0);
	const uint32_t gtIntrDw1 = readTraceReg(GEN11_GT_INTR_DW1);

	SYSLOG("ngreen", "RCS_TRACE[%s]: sched=%d allow3d=%d forcewake=0x%x RCS_START=0x%x RCS_CTL=0x%x RCS_HEAD=0x%x RCS_TAIL=0x%x HWS_PGA=0x%x EXECLIST_STATUS=0x%x CONTEXT_STATUS_PTR=0x%x ACTHD=0x%x ACTHD_UDW=0x%x IPEHR=0x%x IPEIR=0x%x INSTDONE=0x%x EIR=0x%x ESR=0x%x RING_FAULT=0x%x ERROR_GEN6=0x%x GEN12_RING_FAULT=0x%x RC_INTR_EN=0x%x RCS0_MASK=0x%x GFX_MSTR_IRQ=0x%x GT_INTR_DW0=0x%x GT_INTR_DW1=0x%x",
	       stage ? stage : "<null>", schedType, allow3d, forceWake,
	       ringStart, ringCtl, ringHead, ringTail, ringHws, execListStatus,
	       contextStatusPtr, acthd, acthdUdw, ipehr, ipeir, instdone, eir, esr,
	       ringFault, errorGen6, globalRingFault, rcIntrEnable, rcsMask,
	       gfxMstrIrq, gtIntrDw0, gtIntrDw1);
	SYSLOG("ngreen", "NG_RCS_BOUNDARY[%s]: sched=%d allow3d=%d forcewake=0x%x execlist=0x%x ctxptr=0x%x",
	       stage ? stage : "<null>", schedType, allow3d, forceWake, execListStatus, contextStatusPtr);
	SYSLOG("ngreen", "NG_RING[%s]: START=0x%x CTL=0x%x HEAD=0x%x TAIL=0x%x HWS_PGA=0x%x ACTHD=0x%x ACTHD_UDW=0x%x IPEHR=0x%x IPEIR=0x%x INSTDONE=0x%x EIR=0x%x ESR=0x%x RING_FAULT=0x%x ERROR_GEN6=0x%x GEN12_RING_FAULT=0x%x",
	       stage ? stage : "<null>", ringStart, ringCtl, ringHead, ringTail, ringHws,
	       acthd, acthdUdw, ipehr, ipeir, instdone, eir, esr, ringFault, errorGen6, globalRingFault);
	SYSLOG("ngreen", "NG_ELSP[%s]: EXECLIST_STATUS=0x%x CONTEXT_STATUS_PTR=0x%x RC_INTR_EN=0x%x RCS0_INTR_MASK=0x%x GFX_MSTR_IRQ=0x%x GT_INTR_DW0=0x%x GT_INTR_DW1=0x%x",
	       stage ? stage : "<null>", execListStatus, contextStatusPtr, rcIntrEnable, rcsMask, gfxMstrIrq, gtIntrDw0, gtIntrDw1);

	for (uint32_t i = 0; i < 6; i++) {
		SYSLOG("ngreen", "RCS_TRACE[%s]: CSB[%u]=0x%08x%08x",
		       stage ? stage : "<null>", i,
		       readTraceReg(RING_CONTEXT_STATUS_BUF_HI(RENDER_RING_BASE, i)),
		       readTraceReg(RING_CONTEXT_STATUS_BUF(RENDER_RING_BASE, i)));
		SYSLOG("ngreen", "NG_CSB[%s]: idx=%u hi=0x%08x lo=0x%08x",
		       stage ? stage : "<null>", i,
		       readTraceReg(RING_CONTEXT_STATUS_BUF_HI(RENDER_RING_BASE, i)),
		       readTraceReg(RING_CONTEXT_STATUS_BUF(RENDER_RING_BASE, i)));
	}

	if (errorGen6) {
		decodeErrorGen6(errorGen6);
	}
}

void markRcsRegisterTransition(const char *stage) {
	if (!isRcsEngineTraceEnabled() || !Gen11::tmmioReady()) {
		return;
	}

	auto ringStart = readTraceReg(RING_START(RENDER_RING_BASE));
	auto ringCtl = readTraceReg(RING_CTL(RENDER_RING_BASE));
	auto ringTail = readTraceReg(RING_TAIL(RENDER_RING_BASE));
	auto acthd = readTraceReg(RING_ACTHD(RENDER_RING_BASE));
	auto contextStatusPtr = readTraceReg(RING_CONTEXT_STATUS_PTR(RENDER_RING_BASE));
	auto errorGen6 = readTraceReg(ERROR_GEN6);

	bool csbNonZero = false;
	for (uint32_t i = 0; i < 6; i++) {
		if (readTraceReg(RING_CONTEXT_STATUS_BUF(RENDER_RING_BASE, i)) != 0 ||
		    readTraceReg(RING_CONTEXT_STATUS_BUF_HI(RENDER_RING_BASE, i)) != 0) {
			csbNonZero = true;
			break;
		}
	}

	if (!gRcsTransitionState.initialized) {
		gRcsTransitionState.initialized = true;
		gRcsTransitionState.ringStart = ringStart;
		gRcsTransitionState.ringCtl = ringCtl;
		gRcsTransitionState.ringTail = ringTail;
		gRcsTransitionState.acthd = acthd;
		gRcsTransitionState.contextStatusPtr = contextStatusPtr;
		gRcsTransitionState.errorGen6 = errorGen6;
		gRcsTransitionState.firstCsbNonZeroSeen = csbNonZero;
		gRcsTransitionState.firstErrorNonZeroSeen = errorGen6 != 0;
		return;
	}

	if (gRcsTransitionState.ringStart == 0 && ringStart != 0) {
		SYSLOG("ngreen", "RCS_TRACE[%s]: transition RING_START 0->0x%x", stage ? stage : "<null>", ringStart);
	}
	if (gRcsTransitionState.ringStart != 0 && ringStart == 0) {
		SYSLOG("ngreen", "RCS_TRACE[%s]: transition RING_START 0x%x->0", stage ? stage : "<null>", gRcsTransitionState.ringStart);
	}
	if (gRcsTransitionState.ringCtl == 0 && ringCtl != 0) {
		SYSLOG("ngreen", "RCS_TRACE[%s]: transition RING_CTL 0->0x%x", stage ? stage : "<null>", ringCtl);
	}
	if (gRcsTransitionState.ringCtl != 0 && ringCtl == 0) {
		SYSLOG("ngreen", "RCS_TRACE[%s]: transition RING_CTL 0x%x->0", stage ? stage : "<null>", gRcsTransitionState.ringCtl);
	}
	if (gRcsTransitionState.ringTail != ringTail) {
		SYSLOG("ngreen", "RCS_TRACE[%s]: transition RING_TAIL 0x%x->0x%x", stage ? stage : "<null>", gRcsTransitionState.ringTail, ringTail);
	}
	if (gRcsTransitionState.acthd != acthd) {
		SYSLOG("ngreen", "RCS_TRACE[%s]: transition ACTHD 0x%x->0x%x", stage ? stage : "<null>", gRcsTransitionState.acthd, acthd);
	}
	if (gRcsTransitionState.contextStatusPtr != contextStatusPtr) {
		SYSLOG("ngreen", "RCS_TRACE[%s]: transition CONTEXT_STATUS_PTR 0x%x->0x%x", stage ? stage : "<null>", gRcsTransitionState.contextStatusPtr, contextStatusPtr);
	}
	if (!gRcsTransitionState.firstCsbNonZeroSeen && csbNonZero) {
		SYSLOG("ngreen", "RCS_TRACE[%s]: transition first CSB non-zero entry", stage ? stage : "<null>");
		gRcsTransitionState.firstCsbNonZeroSeen = true;
	}
	if (!gRcsTransitionState.firstErrorNonZeroSeen && errorGen6 != 0) {
		SYSLOG("ngreen", "RCS_TRACE[%s]: transition first ERROR_GEN6 non-zero 0x%x", stage ? stage : "<null>", errorGen6);
		gRcsTransitionState.firstErrorNonZeroSeen = true;
	}
	if (gRcsTransitionState.errorGen6 != 0 && errorGen6 == 0) {
		SYSLOG("ngreen", "RCS_TRACE[%s]: transition ERROR_GEN6 cleared from 0x%x", stage ? stage : "<null>", gRcsTransitionState.errorGen6);
	}

	gRcsTransitionState.ringStart = ringStart;
	gRcsTransitionState.ringCtl = ringCtl;
	gRcsTransitionState.ringTail = ringTail;
	gRcsTransitionState.acthd = acthd;
	gRcsTransitionState.contextStatusPtr = contextStatusPtr;
	gRcsTransitionState.errorGen6 = errorGen6;
}

bool validateRcsDescriptorOrEngineObject(const char *stage, void *object) {
	if (!isRcsEngineTraceEnabled()) {
		return false;
	}

	SYSLOG("ngreen", "RCS_TRACE[%s]: validate object=%p", stage ? stage : "<null>", object);
	if (!object) {
		return false;
	}

	const IGHwCsDesc *descs = Gen11::tGetHwCsDesc();
	if (!descs) {
		return true;
	}

	const IGHwCsDesc *rcsDesc = nullptr;
	for (size_t i = 0; i < 6; i++) {
		if (descs[i].type == kIGHwCsTypeRCS) {
			rcsDesc = &descs[i];
			break;
		}
	}

	if (!rcsDesc) {
		SYSLOG("ngreen", "RCS_TRACE[%s]: no RCS descriptor present", stage ? stage : "<null>");
		return false;
	}

	uint32_t mmioBase = rcsDesc->mmioExecListControl >= 0x3c ? rcsDesc->mmioExecListControl - 0x3c : 0;
	SYSLOG("ngreen", "RCS_TRACE[%s]: RCS desc=%p type=%u mmioBase=0x%x renderMatch=%d",
	       stage ? stage : "<null>", rcsDesc, static_cast<unsigned>(rcsDesc->type), mmioBase,
	       mmioBase == RENDER_RING_BASE);
	return mmioBase == RENDER_RING_BASE;
}

void dumpNearbyKnownEngineGlobals(const char *stage) {
	if (!isRcsEngineTraceEnabled()) {
		return;
	}

	const IGHwCsDesc *descs = Gen11::tGetHwCsDesc();
	SYSLOG("ngreen", "RCS_TRACE[%s]: kIGHwCsDesc=%p", stage ? stage : "<null>", descs);
	if (!descs) {
		return;
	}

	for (size_t i = 0; i < 6; i++) {
		const auto &desc = descs[i];
		uint32_t mmioBase = desc.mmioExecListControl >= 0x3c ? desc.mmioExecListControl - 0x3c : 0;
		SYSLOG("ngreen", "RCS_TRACE[%s]: desc[%zu]=%p type=%u name=%s label=%s mmioBase=0x%x execCtl=0x%x ctxStatusPtr=0x%x hws=0x%x fwReq=0x%x fwAck=0x%x renderMatch=%d",
		       stage ? stage : "<null>", i, &desc, static_cast<unsigned>(desc.type),
		       desc.name ? desc.name : "<null>", desc.label ? desc.label : "<null>",
		       mmioBase, desc.mmioExecListControl, desc.mmioContextStatusPointer,
		       desc.mmioGlobalStatusPage, desc.mmioForcewakeReq, desc.mmioForcewakeAck,
		       mmioBase == RENDER_RING_BASE);
	}
}
