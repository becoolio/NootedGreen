# NootedGreen Implementation Plan — Real TGL (0x9A498086) on macOS Sonoma

## Current Status: Phase 1 Code Complete, Pre-Boot Verification Done

**Last updated: 2026-05-25**

| Capability | Status |
|---|---|
| macOS Sonoma 14.8.3 boot to login screen | ✅ Working |
| Internal display 1920x1080 @ 60Hz | ✅ Working |
| DMC firmware v2.12 loaded | ✅ Working |
| Power wells, forcewake, MMIO access | ✅ Working |
| eDP link training (linkRate=24, bpp=10, laneCount=2) | ✅ Working |
| AUX/DPCD communication | ✅ Working |
| Host scheduler (sched=5, -disablegfxfirmware) | ✅ Working |
| ERROR_GEN6=0x37 cleared before registerService | ✅ Fixed (kern_gen11.cpp:4836) |
| Platform-ID 0x9A490000 injected | ✅ Fixed (kern_green.cpp:337) |
| Backlight (AppleBacklight + MCCS disable + 11 panel LUT entries) | ✅ Implemented (kern_green.cpp) |
| DTK HW kext (AppleIntelTGLGraphics.kext) verified & ready | ✅ Verified (Apple-signed, 1.8MB, 2922 symbols) |
| 6 AppleIntelFramebufferController export stubs in NootedGreen | ✅ Built & confirmed (nm -Ug) |
| Route audit (37 routes, kPANIC on failure) | ✅ Complete |
| vtable hierarchy audit (4 kexts, all correct) | ✅ Complete |
| DTK source verification (backlight, platform-ID, registers) | ✅ Complete |
| Hybrid kext setup (DTK HW + le/ FB via HookCase) | ✅ Configured |
| -allow3d (full 3D acceleration) | ❌ Not yet tested |
| CoreDisplay hash table crash | ⏳ Not yet verified |
| **WindowServer high CPU (126%) on internal EFI boot** | 🔴 **Diagnosed — HW kext missing** |

---

## Real-Time Diagnostic: Internal EFI Fallback Boot (2026-05-25)

**Observed symptom:** WindowServer using 126.8% CPU at idle, UI sluggish

### IORegistry / Kext State on Current Boot
```
# Loaded graphics kexts:
    as.vit9696.Lilu (1.7.3)                              ✓ (OpenCore injection)
    com.StezzaPilot.NootedGreen (1.0.4)                  ✓ (OpenCore injection)
    com.xxxxx.driver.AppleIntelTGLGraphicsFramebuffer     ✓ (/Library/Extensions/, le/ FB)
    com.apple.iokit.IOAcceleratorFamily2                  ✓ (framework only)
  ⚠ com.apple.driver.AppleIntelTGLGraphics            ❌ NOT LOADED — not installed on /S/L/E

# IGPU IORegistry key properties:
    "device-id" = <499a0000>        ✓ 0x9A49
    "model" = "Intel HD Graphics TGL CRB"   ✓ (from ACPI)
    "IOPCIAccelerationGpu" = <01>   ✓
    "IOAccelCaps" = <00001000>      ⚠ Minimal caps — no real acceleration
    "MetalPluginName" = "AppleIntelTGLGraphics"  ✓ (name via NootedGreen)
  ⚠ "AAPL,ig-platform-id" = ABSENT — NootedGreen v1.0.4 predates platform-ID fix

# No IOGPUDevice service:         ⚠ No HW kext provider
# No IOAccelerator service:       ⚠ No GPU acceleration
# No GL/MTL driver kexts:         ⚠ Not loaded (depend on IOGPUDevice matching)

# Boot-args:
    itlwm_cc=US -v keepsyms=1 debug=0x100 -NGreenDebug ngreen-dmc=tgl
    -disablegfxfirmware ngreenSched=5
  ⚠ Missing -ngreenforceprops (needed for property injection)
```

### Root Cause of WindowServer 126% CPU
1. **HW kext (AppleIntelTGLGraphics.kext) is NOT installed** on the system volume
2. Without it, no IOGPUDevice provider exists → no IOAccelerator created
3. FB kext loads in isolation — display works at 1920x1080 via CPU fallback
4. WindowServer detects no GPU → falls back to **software compositing**
5. All UI rendering done on CPU → 126.8% CPU at idle for 1080p
6. `system_profiler` reports "Metal: Supported" from IORegistry property but no real GPU device backs it — Metal calls would be NULL-path or fail silently

