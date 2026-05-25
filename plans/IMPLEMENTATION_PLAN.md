# NootedGreen Implementation Plan — Real TGL (0x9A498086) on macOS Sonoma

## Current Status: Stage 8/10 — Already Boots to Desktop with Metal

**Verified from V1.0.4_2026-05-12_2254 and V1.0.4_2026-05-15_1135 boot logs:**

| Capability | Status |
|---|---|
| macOS Sonoma 14.8.3 boot to login screen | ✅ Working |
| Internal display 1920x1080 @ 60Hz | ✅ Working |
| Metal: Supported (system_profiler) | ✅ Working |
| DMC firmware v2.12 loaded | ✅ Working |
| Power wells, forcewake, MMIO access | ✅ Working |
| eDP link training (linkRate=24, bpp=10, laneCount=2) | ✅ Working |
| AUX/DPCD communication | ✅ Working |
| WindowServer + IOAccelerationUserClient | ✅ Working |
| GuC scheduler (sched=3) | ✅ Working |
| Host scheduler (sched=5, -disablegfxfirmware) | ✅ Working |
| 15 display planes, 3 display paths | ✅ Working |
| -allow3d (full 3D acceleration) | ❌ Not yet tested |
| Platform-ID injection | ❌ Shows 0x00000000 |

**The Big Sur TGL kexts load and function on Sonoma without IOGPUFamily ABI shims.** No vtable bridging is needed for basic display + Metal. The "IOGPUFamily ABI breakage" fear was unfounded — the real TGL `isRealTGL=true` path in NootedGreen handles the hardware-specific differences (GuC, forcewake, topology) natively, and the kexts themselves are binary-compatible with Sonoma's IOGPUFamily.

**Scope: TGL only, Sonoma only.** Strip all ICL/RPL/ADL/legacy code.

---

## Phase 0: Codebase Simplification

Strip everything not needed for real TGL on Sonoma. The codebase has ~60% dead weight from ICL fallback and RPL/ADL spoofing paths.

### Step 0.1 — Remove ICL kext monitoring
- Remove `callbackICLFB` and `callbackICLHW` watchers from `kern_start.cpp`
- Remove ICL-specific route installations from `kern_gen11.cpp`
- Remove `kern_genx.hpp/cpp` entirely (ICL-only display path)
- Remove ICL personality publishing

### Step 0.2 — Remove RPL/ADL spoof path
- Remove `isRealTGL` flag and all branching — everything is real TGL now
- Remove topology hardcodes (L3BankCount, MaxEUPerSubSlice, NumSubSlices)
- Remove BCS bypass, DPCD clamping, IRQ watchdog, EMR enforcer, GPU health monitor
- Remove BCS stop+clear, TLB invalidation, ERROR_GEN6 R/W clear
- Remove MultiForceWakeSelect=1 injection, Master IRQ pre-enable
- Simplify `IntelAccelerator::start()` — keep only the real TGL execution path

### Step 0.3 — Remove unnecessary DYLD patches
- `GetMTLTexture` NULL stub — real TGL creates textures
- `GetMTLCommandQueue` NULL stub — real TGL creates command queues  
- `RunFullDisplayPipe` NULL vcall guard — DisplayPipe is valid on real TGL
- `AccessComplete` skip — works natively
- `Display::Present` skip — works natively
- ICL Metal device-ID bypass — TGL MTL driver is primary
- CoreLSKD CPUID patch — legacy Haswell spoof
- SkyLight bypass — legacy opt-in

### Step 0.4 — Remove Ventura build artifacts
- All `#if` kernel version checks for Ventura
- Ventura-only binary patch variants
- Ventura-only DYLD patch variants

### Step 0.5 — Clean up support files
- Remove `FirmwareADLP.cpp` (ADL-P firmware, not TGL)
- Remove ICL-related boot-args (`-ngreenRefProbeF2`, etc.)
- Flatten `sle_Internal/` — keep only the deployment le/ variant

