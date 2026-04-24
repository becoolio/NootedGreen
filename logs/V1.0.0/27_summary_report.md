# NootedGreen Post-Reboot Report - V1.0.0

## 1. Boot Result

**Status**: PARTIAL SUCCESS

The system booted successfully and reached the login screen. The display is functional at 1920x1080 resolution with the main display showing as online. However, graphics acceleration is not working - the system is using basic framebuffer mode only.

## 2. Kext Load Result

| Kext | Status | Version |
|------|--------|---------|
| Lilu | LOADED | 1.7.3 |
| NootedGreen | LOADED | 1.0.0 |

Both Lilu and NootedGreen loaded successfully. The versions match between source (1.0.0) and loaded (1.0.0).

## 3. NootedGreen Result

### Version Analysis
- **Source version**: 1.0.0 (from NootedGreen/Info.plist)
- **Installed version**: Unknown (kext not in /Library/Extensions - loaded from build folder)
- **Loaded version**: 1.0.0 (from kextstat)
- **Git commit**: 4cec5e7 gen11: force TGL getGPUInfo success on spoofed full-MTL path

### Analysis
- Version matching: PASS - source and loaded versions match
- IORegistry presence: PASS - NootedGreen appears in IORegistry as !registered, !matched
- Patch routing: UNKNOWN - no detailed patch logs visible
- Device match: PASS - IGPU shows spoofed device-id 0x9a49
- Concern: NootedGreen shows as "!registered, !matched" in IORegistry

## 4. Graphics Stack Result

### IGPU Device
- **Status**: Present as GFX0 in IORegistry
- **Device ID**: 0x0000499a (spoofed to TigerLake GT2)
- **Platform ID**: 0x0000499a
- **Vendor**: Intel (0x8086)

### AppleIntel Graphics Matching
- **Status**: FAILED - No AppleIntel graphics kexts loaded
- AppleIntelTGLGraphics.kext: NOT LOADED
- AppleIntelTGLGraphicsFramebuffer.kext: NOT LOADED
- This is a critical failure - no AppleIntel acceleration

### Framebuffer
- **Status**: Basic framebuffer working
- IOFramebuffer: Present
- AppleIntelFramebuffer: NOT FOUND ("Can't get value for key 'AppleIntelFramebuffer'")
- Resolution: 1920x1080 (correct)
- Display online: Yes

### Display/EDID/VBT/OpRegion
- **Display detected**: Yes (Main Display: Yes)
- **EDID**: UNKNOWN - not visible in logs
- **VBT**: UNKNOWN - not visible in logs  
- **OpRegion**: UNKNOWN - not visible in logs

### Backlight
- **Status**: UNKNOWN - not visible in logs

### CoreDisplay/WindowServer
- **CoreDisplay**: UNKNOWN - no clear transaction logs
- **WindowServer**: Started successfully
- **loginwindow**: Started successfully

### IOGraphics/IOAccelerator
- **IOAccelerator**: Present but generic ("com.unknown.bundle")
- **Metal**: NOT WORKING - "No Metal renderer available" error

## 5. Acceleration Result

### Current Stage: Stage 4 (Stable display, but no accelerator service)

- **IOAccelerator status**: PARTIAL - generic accelerator present
- **Display pipe status**: UNKNOWN - no clear logs
- **User client status**: Present - IOFramebufferUserClient visible
- **Metal status**: FAIL - error "No Metal renderer available"
- **Ring status**: UNKNOWN - no ring logs visible
- **Firmware status**: UNKNOWN - no GuC/HuC/DMC logs visible

### Primary Blocker
The primary blocker is that **AppleIntel graphics kexts are not loading**. Without AppleIntelTGLGraphics.kext and AppleIntelTGLGraphicsFramebuffer.kext, there is no hardware acceleration. The system is using a basic fallback framebuffer.

## 6. Regression / Previous Version Comparison

**Previous version folder**: NONE - this is the first boot log collection

A full regression comparison is not possible as this is the first collected log for V1.0.0. There is no previous version to compare against.

## 7. Most Important Evidence

1. **No AppleIntel kexts loaded**: `kextstat` shows no AppleIntel* kexts
2. **No Metal renderer**: SecurityAgent error: "No Metal renderer available"
3. **Generic accelerator**: IOAccelerator shows "com.unknown.bundle" instead of AppleIntel bundle
4. **Low VRAM**: Only 7 MB VRAM reported (indicates basic framebuffer mode)
5. **NootedGreen !matched**: Shows as "!registered, !matched" in IORegistry
6. **Device spoofing works**: device-id shows 0x0000499a (TigerLake)

## 8. What Worked

- macOS booted successfully
- Lilu loaded (v1.7.3)
- NootedGreen loaded (v1.0.0)
- Version matching correct
- IGPU present in IORegistry (as GFX0)
- Device spoofing working (0x9a49)
- Platform ID set correctly (0x0000499a)
- Basic IOFramebuffer present
- Display online at 1920x1080
- WindowServer started
- No kernel panic
- No crash reports

## 9. What Failed

- **AppleIntel graphics kexts NOT loaded** - critical failure
- **No Metal acceleration** - "No Metal renderer available"
- **Generic IOAccelerator** - shows com.unknown.bundle instead of AppleIntel
- **IOAcceleratorFamily2** - not attached properly
- **Display pipe** - unknown status, likely not created properly
- **GuC/HuC/DMC firmware** - no logs visible
- **Forcewake/power wells** - no logs visible
- **Ring initialization** - no logs visible
- **EDID/VBT/OpRegion** - not visible in logs
- **NootedGreen !matched** - shows as not matched to IGPU