### Why Our Test USB EFI Won't Have This Problem
- HW kext injected via OpenCore (or installed to /S/L/E on test volume)
- NootedGreen with platform-ID fix injects `AAPL,ig-platform-id` = 0x9A490000
- Boot-args include `-ngreenforceprops` to enable property injection
- With HW kext + platform-ID → IOGPUDevice → IOAccelerator → GL/MTL kexts → real acceleration

### What We Verified Is Correct for Our Setup
- **DTK HW kext** Apple-signed (com.apple.driver.AppleIntelTGLGraphics, 1.8MB, 2922 exported symbols) — confirmed valid in `sle_Internal/sle/`
- **Platform-ID** value 0x9A490000 (bytes `{0x00,0x00,0x49,0x9A}`) — matches DTK `getPlatformID()` fallback return value 2588475392
- **ERROR_GEN6** register 0x40A0 register procedure — R/W type verified via MMIO sweep
- **Backlight registers** 0xC8250 (enable), 0xC8254 (frequency), 0xC8258 (duty cycle) — confirmed in DTK `hwSetBacklight` and `CamelliaBase::SetDPCDBacklight`
- **DPCD address** 0x722 — confirmed in DTK `SetDPCDBacklight` AUX write
- **Export stubs** — all 6 symbols match DTK declarations exactly (nm -Ug confirmed)
- **37 route symbols** — all confirmed present in DTK source with matching mangled names
- **AppleMCCSControl::probe** return-zero — correct approach verified; MCCS is a separate kext competing for backlight ownership via AppleMCCSParameterHandler protocol

---

## Phase 0: Codebase Simplification (DONE)

All Phase 0 steps completed:

- **0.1** — ICL kext monitoring: `kern_genx.cpp`, `kern_genx.hpp` removed. ICL callbacks stripped from `kern_start.cpp`.
- **0.2** — RPL/ADL spoof path: `isRealTGL` and all branching, topology hardcodes, BCS bypass, DPCD clamping, IRQ watchdog, EMR enforcer, GPU health monitor, BCS stop+clear, TLB invalidation, ERROR_GEN6 R/W clear (moved to Phase 1 fix), MultiForceWakeSelect=1, Master IRQ pre-enable all removed. `IntelAccelerator::start()` simplified.
- **0.3** — Unnecessary DYLD patches: `GetMTLTexture` NULL stub, `GetMTLCommandQueue` NULL stub, `AccessComplete` skip, `Display::Present` skip, ICL Metal device-ID bypass, CoreLSKD CPUID patch, SkyLight bypass all removed. `RunFullDisplayPipe` NULL vcall guard kept but stage-gated.
- **0.4** — Ventura build artifacts: All `#if` kernel version checks, Ventura binary patch variants, Ventura DYLD patch variants removed.
- **0.5** — Support files: `FirmwareADLP.cpp` removed. ICL boot-args removed.

---

## Phase 1: Fix Boot Errors (DONE — Code Complete, Awaits Boot Test)

### Fix 1: ERROR_GEN6=0x37
- **Root cause:** Register 0x40A0 (ERROR_GEN6 per Intel i915 docs) holds stale GT-level error state from early init. Apple's DTK driver never touches this register — Apple only clears display error registers 0x44030/0x44038/0x44050/0x44054 in `hwEnableInterrupts`. (DTK source confirmation: no reference to register 0x40A0 exists anywhere in AppleIntelTGLGraphicsFramebuffer.c or AppleIntelTGLGraphics_kasan.c.)
- **Fix:** Added read-and-clear of ERROR_GEN6 (write 0x0, proven R/W by register sweep) before `service->registerService()` in the fbcontroller path (`kern_gen11.cpp:4836-4841`).
- **Verification:** Boot log will show ERROR_GEN6=0x0 at registerService time.

### Fix 2: Platform-ID=0x00000000
- **Root cause:** `seedIGPUPropertiesOnEntry()` was conditionally skipped (gated by `isRealTGL` flag which was only set from `getPlatformID()` — a chicken-and-egg problem). `ogetPlatformID` function exists in header but is dead code (never referenced in any RouteRequest array).
- **Fix:** Made `seedIGPUPropertiesOnEntry()` unconditional. Injects `AAPL,ig-platform-id` = `{0x00, 0x00, 0x49, 0x9A}` = 0x9A490000 little-endian (verified from DTK source `getPlatformID()` fallback returns 2588475392 = 0x9A490000) into IORegistry (`kern_green.cpp:337-338`).
- **Verification:** `ioreg -p IODeviceTree -n IGPU -r | grep platform-id` will show 0x9a490000.

