# NootedGreen Post-Reboot Report - V1.0.2

## 1. Boot Result

**Status**: PARTIAL SUCCESS - DISPLAY WORKS, NO ACCELERATION

The system booted successfully and reached the login screen. The display is functional at 1920x1080 resolution with the main display showing as online. Graphics acceleration is NOT working - the system is using basic framebuffer mode. This is a MAJOR IMPROVEMENT over V1.0.1 where AppleIntel graphics kexts weren't even loaded.

## 2. Kext Load Result

| Kext | Status | Version |
|------|--------|---------|
| Lilu | LOADED | 1.7.3 |
| NootedGreen | LOADED | 1.0.2 |
| AppleIntelTGLGraphicsFramebuffer | **LOADED** | 16.0.0 |
| AppleIntelTGLGraphics | **LOADED** | 16.0.0 |
| IOAcceleratorFamily2 | LOADED | 481.40.2 |
| IOGraphicsFamily | LOADED | 598 |

**MASSIVE IMPROVEMENT**: AppleIntel graphics kexts are NOW LOADING! This was the primary V1.0.1 blocker.

## 3. NootedGreen Result

### Version Analysis
- **Source version**: 1.0.2 (from NootedGreen/Info.plist)
- **Loaded version**: 1.0.2 (from kextstat)
- **Git commit**: 79d4d3e 1.0.2
- **Version matching**: PASS - source and loaded versions match

### Analysis
- Version matching: PASS
- IORegistry presence: PASS
- IGPU present as GFX0
- Device spoofing: WORKING - device-id 0x9a49 (TigerLake GT2)
- Platform ID: 0x0000499a

## 4. Graphics Stack Result

### IGPU Device
- **Status**: Present as IGPU@2/GFX0 in IORegistry
- **Device ID**: 0x0000499a (spoofed to TigerLake GT2)
- **Platform ID**: 0x0000499a
- **Vendor**: Intel (0x8086)
- **GPU Name**: "Intel Iris Xe Graphics"

### AppleIntel Graphics Matching
- **Status**: LOADED - **MASSIVE IMPROVEMENT**
- AppleIntelTGLGraphicsFramebuffer.kext: **LOADED**
- AppleIntelTGLGraphics.kext: **LOADED**

### Framebuffer
- **Status**: Working
- IOFramebuffer: Present
- Resolution: 1920x1080 (correct)
- Display online: Yes

### CoreDisplay/WindowServer
- WindowServer: Started successfully
- loginwindow: Started successfully

### IOAccelerator/Metal
- **Status**: GENERIC ACCELERATOR - NEW BLOCKER
- IOAccelerator: Present but generic ("com.unknown.bundle")
- Metal: Shows "AGXMetalA12" (Apple GPU framework, not Intel)

## 5. Acceleration Result

### Current Stage: Stage 4

**IMPROVEMENT**: Stage 4 remains but the primary blocker (AppleIntel kexts not loading) is now FIXED.
- AppleIntel kexts now LOADING
- Generic IOAccelerator present
- Metal shows AGXMetalA12 (not Intel)
- New blocker: IOAccelerator binding to generic instead of AppleIntel

## 6. Regression Comparison

| Aspect | V1.0.1 | V1.0.2 | Change |
|--------|--------|--------|--------|
| AppleIntelTGLGraphicsFramebuffer | NOT LOADED | **LOADED** | **NEW SUCCESS** |
| AppleIntelTGLGraphics | NOT LOADED | **LOADED** | **NEW SUCCESS** |
| Display | 1920x1080 | 1920x1080 | UNCHANGED |
| Acceleration stage | 4 | 4 | UNCHANGED |

**Overall**: **MOSTLY IMPROVEMENT** - The core blocker is fixed but new accelerator binding issue.

## 7. Most Important Evidence

1. **AppleIntel kexts ARE LOADING** - kextstat shows loading (was NOT in V1.0.1)
2. device-id spoofed to 0x9a49
3. Platform ID 0x0000499a
4. IOAccelerator shows "com.unknown.bundle" (new issue)
5. Metal shows "AGXMetalA12" (not Intel)

## 8. What Worked

- macOS booted successfully
- Lilu loaded (v1.7.3)
- NootedGreen loaded (v1.0.2)
- **AppleIntelTGLGraphicsFramebuffer loaded** - WASN'T IN V1.0.1!
- **AppleIntelTGLGraphics loaded** - WASN'T IN V1.0.1!
- IGPU present in IORegistry (as GFX0)
- Device spoofing working (0x9a49)
- Basic IOFramebuffer present
- Display online at 1920x1080
- WindowServer started
- No kernel panic

## 9. What Failed

- IOAccelerator is generic (com.unknown.bundle) not AppleIntel
- Metal shows AGXMetalA12 (Apple GPU) not Intel Metal
- No full GPU acceleration working

## 10. Most Likely Current Blocker

### Primary Blocker: IOAccelerator Binding to Generic Instead of AppleIntel

**Supporting Evidence**:
- AppleIntel kexts are loaded but IOAccelerator shows "com.unknown.bundle"
- Metal shows AGXMetalA12 instead of Intel Metal
- No AppleIntel accelerator services visible

**Contradicting Evidence**:
- AppleIntel kexts ARE loading
- Framebuffer works
- Display is online

**Confidence**: HIGH (90%)

## 11. Root Cause Ranking

1. **Most Likely**: IOAccelerator binds to generic instead of AppleIntel acceleration service
2. **Second**: Platform configuration missing for AppleIntel accelerator binding
3. **Third**: Missing properties for proper accelerator matching
4. **Less Likely**: Hardware issue - IGPU and framebuffer work
5. **Ruled Out**: AppleIntel kext loading - now working

## 12. Next 10 Implementations

See file: `25_next_10_implementations.md`

Focus areas:
1. Add boot-stage marker logging
2. Add version print at boot
3. Add IGPU match verification
4. Add AppleIntel component tracer
5. Add framebuffer attach tracer
6. Investigate and fix generic IOAccelerator binding
7. Verify Metal plugin loading
8. Add property publication validator
9. Add display pipe publication tracer
10. Verify forcewake/powerwell/ring initialization

## 13. Next Reboot Recommendation

**Recommended flags**:
```
-v -liludbg liludump=60 keepsyms=1 debug=0x100 -NGreenDebug ngreenSched=5 e1000=0 IGLogLevel=8 -ngreenforceprops
```

**Focus for next boot**: 
1. The AppleIntel kext loading blocker is FIXED
2. Next focus: Why IOAccelerator binds to generic instead of AppleIntel
3. Add logging to trace accelerator matching

## 14. Commands Used

All commands documented in `30_raw_command_list.txt`

## 15. Files Generated

All log files in logs/V1.0.2/ including:
- 00_README_BOOT_CONTEXT.txt
- 01_system_identity.txt
- 02_git_and_build_state.txt
- 03_loaded_kexts*.txt
- 04_kernel_log*.txt
- 06_dmesg*.txt
- 07_nvram*.txt
- 08_ioreg*.txt
- 09-14 system_profiler files
- 23_failure_matrix.txt
- 24_success_matrix.txt
- 25_next_10_implementations.md
- 26_implementation_plan.md
- 27_summary_report.md
- 28_regression_comparison.txt
- 29_acceleration_readiness_analysis.txt
- 30_raw_command_list.txt