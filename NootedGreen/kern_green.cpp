//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#include "kern_green.hpp"
#include "kern_gen11.hpp"
#include "kern_genx.hpp"
#include "kern_model.hpp"
#include "DYLDPatches.hpp"
#include "HDMI.hpp"
#include "kern_patcherplus.hpp"
#include <Headers/kern_api.hpp>
#include <Headers/kern_devinfo.hpp>
#include <IOKit/IOCatalogue.h>


static const char *pathIOAcceleratorFamily2= "/System/Library/Extensions/IOAcceleratorFamily2.kext/Contents/MacOS/IOAcceleratorFamily2";
static const char *pathAGDP = "/System/Library/Extensions/AppleGraphicsControl.kext/Contents/PlugIns/"
							  "AppleGraphicsDevicePolicy.kext/Contents/MacOS/AppleGraphicsDevicePolicy";
static const char *pathBacklight = "/System/Library/Extensions/AppleBacklight.kext/Contents/MacOS/AppleBacklight";
static const char *pathMCCSControl = "/System/Library/Extensions/AppleMCCSControl.kext/Contents/MacOS/AppleMCCSControl";
static const char *pathIOGraphics= "/System/Library/Extensions/IOGraphicsFamily.kext/IOGraphicsFamily";

static KernelPatcher::KextInfo kextAGDP {"com.apple.driver.AppleGraphicsDevicePolicy", &pathAGDP, 1, {true}, {},
	KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextBacklight {"com.apple.driver.AppleBacklight", &pathBacklight, 1, {true}, {},
	KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextMCCSControl {"com.apple.driver.AppleMCCSControl", &pathMCCSControl, 1, {true}, {},
	KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextIOGraphics { "com.apple.iokit.IOGraphicsFamily", &pathIOGraphics, 1, {true}, {},
	KernelPatcher::KextInfo::Unloaded };
static KernelPatcher::KextInfo kextIOAcceleratorFamily2 { "com.apple.iokit.IOAcceleratorFamily2", &pathIOAcceleratorFamily2, 1, {true}, {},
	KernelPatcher::KextInfo::Unloaded };

NGreen *NGreen::callback = nullptr;

static Genx genx;
static Gen11 gen11;
static DYLDPatches dyldpatches;
static HDMI agfxhda;

static uint8_t builtin2[] = {0x00, 0x00, 0x49, 0x9A};
static uint8_t builtin3[] = {0x49, 0x9A, 0x00, 0x00};

static bool shouldForceRPLBringupPlatform(IORegistryEntry *entry) {
	auto *prop = OSDynamicCast(OSData, entry ? entry->getProperty("AAPL,ig-platform-id") : nullptr);
	if (!prop || prop->getLength() < sizeof(uint32_t)) {
		return true;
	}

	uint32_t platformId = *reinterpret_cast<const uint32_t *>(prop->getBytesNoCopy());
	// 0x8A5C0002 is undefined for the currently used TGL framebuffer path on this setup.
	return platformId == 0x8A5C0002U;
}

static bool shouldAutoSeedIGPUPropertiesEarly() {
	const char *paths[] = {
		"IOService:/AppleACPIPlatformExpert/PC00@0/IGPU@2",
		"IOService:/AppleACPIPlatformExpert/PC00@0/GFX0@2",
		"IOService:/AppleACPIPlatformExpert/PCI0@0/IGPU@2",
		"IOService:/AppleACPIPlatformExpert/PCI0@0/GFX0@2"
	};

	for (auto path : paths) {
		auto *entry = IORegistryEntry::fromPath(path, gIOServicePlane);
		if (!entry) {
			continue;
		}

		const bool shouldSeed = shouldForceRPLBringupPlatform(entry) || !entry->getProperty("device-id");
		entry->release();
		if (shouldSeed) {
			return true;
		}
	}

	return false;
}

static bool shouldEnableLegacyPllBringup() {
	int enabled = 0;
	if (PE_parse_boot_argn("ngreenlegacypll", &enabled, sizeof(enabled))) {
		return enabled != 0;
	}

	return checkKernelArgument("-ngreenlegacypll");
}

static bool hasAllow3DBootArg() {
	return checkKernelArgument("-allow3d");
}

static void logStampTimeoutTriage(const char *stage, int stampIndex, uint64_t stampValue, uint64_t gpuStamp) {
	if (!NGreen::callback || !NGreen::callback->hasMMIO()) {
		return;
	}
	Gen11::logRCSProgrammingSummary(stage ? stage : "stamp-timeout");

	SYSLOG("ngreen", "STAMP_TIMEOUT_TRIAGE[%s] idx=%d stamp=0x%llx gpu=0x%llx RCS[HEAD=0x%x TAIL=0x%x CTL=0x%x START=0x%x HWS=0x%x CTX_SIZE=0x%x CTX_CTRL=0x%x CCID=0x%x EXEC=0x%x CSB=0x%x] BCS[HEAD=0x%x TAIL=0x%x CTL=0x%x START=0x%x HWS=0x%x EXEC=0x%x CSB=0x%x] IRQ[MSTR=0x%x RC=0x%x RCS_MASK=0x%x BCS_MASK=0x%x ERR=0x%x]",
	       stage ? stage : "<null>", stampIndex,
	       static_cast<unsigned long long>(stampValue), static_cast<unsigned long long>(gpuStamp),
	       NGreen::callback->readMMIO32(RING_HEAD(RENDER_RING_BASE)),
	       NGreen::callback->readMMIO32(RING_TAIL(RENDER_RING_BASE)),
	       NGreen::callback->readMMIO32(RING_CTL(RENDER_RING_BASE)),
	       NGreen::callback->readMMIO32(RING_START(RENDER_RING_BASE)),
	       NGreen::callback->readMMIO32(RING_HWS_PGA(RENDER_RING_BASE)),
	       NGreen::callback->readMMIO32(RING_CTX_SIZE(RENDER_RING_BASE)),
	       NGreen::callback->readMMIO32(RING_CTX_CTRL(RENDER_RING_BASE)),
	       NGreen::callback->readMMIO32(RING_CCID(RENDER_RING_BASE)),
	       NGreen::callback->readMMIO32(RING_EXECLIST_STATUS(RENDER_RING_BASE)),
	       NGreen::callback->readMMIO32(RING_CONTEXT_STATUS_PTR(RENDER_RING_BASE)),
	       NGreen::callback->readMMIO32(RING_HEAD(BLT_RING_BASE)),
	       NGreen::callback->readMMIO32(RING_TAIL(BLT_RING_BASE)),
	       NGreen::callback->readMMIO32(RING_CTL(BLT_RING_BASE)),
	       NGreen::callback->readMMIO32(RING_START(BLT_RING_BASE)),
	       NGreen::callback->readMMIO32(RING_HWS_PGA(BLT_RING_BASE)),
	       NGreen::callback->readMMIO32(RING_EXECLIST_STATUS(BLT_RING_BASE)),
	       NGreen::callback->readMMIO32(RING_CONTEXT_STATUS_PTR(BLT_RING_BASE)),
	       NGreen::callback->readMMIO32(GEN11_GFX_MSTR_IRQ),
	       NGreen::callback->readMMIO32(GEN11_RENDER_COPY_INTR_ENABLE),
	       NGreen::callback->readMMIO32(GEN11_RCS0_RSVD_INTR_MASK),
	       NGreen::callback->readMMIO32(GEN11_BCS_RSVD_INTR_MASK),
	       NGreen::callback->readMMIO32(ERROR_GEN6));
}

static void publishTglFramebufferPersonality(IOPCIDevice *gpu) {
	auto *fbDict = OSDictionary::withCapacity(8);
	if (!fbDict) {
		return;
	}

	auto *bi = OSString::withCString("com.xxxxx.driver.AppleIntelTGLGraphicsFramebuffer");
	auto *cls = OSString::withCString("AppleIntelFramebufferController");
	auto *cat = OSString::withCString("IOFramebuffer");
	auto *prov = OSString::withCString("IOPCIDevice");
	auto *pciClass = OSString::withCString("0x03000000&0xff000000");
	auto *pciMatch = OSString::withCString("0x9A498086");
	auto *src = OSString::withCString("0.0.0.0.0");
	auto *score = OSNumber::withNumber(static_cast<unsigned long long>(80000), 32);

	if (bi) fbDict->setObject("CFBundleIdentifier", bi);
	if (cls) fbDict->setObject("IOClass", cls);
	if (cat) fbDict->setObject("IOMatchCategory", cat);
	if (prov) fbDict->setObject("IOProviderClass", prov);
	if (pciClass) fbDict->setObject("IOPCIClassMatch", pciClass);
	if (pciMatch) fbDict->setObject("IOPCIPrimaryMatch", pciMatch);
	if (src) fbDict->setObject("IOSourceVersion", src);
	if (score) fbDict->setObject("IOProbeScore", score);

	OSSafeReleaseNULL(bi);
	OSSafeReleaseNULL(cls);
	OSSafeReleaseNULL(cat);
	OSSafeReleaseNULL(prov);
	OSSafeReleaseNULL(pciClass);
	OSSafeReleaseNULL(pciMatch);
	OSSafeReleaseNULL(src);
	OSSafeReleaseNULL(score);

	auto *array = OSArray::withCapacity(1);
	if (array) {
		array->setObject(fbDict);
		auto *pciPrimaryMatch = OSDynamicCast(OSString, fbDict->getObject("IOPCIPrimaryMatch"));
		auto *providerClass = OSDynamicCast(OSString, fbDict->getObject("IOProviderClass"));
		auto *ioClass = OSDynamicCast(OSString, fbDict->getObject("IOClass"));
		SYSLOG("ngreen",
				"Publishing TGL framebuffer personality IOClass=%s provider=%s pciMatch=%s count=%u",
				ioClass ? ioClass->getCStringNoCopy() : "<null>",
				providerClass ? providerClass->getCStringNoCopy() : "<null>",
				pciPrimaryMatch ? pciPrimaryMatch->getCStringNoCopy() : "<null>",
				fbDict->getCount());
		if (gIOCatalogue) {
			const bool ok = gIOCatalogue->addDrivers(array, true);
			SYSLOG("ngreen", "Published TGL framebuffer personality for 0x9A49: %d", ok);
			if (gpu) {
				SYSLOG("ngreen", "Republishing IGPU after framebuffer personality: name=%s class=%s",
					gpu->getName(), gpu->getMetaClass()->getClassName());
				gpu->registerService();
			}
		} else {
			SYSLOG("ngreen", "Failed to publish TGL framebuffer personality: gIOCatalogue is null");
		}
		array->release();
	}

	fbDict->release();
}

static bool isLegacyIGPUPropSeedingEnabled() {
	int enabled = 0;
	if (PE_parse_boot_argn("ngreenforceprops", &enabled, sizeof(enabled))) {
		return enabled != 0;
	}

	return checkKernelArgument("-ngreenforceprops");
}

static bool seedIGPUPropertiesOnEntry(IORegistryEntry *entry) {
	if (!entry) {
		return false;
	}

	bool changed = false;

	// Default fallback platform-id for RPL/TGL spoof bring-up. Only inject when missing.
	if (!entry->getProperty("AAPL,ig-platform-id")) {
		entry->setProperty("AAPL,ig-platform-id", builtin2, arrsize(builtin2));
		changed = true;
	}

	if (!entry->getProperty("device-id")) {
		entry->setProperty("device-id", builtin3, arrsize(builtin3));
		changed = true;
	}

	if (!entry->getProperty("built-in")) {
		static uint8_t builtin[] = {0x00};
		entry->setProperty("built-in", builtin, arrsize(builtin));
		changed = true;
	}

	if (!entry->getProperty("AAPL,slot-name")) {
		entry->setProperty("AAPL,slot-name", const_cast<char *>("built-in"), 9);
		changed = true;
	}

	if (!entry->getProperty("hda-gfx")) {
		entry->setProperty("hda-gfx", const_cast<char *>("onboard-1"), 10);
		changed = true;
	}

	if (!entry->getProperty("model")) {
		entry->setProperty("model", const_cast<char *>("Intel Iris Xe Graphics"), 23);
		changed = true;
	}

	if (!entry->getProperty("framebuffer-unifiedmem")) {
		static uint8_t unifiedMem[] = {0x00, 0x00, 0x00, 0x60}; // 1536 MB
		entry->setProperty("framebuffer-unifiedmem", unifiedMem, arrsize(unifiedMem));
		changed = true;
	}

	if (!entry->getProperty("saved-config")) {
		static uint8_t sconf[] = {};
		entry->setProperty("saved-config", sconf, 0xea);
		changed = true;
	}

	return changed;
}

static void seedIGPUPropertiesEarly() {
	// Try common ACPI namespace variants used by laptop firmware before DeviceInfo scans.
	const char *paths[] = {
		"IOService:/AppleACPIPlatformExpert/PC00@0/IGPU@2",
		"IOService:/AppleACPIPlatformExpert/PC00@0/GFX0@2",
		"IOService:/AppleACPIPlatformExpert/PCI0@0/IGPU@2",
		"IOService:/AppleACPIPlatformExpert/PCI0@0/GFX0@2"
	};

	bool found = false;
	bool changed = false;
	for (auto path : paths) {
		auto *entry = IORegistryEntry::fromPath(path, gIOServicePlane);
		if (!entry) {
			continue;
		}
		found = true;
		if (seedIGPUPropertiesOnEntry(entry)) {
			changed = true;
		}
		entry->release();
	}

	if (found) {
		SYSLOG("ngreen", "Early IGPU pre-seed via IOService path: changed=%d", changed);
	} else {
		SYSLOG("ngreen", "Early IGPU pre-seed skipped: no IGPU path resolved before DeviceInfo");
	}
}

void NGreen::init() {
    callback = this;

	SYSLOG("ngreen", "============================================================");
	SYSLOG("ngreen", "NootedGreen V1.0.4 - STARTING");
    SYSLOG("ngreen", "============================================================");
	SYSLOG("ngreen", "Acceleration gate: -allow3d=%d", hasAllow3DBootArg());
	if (!hasAllow3DBootArg()) {
		SYSLOG("ngreen", "Acceleration gate: boot is missing -allow3d; acceleration probe results are not authoritative");
	}

    lilu.onKextLoadForce(&kextAGDP);
	/*lilu.onKextLoadForce(&kextBacklight);
	lilu.onKextLoadForce(&kextMCCSControl);
	lilu.onKextLoadForce(&kextIOGraphics);*/
	lilu.onKextLoadForce(&kextIOAcceleratorFamily2);
	
	genx.init();
	gen11.init();
	//agfxhda.init();
	dyldpatches.init();
	
    lilu.onPatcherLoadForce(
        [](void *user, KernelPatcher &patcher) { static_cast<NGreen *>(user)->processPatcher(patcher); }, this);
    lilu.onKextLoadForce(
        nullptr, 0,
        [](void *user, KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
            static_cast<NGreen *>(user)->processKext(patcher, index, address, size);
        },
        this);
	
}


void NGreen::processPatcher(KernelPatcher &patcher) {
	SYSLOG("ngreen", "[Stage 1] Starting processPatcher");

	// Hook _cs_validate_page FIRST — before DeviceInfo which blocks
	// for >60s polling PEGP (NVIDIA dGPU) disable via processSwitchOff.
	// Without this, WindowServer starts before the hook is established
	// and CoreDisplay loads unpatched from the shared cache.
	if (!checkKernelArgument("-nbdyldoff")) {
		dyldpatches.processPatcher(patcher);
	} else {
		DBGLOG("ngreen", "DYLD patches disabled by boot argument -nbdyldoff");
	}

	const bool autoSeedIGPUProps = shouldAutoSeedIGPUPropertiesEarly();
	const bool seedIGPUProps = isLegacyIGPUPropSeedingEnabled() || autoSeedIGPUProps;

	SYSLOG("ngreen", "[Stage 2] IGPU property seeding: seedIGPUProps=%d autoSeed=%d", seedIGPUProps, autoSeedIGPUProps);

	if (seedIGPUProps) {
		seedIGPUPropertiesEarly();
	} else {
		SYSLOG("ngreen", "compat mode: legacy IGPU property seeding disabled (use -ngreenforceprops to enable)");
	}

    auto *devInfo = DeviceInfo::create();
    if (devInfo) {
        devInfo->processSwitchOff();
		
		

		this->iGPU = OSDynamicCast(IOPCIDevice, devInfo->videoBuiltin);
		PANIC_COND(!this->iGPU, "ngreen", "videoBuiltin is not IOPCIDevice");

		SYSLOG("ngreen", "[Stage 3] IGPU matched: name=%s device-id=0x%04x",
		       this->iGPU->getName(), WIOKit::readPCIConfigValue(this->iGPU, WIOKit::kIOPCIConfigDeviceID));

		this->iGPU->enablePCIPowerManagement(kPCIPMCSPowerStateD0);
		this->iGPU->setBusMasterEnable(true);
		this->iGPU->setMemoryEnable(true);
		

		static uint8_t builtin[] = {0x00};

		WIOKit::renameDevice(this->iGPU, "IGPU");
		WIOKit::awaitPublishing(this->iGPU);

		if (seedIGPUProps) {
			seedIGPUPropertiesOnEntry(this->iGPU);
		}
		
		static uint8_t sconf[] = {};
		
		static uint8_t panel[] = {0x01, 0x00, 0x00, 0x00};
		/*static uint8_t panel1[] = {0x19, 0x01, 0x00, 0x00};
		static uint8_t panel2[] = {0x3c, 0x00, 0x00, 0x00};
		static uint8_t panel3[] = {0x11, 0x00, 0x00, 0x00};
		static uint8_t panel4[] = {0xfa, 0x00, 0x00, 0x00};

		this->iGPU->setProperty("AAPL00,PanelPowerUp", panel, arrsize(panel));
		this->iGPU->setProperty("AAPL00,PanelPowerOn", panel1, arrsize(panel1));
		this->iGPU->setProperty("AAPL00,PanelPowerDown", panel2, arrsize(panel2));
		this->iGPU->setProperty("AAPL00,PanelPowerOff", panel3, arrsize(panel3));
		this->iGPU->setProperty("AAPL00,PanelCycleDelay", panel4, arrsize(panel4));*/
		

		//this->iGPU->setProperty("@0,display-dither-support", panel, arrsize(panel));
		
		if (seedIGPUProps) {
			this->iGPU->setProperty("built-in", builtin, arrsize(builtin));
			this->iGPU->setProperty("AAPL,slot-name", const_cast<char *>("built-in"), 9);
			this->iGPU->setProperty("hda-gfx", const_cast<char *>("onboard-1"), 10);
			this->iGPU->setProperty("model", const_cast<char *>("Intel Iris Xe Graphics"), 23);

			auto *prop = OSDynamicCast(OSData, this->iGPU->getProperty("saved-config"));
			if (!prop) this->iGPU->setProperty("saved-config", sconf, 0xea);
		}

		uint32_t injectedDeviceId = 0, injectedPlatformId = 0, injectedVendorId = 0;
		WIOKit::getOSDataValue(this->iGPU, "device-id", injectedDeviceId);
		WIOKit::getOSDataValue(this->iGPU, "AAPL,ig-platform-id", injectedPlatformId);
		WIOKit::getOSDataValue(this->iGPU, "vendor-id", injectedVendorId);
		SYSLOG("ngreen", "[IGPU Props After Injection] device-id=0x%04x platform-id=0x%08x vendor-id=0x%04x",
		       injectedDeviceId, injectedPlatformId, injectedVendorId);
			
		//auto x = OSDynamicCast(OSData, this->iGPU->getProperty("AAPL,ig-platform-id"));
		//framebufferId = *(uint32_t*)x->getBytesNoCopy();
		
		//NETLOG("gen11", "framebufferId: = %x", framebufferId);
		//setRMMIOIfNecessary();

        this->deviceId = WIOKit::readPCIConfigValue(this->iGPU, WIOKit::kIOPCIConfigDeviceID);
        this->pciRevision = WIOKit::readPCIConfigValue(NGreen::callback->iGPU, WIOKit::kIOPCIConfigRevisionID);

        // V52: Detect real TGL vs spoofed RPL/ADL by CPU model
        // TGL-U: model 0x8C, TGL-H: model 0x8D
        // RPL-P: model 0xBA, RPL-S: model 0xBF, RPL-HX: model 0xB7
        // ADL-P: model 0x9A, ADL-S: model 0x97
        {
            uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
            asm volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(1));
            uint32_t family = (eax >> 8) & 0xF;
            uint32_t model = (eax >> 4) & 0xF;
            uint32_t extModel = (eax >> 16) & 0xF;
            uint32_t stepping = eax & 0xF;
            if (family == 0x6) model |= (extModel << 4);
            this->cpuModel = model;
            this->isRealTGL = (model == 0x8C || model == 0x8D);
            SYSLOG("ngreen", "V52: CPU family=0x%x model=0x%x stepping=%u isRealTGL=%d",
                   family, model, stepping, this->isRealTGL);
        }
		
		auto gms = WIOKit::readPCIConfigValue(devInfo->videoBuiltin, WIOKit::kIOPCIConfigGraphicsControl, 0, 16) >> 8;
		
		if (gms < 0x10) {
			stolen_size = gms * 32;
		} else if (gms == 0x20 || gms == 0x30 || gms == 0x40) {
			stolen_size = gms * 32;
		} else if (gms >= 0xF0 && gms <= 0xFE) {
			stolen_size = ((gms & 0x0F) + 1) * 4;
		} else {
			SYSLOG( "ngreen", "PANIC stolen_size=0 check DVMT in bios");
		}
		if (stolen_size<128) stolen_size=128;
		stolen_size *= (1024 * 1024);
		SYSLOG("ngreen", "stolen_size 0x%x",stolen_size);
		
		if (seedIGPUProps) {
			static uint8_t unifiedMem[] = {0x00, 0x00, 0x00, 0x60};
			this->iGPU->setProperty("framebuffer-unifiedmem", unifiedMem, arrsize(unifiedMem));
		}

		publishTglFramebufferPersonality(this->iGPU);

		SYSLOG("ngreen", "[Stage 4] Framebuffer personality published");

		static uint8_t accelProps[] = {0x01};
		this->iGPU->setProperty("IOPCIAccelerationGpu", accelProps, 1);

		auto *metalPluginName = OSString::withCString("AppleIntelTGLGraphics");
		auto *accelScaleDict = OSString::withCString("IOAccelScaleFactorDict");
		auto *accelShader = OSString::withCString("IOAccelTGLShaderBinary");
		auto *pciMatch = OSString::withCString("0x9A498086");
		auto *accelCategory = OSString::withCString("IOAccelGPU");

		if (metalPluginName) this->iGPU->setProperty("MetalPluginName", metalPluginName);
		if (accelScaleDict) this->iGPU->setProperty("IOAccelScaleFactorDict", accelScaleDict);
		if (accelShader) this->iGPU->setProperty("IOAccelDeviceShaderBinary", accelShader);
		if (pciMatch) this->iGPU->setProperty("IOPCIMatch", pciMatch);
		if (accelCategory) this->iGPU->setProperty("IOAccelDriverConnectCategory", accelCategory);

		OSSafeReleaseNULL(metalPluginName);
		OSSafeReleaseNULL(accelScaleDict);
		OSSafeReleaseNULL(accelShader);
		OSSafeReleaseNULL(pciMatch);
		OSSafeReleaseNULL(accelCategory);

		static uint8_t accelCaps[] = {0x00, 0x00, 0x10, 0x00};
		this->iGPU->setProperty("IOAccelCaps", accelCaps, 4);

		SYSLOG("ngreen", "[Accelerator Props] Published MetalPluginName=AppleIntelTGLGraphics and IOAccel properties");

		auto *metalPlugin = OSDynamicCast(OSString, this->iGPU->getProperty("MetalPluginName"));
		if (metalPlugin) {
			SYSLOG("ngreen", "[Accelerator Verify] MetalPluginName=%s", metalPlugin->getCStringNoCopy());
		}

		if (shouldEnableLegacyPllBringup()) {
			setRMMIOIfNecessary();
			constexpr uint32_t GEN9_PG_ENABLE = 0x46020;
			constexpr uint32_t LEGACY_BXT_DE_PLL_CTL = 0x6d000;

			writeReg32(GEN9_PG_ENABLE, 0);
			IODelay(50);
			writeReg32(BXT_DE_PLL_ENABLE, 0);
			IODelay(20);
			intel_de_rmw(LEGACY_BXT_DE_PLL_CTL, 0xFF, 60);
			writeReg32(BXT_DE_PLL_ENABLE, BXT_DE_PLL_PLL_ENABLE);

			int timeout = 2000;
			while (timeout--) {
				if (readReg32(BXT_DE_PLL_ENABLE) & BXT_DE_PLL_LOCK) {
					break;
				}
				IODelay(1);
			}
			SYSLOG("ngreen", "legacy PLL bring-up applied via -ngreenlegacypll");
		}
		
		KernelPatcher::routeVirtual(this->iGPU, WIOKit::PCIConfigOffset::ConfigRead16, configRead16, &orgConfigRead16);
		KernelPatcher::routeVirtual(this->iGPU, WIOKit::PCIConfigOffset::ConfigRead32, configRead32, &orgConfigRead32);

		SYSLOG("ngreen", "[Stage 5] PCI config read routing complete");
		SYSLOG("ngreen", "[NootedGreen Init Complete] All stages finished successfully");

        DeviceInfo::deleter(devInfo);
		

		
    } else {
        SYSLOG("ngreen", "Failed to create DeviceInfo");
    }
	
	/*KernelPatcher::RouteRequest request {"__ZN15OSMetaClassBase12safeMetaCastEPKS_PK11OSMetaClass", wrapSafeMetaCast,
		this->orgSafeMetaCast};
	PANIC_COND(!patcher.routeMultipleLong(KernelPatcher::KernelID, &request, 1), "ngreen",
		"Failed to route kernel symbols");*/
}

OSMetaClassBase *NGreen::wrapSafeMetaCast(const OSMetaClassBase *anObject, const OSMetaClass *toMeta) {
	auto ret = FunctionCast(wrapSafeMetaCast, callback->orgSafeMetaCast)(anObject, toMeta);
	if (UNLIKELY(!ret)) {
		for (const auto &ent : callback->metaClassMap) {
			if (LIKELY(ent[0] == toMeta)) {
				return FunctionCast(wrapSafeMetaCast, callback->orgSafeMetaCast)(anObject, ent[1]);
			} else if (UNLIKELY(ent[1] == toMeta)) {
				return FunctionCast(wrapSafeMetaCast, callback->orgSafeMetaCast)(anObject, ent[0]);
			}
		}
	}
	return ret;
}

void NGreen::setRMMIOIfNecessary() {
	if (UNLIKELY(!this->rmmio || !this->rmmio->getLength())) {
		this->rmmio = this->iGPU->mapDeviceMemoryWithRegister(kIOPCIConfigBaseAddress0);
		this->rmmioPtr = reinterpret_cast<volatile uint32_t *>(this->rmmio->getVirtualAddress());
	}
}

bool NGreen::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
	SYSLOG("ngreen", "[Accelerator Tracer] processKext index=%zu", index);

	if (kextIOAcceleratorFamily2.loadIndex == index) {
		SYSLOG("NGreen", "IOAccelF2: TEXT 0x%llx size 0x%lx", address, size);
		
		// ── V40: f2 FIXED based on V39 diagnostics ──
		// V39 revealed: Sonoma uses push r12; push rbx; push rax (41 54 53 50)
		// as function prologue instead of sub rsp,imm8 (48 83 ec).
		// Two matching test edx,imm32; je found at +0x38b0 and +0x102e6.
		// Patching both (count=2) to bypass capability checks.
		static const uint8_t f2_f[]  = {0x41, 0x54, 0x53, 0x50, 0xf7, 0xc2, 0x00, 0x00, 0x00, 0x00, 0x74, 0x00};
		static const uint8_t f2_m[]  = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00};
		static const uint8_t f2_r[]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEB, 0x00};
		static const uint8_t f2_rm[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00};
		const LookupPatchPlus p2 {&kextIOAcceleratorFamily2, f2_f, f2_m, f2_r, f2_rm, 2};
		bool f2ok = p2.apply(patcher, address, size);
		patcher.clearError();
		SYSLOG("NGreen", "IOAccelF2 f2 (fixed): %s", f2ok ? "OK" : "FAILED");
		
		// ── V41: f1 FIXED based on V40 diagnostics ──
		// V40 revealed: Sonoma has jne (0x75) at -16 from mov r9d,[r15+0x284],
		// NOT je (0x74) at -2 as the original pattern assumed.
		// Code structure: cmp eax,ebx; jne +0x39; <device-specific setup>; mov r9d,[r15+0x284]
		// This is a device-ID capability check: vtable call returns ID, compared with expected.
		// NOP the jne (75 XX → 90 90) so our device 0x9A49 always falls through.
		// Pattern: jne XX; mov rax,[r15+disp32]; mov r8,[rax+disp32]; mov r9d,[r15+disp32]
		static const uint8_t f1_f[]  = {0x75, 0x00, 0x49, 0x8b, 0x87, 0x00, 0x00, 0x00, 0x00, 0x4c, 0x8b, 0x80, 0x00, 0x00, 0x00, 0x00, 0x45, 0x8b, 0x8f};
		static const uint8_t f1_m[]  = {0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF};
		static const uint8_t f1_r[]  = {0x90, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		static const uint8_t f1_rm[] = {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		const LookupPatchPlus p1 {&kextIOAcceleratorFamily2, f1_f, f1_m, f1_r, f1_rm, 1};
		bool f1ok = p1.apply(patcher, address, size);
		patcher.clearError();
		SYSLOG("NGreen", "IOAccelF2 f1 (fixed): %s", f1ok ? "OK" : "FAILED");

		if (isIOAccelStampTraceEnabled()) {
			RouteRequestPlus stampRoutes[] = {
				{"__ZN22IOAccelEventMachineFast213setEventStampElPy", wrapSetEventStamp, callback->orgSetEventStamp},
				{"__ZN22IOAccelEventMachineFast217setEventStampWaitEiPym", wrapSetEventStampWait, callback->orgSetEventStampWait},
				{"__ZN22IOAccelEventMachineFast211finishStampEl", wrapFinishStamp, callback->orgFinishStamp},
				{"__ZN22IOAccelEventMachineFast215finishAllStampsEl", wrapFinishAllStamps, callback->orgFinishAllStamps},
				{"__ZN22IOAccelEventMachineFast29testStampEi", wrapTestStamp, callback->orgTestStamp},
				{"__ZN22IOAccelEventMachineFast213testAllStampsEv", wrapTestAllStamps, callback->orgTestAllStamps},
				{"__ZN22IOAccelEventMachineFast215waitForAnyStampEmm", wrapWaitForAnyStamp, callback->orgWaitForAnyStamp},
				{"__ZN22IOAccelEventMachineFast217getSubmittedStampEi", wrapGetSubmittedStamp, callback->orgGetSubmittedStamp},
			};
			const bool stampOk = RouteRequestPlus::routeAll(patcher, index, stampRoutes, address, size);
			patcher.clearError();
			SYSLOG("NGreen", "IOAccelF2 stamp tracing routes: %s", stampOk ? "active" : "partial/unavailable");
			bool fallbackAny = false;
			{
				RouteRequestPlus req0 {"__ZN20IOAccelEventMachine212waitForStampEijPj", wrapWaitForStamp2, callback->orgWaitForStamp2};
				RouteRequestPlus req1 {"__ZN20IOAccelEventMachine212waitForStampEixPj", wrapWaitForStamp2, callback->orgWaitForStamp2};
				RouteRequestPlus req2 {"__ZN20IOAccelEventMachine212waitForStampExiPj", wrapWaitForStamp2, callback->orgWaitForStamp2};
				if (req0.route(patcher, index, address, size) || req1.route(patcher, index, address, size) || req2.route(patcher, index, address, size)) {
					SYSLOG("NGreen", "IOAccelF2 fallback route waitForStamp active");
					fallbackAny = true;
				} else {
					patcher.clearError();
					SYSLOG("NGreen", "IOAccelF2 fallback route waitForStamp unavailable");
				}
			}
			{
				RouteRequestPlus req0 {"__ZN20IOAccelEventMachine226handleFinishChannelRestartEiij", wrapHandleFinishChannelRestart, callback->orgHandleFinishChannelRestart};
				RouteRequestPlus req1 {"__ZN20IOAccelEventMachine226handleFinishChannelRestartExjj", wrapHandleFinishChannelRestart, callback->orgHandleFinishChannelRestart};
				RouteRequestPlus req2 {"__ZN20IOAccelEventMachine226handleFinishChannelRestartExij", wrapHandleFinishChannelRestart, callback->orgHandleFinishChannelRestart};
				if (req0.route(patcher, index, address, size) || req1.route(patcher, index, address, size) || req2.route(patcher, index, address, size)) {
					SYSLOG("NGreen", "IOAccelF2 fallback route handleFinishChannelRestart active");
					fallbackAny = true;
				} else {
					patcher.clearError();
					SYSLOG("NGreen", "IOAccelF2 fallback route handleFinishChannelRestart unavailable");
				}
			}
			{
				RouteRequestPlus req0 {"__ZN20IOAccelEventMachine215restart_channelEv", wrapRestartChannel2, callback->orgRestartChannel2};
				if (req0.route(patcher, index, address, size)) {
					SYSLOG("NGreen", "IOAccelF2 fallback route restart_channel active");
					fallbackAny = true;
				} else {
					patcher.clearError();
					SYSLOG("NGreen", "IOAccelF2 fallback route restart_channel unavailable");
				}
			}
			SYSLOG("NGreen", "IOAccelF2 EventMachine2 fallback tracing: %s", fallbackAny ? "active" : "unavailable");
		}
		
	}  else if (kextIOGraphics.loadIndex == index) {
		/*
		KernelPatcher::RouteRequest requests[] = {
				{"__ZN13IOFramebuffer25extValidateDetailedTimingEP8OSObjectPvP25IOExternalMethodArguments", wrapValidateDetailedTiming},
			};
			patcher.routeMultiple(index, requests, address, size);
			patcher.clearError();*/
		
	}  else if (kextAGDP.loadIndex == index) {
		const LookupPatchPlus patch {&kextAGDP, kAGDPBoardIDKeyOriginal, kAGDPBoardIDKeyPatched, 1};
		SYSLOG_COND(!patch.apply(patcher, address, size), "NGreen", "Failed to apply AGDP board-id patch");

		/*if (getKernelVersion() == KernelVersion::Ventura) {
			const LookupPatchPlus patch {&kextAGDP, kAGDPFBCountCheckVenturaOriginal, kAGDPFBCountCheckVenturaPatched,
				1};
			SYSLOG_COND(!patch.apply(patcher, address, size), "NGreen", "Failed to apply AGDP fb count check patch");
		} else {
			const LookupPatchPlus patch {&kextAGDP, kAGDPFBCountCheckOriginal, kAGDPFBCountCheckPatched, 1};
			SYSLOG_COND(!patch.apply(patcher, address, size), "NGreen", "Failed to apply AGDP fb count check patch");
		}*/
	}  else if (kextBacklight.loadIndex == index) {
		/*KernelPatcher::RouteRequest request {"__ZN15AppleIntelPanel10setDisplayEP9IODisplay", wrapApplePanelSetDisplay,
	  orgApplePanelSetDisplay};
			if (patcher.routeMultiple(kextBacklight.loadIndex, &request, 1, address, size)) {
				const UInt8 find[] = {"F%uT%04x"};
				const UInt8 replace[] = {"F%uTxxxx"};
				const LookupPatchPlus patch {&kextBacklight, find, replace, 1};
				SYSLOG_COND(!patch.apply(patcher, address, size), "NGreen", "Failed to apply backlight patch");
			}*/
} else if (kextMCCSControl.loadIndex == index) {
		/*KernelPatcher::RouteRequest requests[] = {
				{"__ZN25AppleMCCSControlGibraltar5probeEP9IOServicePi", wrapFunctionReturnZero},
				{"__ZN21AppleMCCSControlCello5probeEP9IOServicePi", wrapFunctionReturnZero},
			};
			patcher.routeMultiple(index, requests, address, size);
			patcher.clearError();*/
} else if (genx.processKext(patcher, index, address, size)) {
	DBGLOG("ngreen", "Processed Generation x configuration");
} else if (gen11.processKext(patcher, index, address, size)) {
        DBGLOG("ngreen", "Processed Generation 11 configuration");
    } /*else if (agfxhda.processKext(patcher, index, address, size)) {
		DBGLOG("ngreen", "Processed AppleGFXHDA");
	}*/
    return true;
}

bool NGreen::isIOAccelStampTraceEnabled() {
	return checkKernelArgument("-NGreenDebug") || checkKernelArgument("-liludbg");
}

void NGreen::logIOAccelStampTrace(const char *func, void *that, int stampIndex, uint64_t submittedStamp,
	uint64_t finishedStamp, uint64_t auxValue, const char *ownerClass) {
	if (!callback || !isIOAccelStampTraceEnabled()) {
		return;
	}

	const uint32_t rcsHead = callback->readReg32(RING_HEAD(RENDER_RING_BASE));
	const uint32_t rcsTail = callback->readReg32(RING_TAIL(RENDER_RING_BASE));
	const uint32_t rcsCtl = callback->readReg32(RING_CTL(RENDER_RING_BASE));
	const uint32_t bcsHead = callback->readReg32(RING_HEAD(BLT_RING_BASE));
	const uint32_t bcsTail = callback->readReg32(RING_TAIL(BLT_RING_BASE));
	const uint32_t bcsCtl = callback->readReg32(RING_CTL(BLT_RING_BASE));
	SYSLOG("ngreen", "STAMP %s obj=0x%llx class=%s idx=%d submitted=0x%llx finished=0x%llx aux=0x%llx RCS[h=0x%x t=0x%x c=0x%x] BCS[h=0x%x t=0x%x c=0x%x] IRQ[m=0x%x rc=0x%x err=0x%x]",
	       func ? func : "<null>", static_cast<unsigned long long>(reinterpret_cast<uintptr_t>(that)),
	       ownerClass ? ownerClass : "<unknown>", stampIndex,
	       static_cast<unsigned long long>(submittedStamp), static_cast<unsigned long long>(finishedStamp),
	       static_cast<unsigned long long>(auxValue),
	       rcsHead, rcsTail, rcsCtl, bcsHead, bcsTail, bcsCtl,
	       callback->readReg32(GEN11_GFX_MSTR_IRQ),
	       callback->readReg32(GEN11_RENDER_COPY_INTR_ENABLE),
	       callback->readReg32(ERROR_GEN6));
}

uint64_t NGreen::wrapSetEventStamp(void *that, uint64_t stampIndex, uint64_t *stampStorage) {
	const uint64_t ret = FunctionCast(wrapSetEventStamp, callback->orgSetEventStamp)(that, stampIndex, stampStorage);
	logIOAccelStampTrace("setEventStamp", that, static_cast<int>(stampIndex), stampStorage ? *stampStorage : 0, ret, 0);
	return ret;
}

uint64_t NGreen::wrapSetEventStampWait(void *that, int stampIndex, uint64_t *stampStorage, unsigned int timeout) {
	const uint64_t ret = FunctionCast(wrapSetEventStampWait, callback->orgSetEventStampWait)(that, stampIndex, stampStorage, timeout);
	logIOAccelStampTrace("setEventStampWait", that, stampIndex, stampStorage ? *stampStorage : 0, ret, timeout);
	return ret;
}

uint64_t NGreen::wrapWaitForStamp2(void *that, int stampIndex, uint64_t stampValue, uint32_t *gpuStampOut) {
	const uint64_t ret = FunctionCast(wrapWaitForStamp2, callback->orgWaitForStamp2)(that, stampIndex, stampValue, gpuStampOut);
	const uint64_t gpuStamp = gpuStampOut ? *gpuStampOut : 0;
	logIOAccelStampTrace("waitForStamp2", that, stampIndex, stampValue, gpuStamp, ret);
	if (ret != 0) {
		logStampTimeoutTriage("waitForStamp timeout", stampIndex, stampValue, gpuStamp);
	}
	return ret;
}

void NGreen::wrapHandleFinishChannelRestart(void *that, uint64_t result, unsigned int stampIdx, unsigned int restartType) {
	logIOAccelStampTrace("handleFinishChannelRestart", that, static_cast<int>(stampIdx), restartType, result, 0);
	logStampTimeoutTriage("handleFinishChannelRestart", static_cast<int>(stampIdx), restartType, result);
	FunctionCast(wrapHandleFinishChannelRestart, callback->orgHandleFinishChannelRestart)(that, result, stampIdx, restartType);
}

uint64_t NGreen::wrapRestartChannel2(void *that) {
	logIOAccelStampTrace("restart_channel", that, -1, 0, 0, 0);
	logStampTimeoutTriage("restart_channel", -1, 0, 0);
	return FunctionCast(wrapRestartChannel2, callback->orgRestartChannel2)(that);
}

uint64_t NGreen::wrapFinishStamp(void *that, uint64_t stampIndex) {
	const uint64_t ret = FunctionCast(wrapFinishStamp, callback->orgFinishStamp)(that, stampIndex);
	logIOAccelStampTrace("finishStamp", that, static_cast<int>(stampIndex), ret, ret, 0);
	return ret;
}

uint64_t NGreen::wrapFinishAllStamps(void *that, uint64_t value) {
	const uint64_t ret = FunctionCast(wrapFinishAllStamps, callback->orgFinishAllStamps)(that, value);
	logIOAccelStampTrace("finishAllStamps", that, -1, ret, ret, value);
	return ret;
}

bool NGreen::wrapTestStamp(void *that, int stampIndex) {
	const bool ret = FunctionCast(wrapTestStamp, callback->orgTestStamp)(that, stampIndex);
	logIOAccelStampTrace("testStamp", that, stampIndex, ret, ret, 0);
	return ret;
}

char NGreen::wrapTestAllStamps(void *that) {
	const char ret = FunctionCast(wrapTestAllStamps, callback->orgTestAllStamps)(that);
	logIOAccelStampTrace("testAllStamps", that, -1, ret, ret, 0);
	return ret;
}

uint64_t NGreen::wrapWaitForAnyStamp(void *that, uint64_t stampMask, uint64_t timeout) {
	const uint64_t ret = FunctionCast(wrapWaitForAnyStamp, callback->orgWaitForAnyStamp)(that, stampMask, timeout);
	logIOAccelStampTrace("waitForAnyStamp", that, -1, stampMask, ret, timeout);
	return ret;
}

uint64_t NGreen::wrapGetSubmittedStamp(void *that, int stampIndex) {
	const uint64_t ret = FunctionCast(wrapGetSubmittedStamp, callback->orgGetSubmittedStamp)(that, stampIndex);
	logIOAccelStampTrace("getSubmittedStamp", that, stampIndex, ret, ret, 0);
	return ret;
}



uint16_t NGreen::configRead16(IORegistryEntry *service, uint32_t space, uint8_t offset) {
	if (callback && callback->orgConfigRead16) {
		auto result = callback->orgConfigRead16(service, space, offset);
		if (offset == WIOKit::kIOPCIConfigDeviceID && service != nullptr) {
			auto name = service->getName();
			if (name && name[0] == 'I' && name[1] == 'G' && name[2] == 'P' && name[3] == 'U') {
				uint32_t device;
				if (WIOKit::getOSDataValue(service, "device-id", device) && device != result) {
					return device;
				}
			}
		}

		return result;
	}

	return 0;
}

uint32_t NGreen::configRead32(IORegistryEntry *service, uint32_t space, uint8_t offset) {
	if (callback && callback->orgConfigRead32) {
		auto result = callback->orgConfigRead32(service, space, offset);
		// According to lvs unaligned reads may happen
		if ((offset == WIOKit::kIOPCIConfigDeviceID || offset == WIOKit::kIOPCIConfigVendorID) && service != nullptr) {
			auto name = service->getName();
			if (name && name[0] == 'I' && name[1] == 'G' && name[2] == 'P' && name[3] == 'U') {
				uint32_t device;
				if (WIOKit::getOSDataValue(service, "device-id", device) && device != (result & 0xFFFF)) {
					device = (result & 0xFFFF) | (device << 16);
					return device;
				}
			}
		}

		return result;
	}

	return 0;
}

size_t NGreen::wrapFunctionReturnZero() { return 0; }

struct ApplePanelData {
	const char *deviceName;
	UInt8 deviceData[36];
};

static ApplePanelData appleBacklightData[] = {
	{"F14Txxxx", {0x00, 0x11, 0x00, 0x00, 0x00, 0x34, 0x00, 0x52, 0x00, 0x73, 0x00, 0x94, 0x00, 0xBE, 0x00, 0xFA, 0x01,
					 0x36, 0x01, 0x72, 0x01, 0xC5, 0x02, 0x2F, 0x02, 0xB9, 0x03, 0x60, 0x04, 0x1A, 0x05, 0x0A, 0x06,
					 0x0E, 0x07, 0x10}},
	{"F15Txxxx", {0x00, 0x11, 0x00, 0x00, 0x00, 0x36, 0x00, 0x54, 0x00, 0x7D, 0x00, 0xB2, 0x00, 0xF5, 0x01, 0x49, 0x01,
					 0xB1, 0x02, 0x2B, 0x02, 0xB8, 0x03, 0x59, 0x04, 0x13, 0x04, 0xEC, 0x05, 0xF3, 0x07, 0x34, 0x08,
					 0xAF, 0x0A, 0xD9}},
	{"F16Txxxx", {0x00, 0x11, 0x00, 0x00, 0x00, 0x18, 0x00, 0x27, 0x00, 0x3A, 0x00, 0x52, 0x00, 0x71, 0x00, 0x96, 0x00,
					 0xC4, 0x00, 0xFC, 0x01, 0x40, 0x01, 0x93, 0x01, 0xF6, 0x02, 0x6E, 0x02, 0xFE, 0x03, 0xAA, 0x04,
					 0x78, 0x05, 0x6C}},
	{"F17Txxxx", {0x00, 0x11, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x34, 0x00, 0x4F, 0x00, 0x71, 0x00, 0x9B, 0x00, 0xCF, 0x01,
					 0x0E, 0x01, 0x5D, 0x01, 0xBB, 0x02, 0x2F, 0x02, 0xB9, 0x03, 0x60, 0x04, 0x29, 0x05, 0x1E, 0x06,
					 0x44, 0x07, 0xA1}},
	{"F18Txxxx", {0x00, 0x11, 0x00, 0x00, 0x00, 0x53, 0x00, 0x8C, 0x00, 0xD5, 0x01, 0x31, 0x01, 0xA2, 0x02, 0x2E, 0x02,
					 0xD8, 0x03, 0xAE, 0x04, 0xAC, 0x05, 0xE5, 0x07, 0x59, 0x09, 0x1C, 0x0B, 0x3B, 0x0D, 0xD0, 0x10,
					 0xEA, 0x14, 0x99}},
	{"F19Txxxx", {0x00, 0x11, 0x00, 0x00, 0x02, 0x8F, 0x03, 0x53, 0x04, 0x5A, 0x05, 0xA1, 0x07, 0xAE, 0x0A, 0x3D, 0x0E,
					 0x14, 0x13, 0x74, 0x1A, 0x5E, 0x24, 0x18, 0x31, 0xA9, 0x44, 0x59, 0x5E, 0x76, 0x83, 0x11, 0xB6,
					 0xC7, 0xFF, 0x7B}},
	{"F24Txxxx", {0x00, 0x11, 0x00, 0x01, 0x00, 0x34, 0x00, 0x52, 0x00, 0x73, 0x00, 0x94, 0x00, 0xBE, 0x00, 0xFA, 0x01,
					 0x36, 0x01, 0x72, 0x01, 0xC5, 0x02, 0x2F, 0x02, 0xB9, 0x03, 0x60, 0x04, 0x1A, 0x05, 0x0A, 0x06,
					 0x0E, 0x07, 0x10}},
};

bool NGreen::wrapApplePanelSetDisplay(IOService *that, IODisplay *display) {
	static bool once = false;
	if (!once) {
		once = true;
		auto *panels = OSDynamicCast(OSDictionary, that->getProperty("ApplePanels"));
		if (panels) {
			auto *rawPanels = panels->copyCollection();
			panels = OSDynamicCast(OSDictionary, rawPanels);

			if (panels) {
				for (auto &entry : appleBacklightData) {
					auto pd = OSData::withBytes(entry.deviceData, sizeof(entry.deviceData));
					if (pd) {
						panels->setObject(entry.deviceName, pd);
						//! No release required by current AppleBacklight implementation.
					} else {
					}
				}
				that->setProperty("ApplePanels", panels);
			}

			OSSafeReleaseNULL(rawPanels);
		} else {
		}
	}

	bool ret = FunctionCast(wrapApplePanelSetDisplay, callback->orgApplePanelSetDisplay)(that, display);
	return ret;
}