### Fix 3: Hybrid Kext Setup (DTK HW + le/ FB)
- **Root cause:** The `le/` FB kext in `/Library/Extensions/` has `org.smichaud.HookCase` dependency (HookCase-instrumented, 966KB larger at 3.9MB vs DTK's 2.9MB). The DTK FB kext cannot load on Sonoma due to 6 unresolved AppleIntelFramebufferController symbols that no loaded kext provides.
- **Fix:** **Dual-kext approach:**
  - **HW:** Apple-signed DTK AppleIntelTGLGraphics.kext (com.apple.driver.*) — installed to /S/L/E/
  - **FB:** HookCase-dependent le/ FB kext (com.xxxxx.driver.*) — stays at /Library/Extensions/
  - **Export stubs:** NootedGreen exports the 6 AppleIntelFramebufferController symbols the DTK FB needs (for future DTK FB enablement)
- **Verification:** `kextstat | grep AppleIntelTGLGraphics` shows HW loaded from /S/L/E, FB from /Library/Extensions/

### Fix 4: Backlight Re-enabled
- **Root cause:** All backlight code was commented out during Phase 0 refactoring (watcher registrations, RouteRequests, processKext handler, panel data injection).
- **Fix:** Re-enabled:
  - `kextBacklight` and `kextMCCSControl` watcher registrations in `init()`
  - `AppleIntelPanel::setDisplay` RouteRequest + `F%uT%04x` → `F%uTxxxx` format-string patch
  - `AppleMCCSControl::probe` route (returns zero to prevent DDC/CI backlight conflict)
  - 11 panel data entries (F14-F24Txxxx + Default) for AppleBacklight matching
  - See `kern_green.cpp:270-272, 577-592, 650+`
- **Note:** The DTK FB source `hwSetBacklight` confirms PWM path: DPCD AUX write to 0x722 + MMIO write to 0xC8258 (duty cycle). Register 0xC8254 is the frequency register (not duty cycle — comment in kern_gen11.cpp:2520 is slightly misleading but values are correct).

### Errors NOT Fixed (Harmless, Deferred):
- **IOPresentment interface creation error 0x815** — VSync/display link setup, known harmless on TGL.
- **IOFBSetDisplayModeAndDepth failures** — Gracefully handled by WindowServer, no functional impact.

---

## Route & vtable Audit Findings (Complete)

### Route Audit (kern_gen11.cpp)
- **37 routes** across FB + HW kexts. All use PANIC_COND on failure — any symbol mismatch causes instant kernel panic, making silent failures impossible.
- **3 dead/stale routes found, harmless:**
  - `wrapCDClockFrequency` — never looked up by Lilu
  - `wrapInitCDClock` — never looked up by Lilu
  - `wrapForceWakeMultiThreaded` — never looked up by Lilu
  - These become nops. Safe to remove but not causing issues.
- **`ogetPlatformID`** — Declared in `kern_gen11.hpp:1432` but NEVER referenced in any RouteRequest array. Dead code. Safe to strip.

### vtable Hierarchy Audit
All 4 kexts have correct OSObject specialization inheritance:
- **AppleIntelTGLGraphics** (HW) → IOGPUDevice (4 virtual functions)
- **AppleIntelTGLGraphicsFramebuffer** (FB) → IOFramebuffer (30 virtual functions)
- **AppleIntelTGLGraphicsGLDriver** (GL) → IOGLDrv (10 virtual functions)
- **AppleIntelTGLGraphicsMTLDriver** (MTL) → IOGPUDrv (8 virtual functions)

All have correct OSObject meta class initialization, superclass linkage, and vtable copy chains. No issues found.

### DYLD Patch Audit
- **`-allow3d` is purely advisory** — code NEVER checks `request3D` to abort acceleration. Metal path runs regardless. `-allow3d` only sets `NGreenAllow3DRequested` IORegistry property for diagnostic purposes.
- **`RunFullDisplayPipe` NULL guard** — only active at `fullMTLStage >= 3` (boot-arg `ngreenFullMTLStage=3`). Default stage 0 has only assertion bypass. No functional impact at default settings.
- **Backlight patches** — NOW ACTIVE: `orgApplePanelSetDisplay`, ApplePanel LUT injection, `AppleMCCSControl::probe` disable. Stock Sonoma AppleBacklight.kext controls PWM via FB kext's MMIO registers (0xC8250 range on TGL).

---

## Phase 2: USB EFI Deployment & Testing (NEXT)

### Prerequisites on USB EFI
**CRITICAL: The HW kext (AppleIntelTGLGraphics.kext) MUST be injected.** Without it, acceleration won't work and WindowServer will fall back to software rendering (126% CPU as observed on current boot).

**Method A — OpenCore Kext Injection (Recommended for USB EFI):**
```
EFI/OC/Kexts/
  Lilu.kext
  NootedGreen.kext          ← build/Release/NootedGreen.kext
  AppleIntelTGLGraphics.kext  ← from sle_Internal/sle/ (DTK HW kext)
```
- Add all 3 kexts to config.plist Kernel > Add (with correct bundle IDs, plist paths)
- FB kext stays at /Library/Extensions/ (le/, loaded by Lilu/NootedGreen watcher)
- No need to bless snapshots on the USB volume

**Method B — /S/L/E Install (for internal EFI test):**
```bash
sudo mount -uw /
sudo cp -R sle_Internal/sle/AppleIntelTGLGraphics.kext /System/Library/Extensions/
sudo chown -R root:wheel /System/Library/Extensions/AppleIntelTGLGraphics.kext
sudo kmutil install --update-all --volume-root /
sudo bless --mount / --bootefi --create-snapshot --setBoot
```

### Step 1: Boot Configuration
**Boot-args (first boot, no -allow3d):**
```
-v keepsyms=1 debug=0x100 -NGreenDebug ngreen-dmc=tgl -disablegfxfirmware ngreenSched=5 -ngreenforceprops
```
Note: This is current boot-args + `-ngreenforceprops`. The existing args (`itlwm_cc=US`, debug flags) should be preserved.

### Step 2: Verification Checks (dmesg / IORegistry / ps)
```
# Expected:
✓ "AAPL,ig-platform-id" = <00000000 49009a00> (0x9A490000)
✓ ERROR_GEN6=0x0 at registerService time
✓ AppleIntelTGLGraphics (HW) loaded           ← kextstat | grep AppleIntelTGL
✓ AppleIntelTGLGraphicsFramebuffer (FB) loaded ← kextstat | grep Framebuffer
✓ IOGPUDevice service exists                  ← ioreg -rc IOGPUDevice
✓ IOAccelerator service exists                ← ioreg -rc IOAccelerator
✓ WindowServer CPU < 15% at idle              ← ps aux | grep WindowServer

# If clean, proceed to Step 3.
```

### Step 3: Full Acceleration Test
**Boot-args:**
```
-v keepsyms=1 debug=0x100 -NGreenDebug ngreen-dmc=tgl -disablegfxfirmware ngreenSched=5 -ngreenforceprops -allow3d ngreenFullMTLStage=3
```
- **Verify:** Metal acceleration working (`system_profiler SPDisplaysDataType` shows Metal with feature set)
- **Test:** Run a Metal app, check for GPU hangs, KPs, display corruption
- **Monitor:** WindowServer CPU should be < 10% at idle

---

## Phase 3: If CoreDisplay Hash Table Crash Persists (CONTINGENCY)

If platform-ID injection doesn't resolve the CoreDisplay hash table crash:
- **Root cause:** C++ exception in `std::__1::__hash_table::__construct_node` during `Framebuffer::Framebuffer` → `GPU::GPU` → `GPUWranglerRegisterEventBlock`. Caught and logged by CoreDisplay; system continues.
- **Fix:** Implement DYLD patch to intercept the Framebuffer::Framebuffer constructor. Need target machine's CoreDisplay binary to extract the exact signature.
- **Test:** Check WindowServer log for "hash_table" errors after boot.

---

## Phase 4: Stabilization (FUTURE)

- **DTK FB kext adoption** — requires new byte-pattern LookupPatches (~15 patches) for the DTK FB binary (different from le/ FB)
- **Display quality:** Verify smooth cursor, correct color depth, mode switching
- **Video encode/decode:** Test AppleIntelTGLGraphicsVADriver.bundle + VAME
- **Boot-arg cleanup:** Document all relevant boot-args, add `-ngreenHelp`
- **Remove unnecessary DYLD patches:** After validation, strip NULL stubs and guards
- **NootedGreen v1.0.5 release** — after all Phase 1 fixes validated on real TGL hardware
