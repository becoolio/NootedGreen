#include "TglGpuBringup.hpp"

#include "kern_gen11.hpp"
#include "kern_green.hpp"

#include <Headers/kern_api.hpp>

#include <string.h>

static constexpr uint32_t GUC_STATUS_REG = 0xC000;

static TglGpuBringupState gTglGpuBringupState {};

static bool stageContains(const char *stage, const char *needle) {
	return stage != nullptr && needle != nullptr && strstr(stage, needle) != nullptr;
}

static uint32_t traceRead32(uint32_t reg) {
	return Gen11::tReadRegister32(reg);
}

static bool anyRcsCsbEntrySet() {
	for (uint32_t i = 0; i < 6; i++) {
		if (traceRead32(RING_CONTEXT_STATUS_BUF(RENDER_RING_BASE, i)) != 0 ||
		    traceRead32(RING_CONTEXT_STATUS_BUF_HI(RENDER_RING_BASE, i)) != 0) {
			return true;
		}
	}

	return false;
}

TglGpuBringupState &getTglGpuBringupState() {
	return gTglGpuBringupState;
}

bool isTglTraceEnabled() {
	int enabled = 0;
	if (PE_parse_boot_argn("ngreenTglTrace", &enabled, sizeof(enabled))) {
		return enabled != 0;
	}

	return checkKernelArgument("-ngreenTglTrace");
}

void updateTglGpuBringupStateFromRegisters(const char *stage) {
	if (!isTglTraceEnabled()) {
		return;
	}

	auto &state = gTglGpuBringupState;
	state.allow3d = checkKernelArgument("-allow3d");
	state.mmioReady = Gen11::tmmioReady();

	if (stageContains(stage, "getGPUInfo")) {
		state.topologyOk = true;
	}
	if (stageContains(stage, "resetGraphicsEngine")) {
		state.rcsResetSeen = true;
	}
	if (stageContains(stage, "loadGuCBinary")) {
		state.gucRequested = true;
	}
	if (stageContains(stage, "checkWOPCMSettings")) {
		state.gucWopcmProgrammed = true;
	}

	if (!state.mmioReady) {
		return;
	}

	const uint32_t fwAckRender = traceRead32(FORCEWAKE_ACK_RENDER_GEN9);
	const uint32_t gucStatus = traceRead32(GUC_STATUS_REG);
	const uint32_t gucIntrEnable = traceRead32(GEN11_GUC_SG_INTR_ENABLE);
	const uint32_t gucIntrMask = traceRead32(GEN11_GUC_SG_INTR_MASK);
	const uint32_t rcsCtl = traceRead32(RING_CTL(RENDER_RING_BASE));
	const uint32_t rcsHead = traceRead32(RING_HEAD(RENDER_RING_BASE));
	const uint32_t rcsTail = traceRead32(RING_TAIL(RENDER_RING_BASE));
	const uint32_t rcsStart = traceRead32(RING_START(RENDER_RING_BASE));
	const uint32_t rcsHws = traceRead32(RING_HWS_PGA(RENDER_RING_BASE));
	const uint32_t rcsContextStatusPtr = traceRead32(RING_CONTEXT_STATUS_PTR(RENDER_RING_BASE));
	const uint32_t rcsIntrEnable = traceRead32(GEN11_RENDER_COPY_INTR_ENABLE);
	const uint32_t rcsIntrMask = traceRead32(GEN11_RCS0_RSVD_INTR_MASK);
	const uint32_t acthd = traceRead32(RING_ACTHD(RENDER_RING_BASE));
	const uint32_t ipehr = traceRead32(RING_IPEHR(RENDER_RING_BASE));
	const uint32_t errorGen6 = traceRead32(ERROR_GEN6);
	const uint32_t gtIntrDw0 = traceRead32(GEN11_GT_INTR_DW0);

	state.forceWakeOk = (fwAckRender & 1U) != 0;
	state.gtPowered = state.forceWakeOk || rcsCtl != 0 || rcsStart != 0 || gucStatus != 0;
	state.gucBooted = gucStatus != 0;
	state.gucInterruptsEnabled = gucIntrEnable != 0 && gucIntrMask != 0xFFFFFFFFU;
	state.gucSubmissionReady = state.gucBooted && state.gucInterruptsEnabled;
	state.rcsRingStarted = rcsStart != 0 || rcsCtl != 0;
	state.rcsContextReady = rcsHws != 0 || rcsContextStatusPtr != 0;
	state.rcsInterruptsEnabled = (rcsIntrEnable & (1U << GEN11_RCS0)) != 0 &&
		((rcsIntrMask & (GT_CONTEXT_SWITCH_INTERRUPT | GT_RENDER_USER_INTERRUPT)) !=
		 (GT_CONTEXT_SWITCH_INTERRUPT | GT_RENDER_USER_INTERRUPT));
	state.rcsFirstSubmitSeen = rcsHead != 0 || rcsTail != 0 || acthd != 0 || ipehr != 0;
	state.rcsFirstCompletionSeen = anyRcsCsbEntrySet() || ((gtIntrDw0 & (1U << GEN11_RCS0)) != 0);
	state.lastGucStatus = gucStatus;
	state.lastRcsCtl = rcsCtl;
	state.lastRcsHead = rcsHead;
	state.lastRcsTail = rcsTail;
	state.lastErrorGen6 = errorGen6;
}

