# NootedGreen Post-Reboot Report - V1.0.2_2026-04-24_0339

## 1. Boot Result

**Status**: PARTIAL SUCCESS - DISPLAY WORKS, ACCELERATION PARTIAL

The system booted successfully and reached the login screen. The display is functional at 1920x1080 resolution with the main display showing as online. Graphics acceleration is working at a basic level - Metal shows as "Supported" but appears to be using generic/Apple Metal (AGXMetalA12) rather than Intel-specific Metal.

## 2. Kext Load Result

| Kext | Status | Version |
|------|--------|---------|
| Lilu | LOADED | 1.7.3 |
| NootedGreen | LOADED | 1.0.2 |
| AppleIntelTGLGraphicsFramebuffer | LOADED | 16.0.0 |
| IOAcceleratorFamily2 | LOADED | 481.40.2 |
| IOGraphicsFamily | LOADED | 598 |

## 3. NootedGreen Result

### Version Analysis
- **Source version**: 1.0.2 (from NootedGreen/Info.plist)
- **Loaded version**: 1.0.2 (from kextstat)
- **Git commit**: 2ddfffa 1.0.2 logs
- **Version matching**: PASS - source and loaded versions match

### Analysis
- Version matching: PASS
- IORegistry presence: PASS
- IGPU present in IORegistry (IGPU@2)
- Device spoofing: WORKING - device-id 0x9a49 (TigerLake GT2)
- Platform ID: Configured

### Issues
- Uncommitted local changes to source (kern_gen11.cpp, kern_green.cpp)
- Lilu loaded version (1.7.3) differs from source (1.7.2) - pre-installed

## 4. Graphics Stack Result

### IGPU Device
- **Status**: Present as IGPU@2 in IORegistry
- **Device ID**: 0x00009a49 (spoofed to TigerLake GT2)
- **Vendor**: Intel (0x8086)
- **GPU Name**: "Intel Iris Xe Graphics"

### AppleIntel Graphics Matching
- **Status**: LOADED
- AppleIntelTGLGraphicsFramebuffer.kext: LOADED (16.0.0)

### Framebuffer
- **Status**: Working
- Resolution: 1920x1080 (correct)
- Display online: Yes
- Framebuffer depth: 24-Bit Color (ARGB8888)

### CoreDisplay/WindowServer
- WindowServer: Started successfully
- GUI reached: Login screen available

### IOAccelerator/Metal
- **Status**: Basic acceleration working
- Metal: Supported (but appears to be generic Apple Metal)
- VRAM: 1536 MB (Dynamic, Max)
- Type: GPU (not "Basic Display")

## 5. Acceleration Result

### Current Stage: Stage 4-5

**Summary**:
- Display works at 1920x1080
- Basic IOAccelerator present
- Metal shows "Supported"
- Intel-specific accelerator may not be binding properly (generic binding)

### Blocker Preventing Next Stage:
The IOAccelerator appears to be binding generically rather than to the Intel-specific IntelAccelerator personality. This prevents full Intel GPU acceleration from working.

## 6. Regression / Previous Version Comparison

**Previous version folder**: logs/V1.0.2/
**Current version folder**: logs/V1.0.2_2026-04-24_0339/
**Overall result**: MOSTLY UNCHANGED

### Key Findings:
- Boot behavior identical to V1.0.2
- All major components in same state as previous report
- No improvements or regressions detected
- This appears to be a clean reboot of V1.0.2

## 7. Most Important Evidence

1. **Device ID**: 0x9a49 - TigerLake GT2 spoofing working
2. **Display**: 1920x1080, online, internal panel
3. **Metal**: Supported (but generic)
4. **NootedGreen**: 1.0.2 loaded correctly
5. **AppleIntel framebuffer**: Loaded and functional

## 8. What Worked

- macOS booted successfully (14.8.3, 23J220)
- Lilu loaded (v1.7.3)
- NootedGreen loaded (v1.0.2)
- AppleIntelTGLGraphicsFramebuffer loaded
- IGPU present in IORegistry
- Device spoofing working (0x9a49)
- Basic IOFramebuffer present
- Display online at 1920x1080
- WindowServer started
- No kernel panic
- No black screen
- Metal framework available