**Deliverable:** Clean TGL-only codebase. Verify existing boot still works after stripping.

---

## Phase 1: Fix Non-Fatal Errors

The system boots and displays, but has known non-fatal errors from the May 12 boot log.

### Error 1: `ERROR_GEN6=0x37` during DisplayPipe init
- **Observed:** MMIO read returns 0x37 at ERROR_GEN6 register during DisplayPipe creation
- **Likely cause:** Stale GPU error state from early init. Gen6 registers hold pre-existing errors that Apple's driver doesn't expect on clean hardware
- **Fix:** Add MMIO write to clear ERROR_GEN6 (write 0xFFFFFFFF to clear) before DisplayPipe init, or mask in the existing EMR enforcer logic
- **Test:** Boot with verbose logging, verify ERROR_GEN6 reads as 0 after clear

### Error 2: CoreDisplay DisplayPipe hash table insert error
- **Observed:** WindowServer log shows C++ `std::__1::unique_ptr...hash_table...` exception during DisplayPipe creation
- **Likely cause:** Duplicate pipe entry or stale state in CoreDisplay's display pipe tracking
- **Fix:** Add DYLD patch or framebuffer property to suppress/skip duplicate pipe registration. Alternatively, ensure only one DisplayPipe is published initially (`-ngreenSinglePipe`)
- **Test:** Boot, check WindowServer logs for hash table errors

### Error 3: Platform-ID showing 0x00000000
- **Observed:** dmesg shows "platform-id=0x00000000"
- **Likely cause:** AAPL,ig-platform-id not injected by bootloader or NootedGreen
- **Fix:** Add platform-ID injection in NootedGreen's IGPU property seeding. Use known TGL platform IDs (e.g., 0x8A987600 for TGL-U with 2 ports, or 0x8A987601 for TGL-H)
- **Test:** Boot, verify platform-id shows expected value

### Error 4: `IOPresentment` interface creation error 0x815
- **Observed:** WindowServer log shows error 0x815 during IOPresentment setup
- **Likely cause:** VSync/display link setup issue. Presentment is tied to display timing
- **Fix:** Likely harmless — verify display timing is correct. If persistent, trace via HookCase

### Error 5: `IOFBSetDisplayModeAndDepth` failures
- **Observed:** Gracefully handled failures during mode setting
- **Likely cause:** Mode timing negotiation between framebuffer and display
- **Fix:** Likely harmless (handled gracefully). Monitor for regressions

**Deliverable:** Clean boot log — no ERROR_GEN6, no CoreDisplay exceptions, valid platform-ID.

---

## Phase 2: Debug Infrastructure

### Step 2.1 — Build version logging
- Print NootedGreen version, git commit, build date at plugin start
- Boot arg `-ngreenVerbose` for increased verbosity

### Step 2.2 — Boot-stage markers
- Add `[NGreen Stage X/Y]` markers at each init stage
- Confirm all 5 stages complete in kernel log

### Step 2.3 — IGPU property dump
- Log device-id, vendor-id, platform-id, model at match time
- Confirm spoofed device-id 0x9A49

### Step 2.4 — Firmware decision logging
- Log GuC (SKIP/LOAD/ERROR), HuC (SKIP/LOAD/ERROR), DMC (LOADED/ERROR)
- Currently: DMC loaded, GuC depends on boot args

### Step 2.5 — Kext component tracer
- Log when AppleIntelTGLGraphicsFramebuffer and AppleIntelTGLGraphics are processed
- Confirm all expected routes applied

**Deliverable:** Every boot produces a clear log trail from kext load through desktop.

---

## Phase 3: Test `-allow3d` (Full 3D Acceleration)

This is the single biggest untested item. The May 12 report explicitly recommended testing it.