## 10. Most Likely Current Blocker

### Primary Blocker: AppleIntel Graphics Kexts Not Loading

**Supporting Evidence**:
- kextstat shows no AppleIntel* kexts
- No AppleIntelFramebuffer in IORegistry
- Generic "com.unknown.bundle" accelerator instead of AppleIntel
- Only 7 MB VRAM (basic framebuffer mode)
- "No Metal renderer available" error

**Contradicting Evidence**:
- IGPU is detected and spoofed correctly
- Basic framebuffer works (display is on)
- NootedGreen is loaded

**Confidence**: HIGH (95%)

**What would prove it**: Loading AppleIntelTGLGraphics.kext and seeing it match to IGPU

**What would disprove it**: Finding AppleIntel kexts ARE loaded in a future boot

## 11. Root Cause Ranking

1. **Most Likely**: NootedGreen patch routing failure - AppleIntel kexts aren't being patched/matched properly. The kext loads but doesn't attach to graphics path.

2. **Second Most Likely**: Missing or incorrect device-id/platform-id matching - AppleIntel kexts reject the spoofed device-id and refuse to load.

3. **Third Most Likely**: NootedGreen initialization timing - NootedGreen may be loading too late or before the AppleIntel kexts need patching.

4. **Less Likely**: OpenCore kext injection order - Lilu/NootedGreen may need to load at different priority.

5. **Ruled Out**: Hardware issue - IGPU is detected and basic display works.

## 12. Next 10 Implementations

See file: `25_next_10_implementations.md`

The priority order should be:
1. Add boot-stage marker logging (Task 1)
2. Add version/commit print (Task 2)
3. Add IGPU match verification (Task 3)
4. Add AppleIntel graphics component tracer (Task 4)
5. Add framebuffer attach tracer (Task 5)
6. Add IOAccelerator attach tracer (Task 10)
7. Add display pipe tracer (Task 7)
8. Add test flags for display pipe (Task 8, 9)
9. Add ring/firmware verification (Task from Phase 8)
10. Continue acceleration enablement (Phase 10-12)

## 13. Next Reboot Recommendation

**Recommended flags**: Continue with current flags but consider:

- Keep: `-v -liludbg -NGreenDebug keepsyms=1 debug=0x100`
- The current flags are good for debugging
- The `-liludump=60` may produce verbose output

**Do NOT change**:
- Don't add -igfxvesa (would disable all graphics)
- Don't add aggressive test flags yet

**Focus for next boot**: 
1. Add the logging from Tasks 1-4 (phases 1-3)
2. Reboot and check if AppleIntel kexts load
3. If still failing, investigate device-id/platform-id matching

## 14. Commands Used

See file: `30_raw_command_list.txt`

## 15. Files Generated

```
logs/V1.0.0/
├── 00_README_BOOT_CONTEXT.txt
├── 01_system_identity.txt
├── 02_git_and_build_state.txt
├── 03_loaded_kexts.txt
├── 03_loaded_kexts_filtered.txt
├── 04_kernel_log_full.txt
├── 04_kernel_log_last_10m.txt
├── 04_kernel_log_last_30m.txt
├── 05_kernel_log_filtered_graphics.txt
├── 05_kernel_log_predicate_graphics.txt
├── 06_dmesg.txt
├── 06_dmesg_filtered.txt
├── 07_nvram_full.txt
├── 07_nvram_boot_args.txt
├── 08_ioreg_full.txt
├── 08_ioreg_lite.txt
├── 09_ioreg_graphics_tree.txt
├── 10_ioreg_display_tree.txt
├── 11_ioreg_accelerator_tree.txt
├── 11_ioreg_accelerator_family2.txt
├── 12_ioreg_framebuffer_tree.txt
├── 12_ioreg_appleintel_framebuffer.txt
├── 13_ioreg_pci_gpu_tree.txt
├── 13_ioreg_igpu_node.txt
├── 13_ioreg_gfx0_node.txt
├── 14_system_profiler_graphics.txt
├── 14_system_profiler_hardware.txt
├── 14_system_profiler_extensions_filtered.txt
├── 15_display_and_panel_state.txt
├── 16_windowserver_logs.txt
├── 16_windowserver_coredisplay_combined.txt
├── 17_coredisplay_logs.txt
├── 18_lilu_nootedgreen_logs.txt
├── 19_appleintel_graphics_logs.txt
├── 20_diagnosticreports_listing.txt
├── 21_guc_forcewake_powerwell_ring_logs.txt (empty - no data found)
├── 22_boot_timeline.txt
├── 23_failure_matrix.txt
├── 24_success_matrix.txt
├── 25_next_10_implementations.md
├── 26_implementation_plan.md
├── 27_summary_report.md
├── 28_regression_comparison.txt (N/A - first version)
└── 30_raw_command_list.txt
```

---

## Summary

This is a **Stage 4** boot - display works but no acceleration. The primary issue is that **AppleIntel graphics kexts are not loading**, which prevents Metal and full acceleration. The NootedGreen kext loads and performs device spoofing (IGPU shows as TigerLake 0x9a49), but the AppleIntel framebuffer and accelerator drivers are not being matched to the GPU.

**Recommended next action**: Implement Tasks 1-4 (logging phases) and reboot to get visibility into why AppleIntel kexts aren't loading.