## 9. What Failed

- IOAccelerator binds to generic instead of Intel-specific
- Metal shows as "Supported" but appears generic (AGXMetalA12)
- No Intel-specific accelerator visible in IORegistry
- Full GPU acceleration not working

## 10. Most Likely Current Blocker

### Primary Blocker: IOAccelerator Binding to Generic Instead of AppleIntel

**Supporting Evidence**:
- system_profiler shows "Metal: Supported" but no Intel-specific Metal
- Basic IOAccelerator present but not Intel-specific
- No IntelAccelerator personality visible in IORegistry

**Contradicting Evidence**:
- AppleIntel kexts ARE loading
- Framebuffer works
- Display is online
- Metal framework is present

**Confidence**: HIGH (85%)

## 11. Root Cause Ranking

1. **Most Likely**: IOAccelerator binds to generic instead of Intel-specific accelerator service
2. **Second**: Info.plist accelerator personality not matching properly
3. **Third**: Missing properties for Intel accelerator matching
4. **Less Likely**: Hardware issue - IGPU and framebuffer work correctly
5. **Ruled Out**: AppleIntel kext loading - confirmed working

## 12. Next 10 Implementations

See file: `25_next_10_implementations.md`

Priority implementations:
1. Add boot-stage marker logging to NootedGreen startup
2. Add version and Git commit print at boot
3. Add IGPU match verification and full property dump
4. Add AppleIntel graphics component load/match tracer
5. Add framebuffer attach tracer
6. Add test flag to suppress display pipe publication
7. Add test flag to publish only one display pipe
8. Add IOAccelerator attach tracer
9. Add forcewake and powerwell verification before ring use
10. Add device-id/platform-id sanity check

## 13. Next Reboot Recommendation

**Recommended flags**:
```
-v -liludbg liludump=60 keepsyms=1 debug=0x100 -NGreenDebug ngreenSched=5 e1000=0 IGLogLevel=8 -ngreenforceprops
```

**Focus for next boot**:
1. Add detailed boot-stage logging to trace accelerator binding
2. Verify IntelAccelerator personality matches properly
3. Test with -ngreenNoDisplayPipe flag to isolate display pipe issues
4. Investigate why IOAccelerator binds generically

## 14. Commands Used

All commands documented in `30_raw_command_list.txt`

## 15. Files Generated

All log files in logs/V1.0.2_2026-04-24_0339/:
- 01_system_identity.txt
- 02_git_and_build_state.txt
- 03_loaded_kexts.txt
- 03_loaded_kexts_kextstat_legacy.txt
- 03_loaded_kexts_filtered.txt
- 04_kernel_log_full.txt
- 04_kernel_log_last_30m.txt
- 04_kernel_log_last_10m.txt
- 05_kernel_log_filtered_graphics.txt
- 05_kernel_log_predicate_graphics.txt
- 06_dmesg.txt (partial - requires sudo)
- 07_nvram_full.txt
- 07_nvram_boot_args.txt
- 08_ioreg_full.txt
- 08_ioreg_lite.txt
- 09_ioreg_graphics_tree.txt
- 10_ioreg_display_tree.txt
- 11_ioreg_accelerator_tree.txt
- 11_ioreg_accelerator_family2.txt
- 12_ioreg_framebuffer_tree.txt
- 12_ioreg_appleintel_framebuffer.txt
- 13_ioreg_pci_gpu_tree.txt
- 13_ioreg_igpu_node.txt
- 13_ioreg_gfx0_node.txt
- 14_system_profiler_graphics.txt
- 14_system_profiler_displays.txt
- 14_system_profiler_hardware.txt
- 16_windowserver_logs.txt
- 16_windowserver_coredisplay_combined.txt
- 23_failure_matrix.txt
- 24_success_matrix.txt
- 25_next_10_implementations.md
- 26_implementation_plan.md
- 27_summary_report.md
- 28_regression_comparison.txt
- 29_acceleration_readiness_analysis.txt
- 30_raw_command_list.txt