### Step 3.1 — Test with host scheduler
```bash
Boot args: -allow3d -disablegfxfirmware ngreenSched=5 -v -liludbgall
```
- Expected: GPU 3D acceleration functional
- Monitor for: GPU hangs, KPs, display corruption, CoreDisplay crashes

### Step 3.2 — Test with GuC scheduler
```bash
Boot args: -allow3d ngreenSched=3 -v -liludbgall
```
- Expected: GuC handles scheduling, 3D acceleration works
- Monitor for: GuC initialization errors, H2G/G2H failures, CSB mismatch

### Step 3.3 — Test OpenGL fallback
- Verify AppleIntelTGLGraphicsGLDriver.bundle loads
- Test OpenGL apps (Quartz GL, OpenGL Profiler)

### Step 3.4 — Test hardware video encode/decode
- Verify AppleIntelTGLGraphicsVADriver.bundle + VAME load
- Test with VideoToolbox-based app (QuickTime Player, Final Cut)

**Deliverable:** 3D acceleration, OpenGL, and video encode/decode functional. No GPU hangs.

---

## Phase 4: Stabilization

### Step 4.1 — Crash-driven patching
- Collect crash logs from Phase 3 testing
- Add binary patches or DYLD patches as needed
- Iterate: test → crash → patch → retest

### Step 4.2 — Display quality
- Verify smooth cursor (no stutter)
- Verify correct color depth (30-bit if supported)
- Verify display mode switching (resolution/refresh rate)

### Step 4.3 — Backlight
- Re-enable AppleBacklight kext watcher routes (currently commented out)
- Inject 7 backlight LUT tables (already in codebase)
- Test brightness keys

### Step 4.4 — Boot-arg cleanup
- Document all relevant boot-args
- Remove stale debug boot-args that no longer apply
- Add `-ngreenHelp` to print available boot-args

### Step 4.5 — Remove unnecessary DYLD patches
- After validating that real TGL paths work, remove NULL stubs and guards
- Only keep: cs_validate_page hook, bundle path redirect, IGC path rewrite, CoreDisplay assertion bypass, VideoToolbox/AppleGVA spoofs

**Deliverable:** Stable daily-driver. Clean boot, no crashes, backlight works.

---

## Current Resource Situation (from log analysis)

**The TGL kexts (v16.0.32, Big Sur) work on Sonoma 14.8.3.** They load via modified `com.xxxxx.driver.*` bundle IDs with `IOPCIPrimaryMatch` set appropriately. The real TGL `isRealTGL=true` path is active and handles:
- GuC firmware loading (when not disabled)
- DMC firmware loading (ngreen-dmc=tgl)
- Native forcewake
- Native eDP link training (linkRate=24, laneCount=2)
- Native topology detection

**What we know works** (from ioreg and system_profiler evidence):
- AppleIntelTGLGraphicsFramebuffer v16.0.0 loaded
- AppleIntelFramebufferController active  
- AppleIntelPowerWell active
- AppleIntelPortHAL active
- 15 AppleIntelPlane instances
- 3 AppleIntelDisplayPath instances
- IOAcceleratorFamily2 loaded, IOAccelerationUserClient created by WindowServer

**The ICL/RPL/ADL code paths are dead code** — this is real TGL hardware, none of those paths are ever taken. Stripping them reduces code size by ~60% and eliminates potential confusion.

---

## Summary of What to Build

| Item | Type | Effort |
|---|---|---|
| Codebase cleanup (strip ICL/RPL/ADL) | Code removal | 1 day |
| ERROR_GEN6=0x37 clear | MMIO patch | 2 hours |
| CoreDisplay hash table error | DYLD patch | 2-4 hours |
| Platform-ID injection | Property injection | 2 hours |
| -allow3d testing | Test + iterate | 1-2 days |
| Build version + stage logging | Logging | 2 hours |
| Backlight re-enable | Kext route | 4 hours |
| Boot-arg documentation | Docs | 1 hour |
| **Total remaining work** | | **~3-4 days** |