void dumpRcsRegisters(const char *stage) {
	if (!isTglTraceEnabled()) {
		return;
	}

	if (!Gen11::tmmioReady()) {
		SYSLOG("ngreen", "TGLTrace[%s]: RCS MMIO unavailable", stage ? stage : "<null>");
		return;
	}

	SYSLOG("ngreen", "TGLTrace[%s]: RCS START=0x%x CTL=0x%x HEAD=0x%x TAIL=0x%x HWS_PGA=0x%x EXECLIST_STATUS=0x%x CONTEXT_STATUS_PTR=0x%x ACTHD=0x%x ACTHD_UDW=0x%x IPEHR=0x%x IPEIR=0x%x INSTDONE=0x%x EIR=0x%x ESR=0x%x ERROR_GEN6=0x%x RING_FAULT=0x%x",
	       stage ? stage : "<null>",
	       traceRead32(RING_START(RENDER_RING_BASE)),
	       traceRead32(RING_CTL(RENDER_RING_BASE)),
	       traceRead32(RING_HEAD(RENDER_RING_BASE)),
	       traceRead32(RING_TAIL(RENDER_RING_BASE)),
	       traceRead32(RING_HWS_PGA(RENDER_RING_BASE)),
	       traceRead32(RING_EXECLIST_STATUS(RENDER_RING_BASE)),
	       traceRead32(RING_CONTEXT_STATUS_PTR(RENDER_RING_BASE)),
	       traceRead32(RING_ACTHD(RENDER_RING_BASE)),
	       traceRead32(RING_ACTHD_UDW(RENDER_RING_BASE)),
	       traceRead32(RING_IPEHR(RENDER_RING_BASE)),
	       traceRead32(RING_IPEIR(RENDER_RING_BASE)),
	       traceRead32(RING_INSTDONE(RENDER_RING_BASE)),
	       traceRead32(RING_EIR(RENDER_RING_BASE)),
	       traceRead32(RING_ESR(RENDER_RING_BASE)),
	       traceRead32(ERROR_GEN6),
	       traceRead32(GEN12_RING_FAULT_REG));

	for (uint32_t i = 0; i < 6; i++) {
		SYSLOG("ngreen", "TGLTrace[%s]: RCS_CSB[%u]=0x%08x%08x",
		       stage ? stage : "<null>", i,
		       traceRead32(RING_CONTEXT_STATUS_BUF_HI(RENDER_RING_BASE, i)),
		       traceRead32(RING_CONTEXT_STATUS_BUF(RENDER_RING_BASE, i)));
	}
}

