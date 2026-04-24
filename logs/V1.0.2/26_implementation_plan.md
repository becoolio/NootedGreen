# Implementation Plan - V1.0.2

## Phase 1: Confirm NootedGreen Load and Patch Routing
- **Code changes**: Add boot-stage markers
- **Logs to add**: Early "[NootedGreen] Starting", "[NootedGreen] Version X loaded"
- **Test flags**: Already using -liludbg -NGreenDebug
- **Expected**: Version prints in kernel logs early
- **Failure signal**: NootedGreen not in kmutil showloaded

## Phase 2: Confirm IGPU Matching and IORegistry Properties
- **Code changes**: Add device-id/platform-id dump
- **Logs to add**: "[NootedGreen] IGPU matched: device-id=X, platform-id=Y"
- **Test flags**: Using -ngreenforceprops
- **Expected**: Spoofed device-id visible in IORegistry
- **Failure signal**: Wrong device-id or no spoofing

## Phase 3: Confirm AppleIntel Graphics Component Matching
- **Code changes**: Add matching attempt logs
- **Logs to add**: "[NootedGreen] Attempting AppleIntel match", "[NootedGreen] Match success" or failure reason
- **Test flags**: Continue with -NGreenDebug
- **Expected**: AppleIntelTGLGraphics and AppleIntelFramebuffer in loaded kexts
- **Failure signal**: No match attempts in logs (V1.0.1 issue)

## Phase 4: Confirm Framebuffer Attach and Display State
- **Code changes**: Add framebuffer start logs
- **Logs to add**: "[NootedGreen] Framebuffer attach start", "[NootedGreen] Framebuffer online"
- **Expected**: IOFramebuffer present, display online
- **Failure signal**: No framebuffer or black screen

## Phase 5: Confirm CoreDisplay and WindowServer Behavior
- **Code changes**: Potentially add transaction logs
- **Expected**: WindowServer starts without display errors
- **Failure signal**: WindowServer crashes or corebrightnessd issues

## Phase 6: Confirm IOAccelerator Service Publication
- **Code changes**: Add accelerator attach logs
- **Logs to add**: "[NootedGreen] Accelerator attach start"
- **Expected**: IOAccelerator not "com.unknown.bundle"
- **Failure signal**: Generic accelerator remains

## Phase 7: Confirm Display Pipe Behavior
- **Code changes**: Add pipe publication logs  
- **Expected**: IOAccelDisplayPipeCapabilities published
- **Failure signal**: No pipe or timeout errors

## Phase 8: Confirm Forcewake, Power Wells and Rings
- **Code changes**: Add power initialization logs
- **Expected**: Power states ready, rings available
- **Failure signal**: Power well issues in dmesg

## Phase 9: Confirm GuC/HuC/DMC Handling
- **Code changes**: Add firmware logs
- **Expected**: Firmware decisions logged
- **Failure signal**: Stuck GuC or missing firmware

## Phase 10: Minimal Acceleration Enablement
- **Code changes**: From above phases
- **Expected**: Stage 5+ acceleration
- **Failure signal**: Stuck at Stage 4

## Phase 11: Stabilize Acceleration
- **Code changes**: Based on Phase 10 results
- **Expected**: Stage 7+
- **Failure signal**: Regression or hang