# NootedGreen Post-Reboot Report - V1.0.1

## 1. Boot Result

**Status**: PARTIAL SUCCESS

The system booted successfully and reached the login screen. The display is functional at 1920x1080 resolution with the main display showing as online. However, graphics acceleration is not working - the system is using basic framebuffer mode only.

## 2. Kext Load Result

| Kext | Status | Version |
|------|--------|---------|
| Lilu | LOADED | 1.7.3 |
| NootedGreen | LOADED | 1.0.1 |

Both Lilu and NootedGreen loaded successfully. The versions match between source (1.0.1) and loaded (1.0.1).

## 3. NootedGreen Result

### Version Analysis
- **Source version**: 1.0.1 (from NootedGreen/Info.plist)
- **Installed version**: Unknown (kext loaded from build)
- **Loaded version**: 1.0.1 (from kextstat)
- **Git commit**: 9d7d41b 1.0.1

### Analysis
- Version matching: PASS - source and loaded versions match (updated from 1.0.0)
- IORegistry presence: PASS - NootedGreen appears in IORegistry
- Patch routing: UNKNOWN - no detailed patch logs visible
- Device match: PASS - IGPU shows spoofed device-id 0x9a49
- GPU name improved: NOW shows "Intel Iris Xe Graphics" (was "Unknown Unknown" in V1.0.0)

## 4. Graphics Stack Result

### IGPU Device
- **Status**: Present as GFX0 in IORegistry
- **Device ID**: 0x0000499a (spoofed to TigerLake GT2)
- **Platform ID**: 0x0000499a
- **Vendor**: Intel (0x8086)
- **GPU Name**: "Intel Iris Xe Graphics" (IMPROVED from V1.0.0)

### AppleIntel Graphics Matching
- **Status**: FAILED - No AppleIntel graphics kexts loaded
- AppleIntelTGLGraphics.kext: NOT LOADED
- AppleIntelTGLGraphicsFramebuffer.kext: NOT LOADED

### Framebuffer
- **Status**: Basic framebuffer working
- IOFramebuffer: Present
- Resolution: 1920x1080 (correct)
- Display online: Yes

### CoreDisplay/WindowServer
- **WindowServer**: Started successfully
- **loginwindow**: Started successfully

### IOAccelerator/Metal
- **IOAccelerator**: Present but generic ("com.unknown.bundle")
- **Metal**: NOT WORKING - "No Metal renderer available" error

## 5. Acceleration Result

### Current Stage: Stage 4 (Same as V1.0.0)

- No AppleIntel graphics kexts loaded (same as V1.0.0)
- Generic IOAccelerator present (same as V1.0.0)
- Metal not working (same as V1.0.0)
- **Improvement**: GPU name now shows properly

## 6. Regression Comparison

| Aspect | V1.0.0 | V1.0.1 | Change |
|--------|--------|--------|--------|
| Boot result | Partial | Partial | UNCHANGED |
| NootedGreen version | 1.0.0 | 1.0.1 | IMPROVED |
| GPU name | Unknown Unknown | Intel Iris Xe Graphics | IMPROVED |
| AppleIntel kexts | Not loaded | Not loaded | UNCHANGED |
| Metal | Not working | Not working | UNCHANGED |
| Acceleration stage | 4 | 4 | UNCHANGED |

**Overall**: MINOR IMPROVEMENT - GPU identification works now, but core acceleration issue persists.

## 7. Most Important Evidence

1. **No AppleIntel kexts loaded**: kextstat shows no AppleIntel* kexts
2. **No Metal renderer**: SecurityAgent error: "No Metal renderer available"
3. **GPU name improved**: Now shows "Intel Iris Xe Graphics" (was "Unknown Unknown")
4. **Device spoofing works**: device-id shows 0x0000499a
5. **-ngreenforceprops added**: New flag for property injection

## 8. What Worked

- macOS booted successfully
- Lilu loaded (v1.7.3)
- NootedGreen loaded (v1.0.1) - version updated
- IGPU present in IORegistry (as GFX0)
- Device spoofing working (0x9a49)
- Platform ID set correctly (0x0000499a)
- Basic IOFramebuffer present
- Display online at 1920x1080
- WindowServer started
- **NEW**: GPU name properly shows as "Intel Iris Xe Graphics"
- No kernel panic

## 9. What Failed

- AppleIntel graphics kexts NOT loaded - critical failure (UNCHANGED from V1.0.0)
- No Metal acceleration (UNCHANGED from V1.0.0)
- Generic IOAccelerator (UNCHANGED from V1.0.0)
- No GuC/HuC/DMC logs visible (UNCHANGED from V1.0.0)
- No ring initialization logs (UNCHANGED from V1.0.0)

## 10. Most Likely Current Blocker

### Primary Blocker: AppleIntel Graphics Kexts Not Loading

**Supporting Evidence**:
- kextstat shows no AppleIntel* kexts
- Generic "com.unknown.bundle" accelerator instead of AppleIntel
- Only 7 MB VRAM (basic framebuffer mode)
- "No Metal renderer available" error

**Contradicting Evidence**:
- IGPU detected and spoofed correctly
- GPU name now shows properly (improvement)
- Basic framebuffer works

**Confidence**: HIGH (95%)

## 11. Root Cause Ranking

1. **Most Likely**: NootedGreen patch/routing failure - AppleIntel kexts aren't being patched/matched properly
2. **Second Most Likely**: Device-id/platform-id incompatibility - AppleIntel rejects the spoofed values
3. **Third Most Likely**: NootedGreen initialization timing - loading too late for AppleIntel matching
4. **Less Likely**: OpenCore kext injection order
5. **Ruled Out**: Hardware issue - IGPU detected, display works

## 12. Next 10 Implementations

See file: `25_next_10_implementations.md`

Priority order:
1. Add boot-stage marker logging (Task 1)
2. Add version/commit print (Task 2)
3. Add IGPU match verification (Task 3)
4. Add AppleIntel graphics component tracer (Task 4)
5. Investigate why AppleIntel kexts not loading (Task 6)
6. Add test mode to skip spoofing (Task 7)
7. Add different platform-ID options (Task 8)
8. Add framebuffer attach tracer (Task 5)
9. Add property publication validator (Task 9)
10. Add IOAccelerator attach tracer (Task 10)

## 13. Next Reboot Recommendation

**Recommended flags**: Continue with current flags:

```
-v -liludbg liludump=60 keepsyms=1 debug=0x100 -NGreenDebug ngreenSched=5 -ngreenforceprops
```

**Focus for next boot**: 
1. The -ngreenforceprops flag was added but didn't resolve the AppleIntel loading issue
2. Need diagnostic logging (Tasks 1-4) to understand what's happening
3. Consider trying -ngreennospoof to test without device spoofing

## 14. Commands Used

See file: `30_raw_command_list.txt`

## 15. Files Generated

See output of `ls logs/V1.0.1/`