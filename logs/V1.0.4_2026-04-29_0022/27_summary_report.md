# NootedGreen Post-Reboot Report - V1.0.4

## 1. Boot Result

**SUCCESS** - System completed boot to login screen and desktop. No kernel panic, no black screen, no reboot loop.

## 2. Kext Load Result

- **Lilu**: LOADED (version 1.7.3)
- **NootedGreen**: LOADED (version 1.0.4)
- **AppleIntelTGLGraphicsFramebuffer**: LOADED
- **AppleIntelTGLGraphics**: LOADED
- **IOAcceleratorFamily2**: LOADED
- **IOGraphicsFamily**: LOADED

## 3. NootedGreen Result

- Source version: 1.0.4
- Loaded version: 1.0.4 (matches)
- git commit: c05cd00
- Patch routing: Active (CoreDisplay assertion bypass patch applied per logs)
- Debug flags detected: -NGreenDebug, ngreenSched=5, -ngreenforceprops, -ngreenTraceBCS, -ngreenDisplayPipeTrace

## 4. Graphics Stack Result

- **IGPU device**: Present (device-id: 0x9a49)
- **Framebuffer**: Attached and online
- **Display**: 1920x1080 internal LCD online
- **Metal**: Supported (system_profiler confirms)
- **NootedGreen patches**: Applied
- **EDID**: Present
- **CoreDisplay**: Initialized

## 5. Acceleration Result

**CRITICAL FINDING**: The boot has `-allow3d is NOT set in boot-arg` message in kernel logs!

This is the blocker. The AppleIntelTGLGraphics acceleration probe is being rejected because `-allow3d` boot argument is missing.

Current acceleration stage: **Stage 3** - Framebuffer attached but acceleration blocked by missing boot arg.

## 6. Regression / Previous Version Comparison

Previous version folder: V1.0.4_2026-04-28_2215
Current version folder: V1.0.4_2026-04-29_0022

**Comparison result**: UNCHANGED - Same NootedGreen version, same functionality, same blocker

## 7. Most Important Evidence

```
(AppleIntelTGLGraphics) [IGPU] Failing probe (-allow3d is NOT set in boot-arg)!
```

This message appears 3 times in kernel log - acceleration probe fails without `-allow3d` flag.

## 8. What Worked

- System booted successfully
- Lilu loaded
- NootedGreen loaded (1.0.4)
- IGPU detected (device-id 0x9a49)
- Framebuffer attached
- Display output working (1920x1080)
- Metal shows as supported in system_profiler
- No kernel panic
- No black screen
- WindowServer started

## 9. What Failed

- AppleIntelTGLGraphics acceleration probe (3 instances) - REJECTED due to missing `-allow3d` boot arg
- Full GPU acceleration not available (limited to VESA/basic mode)

## 10. Most Likely Current Blocker

**Primary blocker**: Missing `-allow3d` boot argument

**Supporting evidence**:
- Kernel log shows: "Failing probe (-allow3d is NOT set in boot-arg)" appearing 3 times
- AppleIntelTGLGraphics kext fails probe despite loading
- Acceleration blocked at probe stage

**Contradicting evidence**:
- Metal shows as supported in system_profiler (this may be misleading - basic Metal support)
- Framebuffer works (but this is separate from acceleration)

**Confidence**: HIGH - The missing `-allow3d` flag is explicitly blocking 3D acceleration

**What would prove it**: Adding `-allow3d` to boot args and rebooting

**What would disprove it**: If acceleration still fails with `-allow3d`, then another issue exists

## 11. Root Cause Ranking

1. **Missing -allow3d boot argument** (CONFIRMED via kernel log)
2. **Stale prelinked kernel** (unlikely - kexts loading)
3. **Wrong platform-id** (unlikely - framebuffer works)
4. **Missing firmware** (unlikely - basic functions work)
5. **Hardware incompatibility** (unlikely - Tiger Lake is supported)

## 12. Next 10 Implementations

1. Add `-allow3d` to boot-args in NVRAM
2. Rebuild NootedGreen with explicit -allow3d boot flag handling if not already present
3. Add boot-time logging to detect -allow3d presence
4. Add property dump for AAPL,ig-platform-id verification
5. Add device-id verification logging (expect 0x9a49 for TGL)
6. Add framebuffer attach state logging
7. Add display pipe creation tracing
8. Add IOAccelerator attach verification
9. Add forcewake/powerwell state logging
10. Add GuC/HuC/DMC status logging

## 13. Next Reboot Recommendation

**Add `-allow3d` to boot-args.**

Current boot args: `-v -liludbg liludump=60 keepsyms=1 debug=0x100 e1000=0 IGLogLevel=0xe -NGreenDebug ngreenSched=5 -ngreenforceprops -ngreenTraceBCS -ngreenDisplayPipeTrace`

Recommended: Add `-allow3d` to enable acceleration probe

## 14. Commands Used

- sw_vers, uname -a, hostname, uptime, whoami, date
- sysctl kern.boottime, sysctl hw.model
- csrutil status, csrutil authenticated-root status
- nvram -p
- kmutil showloaded
- kextstat
- log show --last boot
- ioreg -lw0
- system_profiler SPDisplaysDataType
- dmesg (sudo)

## 15. Files Generated

- 01_system_identity.txt
- 02_git_and_build_state.txt
- 03_loaded_kexts.txt
- 03_loaded_kexts_filtered.txt
- 04_kernel_log_full.txt.gz
- 05_kernel_log_filtered_graphics.txt
- 06_dmesg.txt
- 07_nvram_full.txt
- 07_nvram_boot_args.txt
- 08_ioreg_full.txt
- 09_ioreg_graphics_tree.txt
- 10_ioreg_display_tree.txt
- 12_ioreg_framebuffer_tree.txt
- 13_ioreg_pci_gpu_tree.txt
- 13_ioreg_igpu_node.txt
- 14_system_profiler_graphics.txt
- 16_windowserver_coredisplay_combined.txt
- 18_lilu_nootedgreen_logs.txt
- 19_appleintel_graphics_logs.txt
- 20_panic_and_crash_reports/
- 21_guc_forcewake_powerwell_ring_logs.txt