void dumpGucRegisters(const char *stage) {
	if (!isTglTraceEnabled()) {
		return;
	}

	auto &state = gTglGpuBringupState;
	if (!Gen11::tmmioReady()) {
		SYSLOG("ngreen", "TGLTrace[%s]: GuC MMIO unavailable wopcmProgrammed=%d", stage ? stage : "<null>", state.gucWopcmProgrammed);
		return;
	}

	SYSLOG("ngreen", "TGLTrace[%s]: GUC_STATUS=0x%x GUC_SG_INTR_ENABLE=0x%x GUC_SG_INTR_MASK=0x%x GT_INTR_DW0=0x%x GT_INTR_DW1=0x%x INTR_IDENT0=0x%x INTR_IDENT1=0x%x IIR_SEL0=0x%x IIR_SEL1=0x%x wopcmProgrammed=%d",
	       stage ? stage : "<null>",
	       traceRead32(GUC_STATUS_REG),
	       traceRead32(GEN11_GUC_SG_INTR_ENABLE),
	       traceRead32(GEN11_GUC_SG_INTR_MASK),
	       traceRead32(GEN11_GT_INTR_DW0),
	       traceRead32(GEN11_GT_INTR_DW1),
	       traceRead32(GEN11_INTR_IDENTITY_REG0),
	       traceRead32(GEN11_INTR_IDENTITY_REG1),
	       traceRead32(GEN11_IIR_REG0_SELECTOR),
	       traceRead32(GEN11_IIR_REG1_SELECTOR),
	       state.gucWopcmProgrammed);
}

void dumpGtInterruptRegisters(const char *stage) {
	if (!isTglTraceEnabled()) {
		return;
	}

	if (!Gen11::tmmioReady()) {
		SYSLOG("ngreen", "TGLTrace[%s]: GT interrupt MMIO unavailable", stage ? stage : "<null>");
		return;
	}

	SYSLOG("ngreen", "TGLTrace[%s]: GFX_MSTR_IRQ=0x%x RENDER_COPY_INTR_ENABLE=0x%x RCS0_RSVD_INTR_MASK=0x%x GT_INTR_DW0=0x%x GT_INTR_DW1=0x%x GUC_SG_INTR_ENABLE=0x%x GUC_SG_INTR_MASK=0x%x",
	       stage ? stage : "<null>",
	       traceRead32(GEN11_GFX_MSTR_IRQ),
	       traceRead32(GEN11_RENDER_COPY_INTR_ENABLE),
	       traceRead32(GEN11_RCS0_RSVD_INTR_MASK),
	       traceRead32(GEN11_GT_INTR_DW0),
	       traceRead32(GEN11_GT_INTR_DW1),
	       traceRead32(GEN11_GUC_SG_INTR_ENABLE),
	       traceRead32(GEN11_GUC_SG_INTR_MASK));
}

void dumpTglGpuBringupState(const char *stage) {
	if (!isTglTraceEnabled()) {
		return;
	}

	updateTglGpuBringupStateFromRegisters(stage);

	auto &state = gTglGpuBringupState;
	SYSLOG("ngreen", "TGLTrace[%s]: isTgl9A49=%d allow3d=%d schedulerType=%d mmioReady=%d forceWakeOk=%d gtPowered=%d topologyOk=%d gucRequested=%d gucWopcmProgrammed=%d gucFirmwareLoaded=%d gucBooted=%d gucInterruptsEnabled=%d gucSubmissionReady=%d rcsResetSeen=%d rcsRingStarted=%d rcsContextReady=%d rcsInterruptsEnabled=%d rcsFirstSubmitSeen=%d rcsFirstCompletionSeen=%d lastGucStatus=0x%x lastRcsCtl=0x%x lastRcsHead=0x%x lastRcsTail=0x%x lastErrorGen6=0x%x",
	       stage ? stage : "<null>",
	       state.isTgl9A49, state.allow3d, state.schedulerType, state.mmioReady,
	       state.forceWakeOk, state.gtPowered, state.topologyOk, state.gucRequested,
	       state.gucWopcmProgrammed, state.gucFirmwareLoaded, state.gucBooted,
	       state.gucInterruptsEnabled, state.gucSubmissionReady, state.rcsResetSeen,
	       state.rcsRingStarted, state.rcsContextReady, state.rcsInterruptsEnabled,
	       state.rcsFirstSubmitSeen, state.rcsFirstCompletionSeen, state.lastGucStatus,
	       state.lastRcsCtl, state.lastRcsHead, state.lastRcsTail, state.lastErrorGen6);

	dumpRcsRegisters(stage);
	dumpGucRegisters(stage);
	dumpGtInterruptRegisters(stage);
}
