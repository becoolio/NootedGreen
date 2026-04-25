# Next 10 Implementation Tasks - V1.0.4_2026-04-24_0844

## Task 1: Add NootedGreen Boot-Stage Marker Logging
- **File(s) to modify**: `NootedGreen/kern_green.cpp`, `NootedGreen/kern_base.cpp`
- **Function/area**: `NootedGreen::init()`, `NootedGreen::processKext()`
- **Purpose**: Add boot-stage markers to track NootedGreen initialization progress
- **Exact behavior**: Print stage markers like "[NootedGreen] Stage 1: Lilu attach", "[NootedGreen] Stage 2: IGPU detection", etc.
- **What logs to add**: DBGLOG statements at each major initialization step
- **Expected result**: Visible boot-stage markers in kernel log showing initialization order
- **Failure signal**: No markers appear despite NootedGreen loading
- **How to test**: Boot with `-liludbg` and check kernel log for stage markers
- **How to rollback**: Remove the DBGLOG statements, rebuild

## Task 2: Add NootedGreen Version/Git Commit Print at Boot
- **File(s) to modify**: `NootedGreen/kern_green.cpp`
- **Function/area**: `NootedGreen::init()`, constructor
- **Purpose**: Print version and Git commit at boot for debugging
- **Exact behavior**: Print "[NootedGreen] Version: 1.0.4, Commit: 5de6fe1" on boot
- **What logs to add**: Single DBGLOG with version info using __DATE__ and __TIME__ macros
- **Expected result**: Version string visible in kernel log output
- **Failure signal**: Version string not visible
- **How to test**: Run `log show --last boot | grep NootedGreen` and check output
- **How to rollback**: Remove the version print statement

## Task 3: Add IGPU Match Verification and Property Dump
- **File(s) to modify**: `NootedGreen/kern_gen11.cpp`, `NootedGreen/kern_green.cpp`
- **Function/area**: `IntelGen11Graphics::attach()`, device property injection
- **Purpose**: Verify IGPU device-id/platform-id match and dump all injected properties
- **Exact behavior**: Print injected device-id, vendor-id, platform-id, and all framebuffer properties
- **What logs to add**: DBGLOG showing property injection results
- **Expected result**: Visible property dump showing what was injected vs what was detected
- **Failure signal**: Mismatch between expected and actual properties
- **How to test**: Boot and check kernel log for property dump
- **How to rollback**: Remove the dump statements

## Task 4: Add AppleIntel Graphics Component Load/Match Tracer
- **File(s) to modify**: `NootedGreen/kern_base.cpp`
- **Function/area**: `Plugin::processKext()`
- **Purpose**: Track which AppleIntel graphics kexts are processed and matched
- **Exact behavior**: Print "Processing kext: AppleIntelTGLGraphics", "Match succeeded", etc.
- **What logs to add**: DBGLOG at kext processing and match stages
- **Expected result**: Visible log showing each kext as it's processed
- **Failure signal**: Kext not being processed or match failing silently
- **How to test**: Boot and grep kernel log for kext names
- **How to rollback**: Remove tracer DBGLOG statements

## Task 5: Add Framebuffer Attach Tracer
- **File(s) to modify**: `NootedGreen/kern_green.cpp`
- **Function/area**: `Framebuffer callback`, `NootedGreen::intelFramebufferCallback()`
- **Purpose**: Trace framebuffer service attachment
- **Exact behavior**: Print "[NootedGreen] Framebuffer callback: AppleIntelFramebufferController attaching"
- **What logs to add**: DBGLOG when framebuffer services are published
- **Expected result**: Visible log showing framebuffer service publication
- **Failure signal**: No attachment log despite framebuffer services appearing
- **How to test**: Boot and check kernel log for framebuffer attach messages
- **How to rollback**: Remove the tracer statements

## Task 6: Add EDID/VBT/OpRegion Parse Confirmation Logs
- **File(s) to modify**: `NootedGreen/kern_green.cpp`
- **Function/area**: Framebuffer property reading code
- **Purpose**: Confirm EDID, VBT, and OpRegion are being parsed
- **Exact behavior**: Print "EDID parsed: X bytes", "VBT found: Y bytes", "OpRegion: valid"
- **What logs to add**: DBGLOG for each successful parse
- **Expected result**: Visible confirmation of each data structure parse
- **Failure signal**: Parses failing silently
- **How to test**: Boot and check kernel log for parse confirmations
- **How to rollback**: Remove the parse confirmation logs

## Task 7: Add Device-ID/Platform-ID Sanity Check
- **File(s) to modify**: `NootedGreen/kern_gen11.cpp`, `NootedGreen/kern_green.cpp`
- **Function/area**: Device detection code
- **Purpose**: Validate detected device-id matches expected value for spoofing
- **Exact behavior**: Print "Device ID: 0x9a49, Expected: 0x9a49, Match: YES"
- **What logs to add**: DBGLOG showing device validation results
- **Expected result**: Visible validation showing correct device ID
- **Failure signal**: Device ID mismatch
- **How to test**: Boot and verify validation log shows correct match
- **How to rollback**: Remove validation log statements

## Task 8: Add Panic-Safe Minimal MMIO Logging
- **File(s) to modify**: `NootedGreen/kern_green.cpp`
- **Function/area**: MMIO access functions
- **Purpose**: Add safe MMIO read logging for debugging without causing panics
- **Exact behavior**: Use trycatch blocks to safely read MMIO registers and log values
- **What logs to add**: DBGLOG for MMIO reads like "BAR0 + 0x10 = 0x12345678"
- **Expected result**: Visible MMIO register values in kernel log
- **Failure signal**: MMIO read causes panic (will be caught by trycatch)
- **How to test**: Boot and check kernel log for MMIO values
- **How to rollback**: Remove MMIO logging code

## Task 9: Add Forcewake and Power Well Verification Logs
- **File(s) to modify**: `NootedGreen/kern_green.cpp`
- **Function/area**: Power management initialization
- **Purpose**: Verify forcewake domains and power wells are enabled
- **Exact behavior**: Print "Forcewake enabled: YES", "Power well 0: active"
- **What logs to add**: DBGLOG after power initialization
- **Expected result**: Visible confirmation of power state
- **Failure signal**: Power well not coming up
- **How to test**: Boot and verify power state logs
- **How to rollback**: Remove the power verification logs

## Task 10: Add Display Pipe Transaction Correlation Logs
- **File(s) to modify**: `NootedGreen/kern_green.cpp`
- **Function/area**: Framebuffer configuration code
- **Purpose**: Correlate display pipe creation with other boot events
- **Exact behavior**: Print "Display pipe transaction started at timestamp X", "Transaction completed after Y ms"
- **What logs to add**: DBGLOG with timing information around display pipe operations
- **Expected result**: Visible timing data for display bring-up
- **Failure signal**: Transaction not completing or timing out
- **How to test**: Boot and analyze timing data in kernel log
- **How to rollback**: Remove the timing logs

## Implementation Priority Order
1. Task 1 (Boot-Stage Markers) - HIGH PRIORITY - Fundamental debugging capability
2. Task 2 (Version Print) - HIGH PRIORITY - Basic version tracking
3. Task 7 (Device-ID Sanity Check) - HIGH PRIORITY - Quick validation
4. Task 4 (Kext Tracer) - MEDIUM PRIORITY - Component tracking
5. Task 3 (Property Dump) - MEDIUM PRIORITY - Property injection verification
6. Task 5 (Framebuffer Tracer) - MEDIUM PRIORITY - Service attachment tracking
7. Task 6 (EDID/VBT Parsing) - MEDIUM PRIORITY - Data parsing confirmation
8. Task 8 (MMIO Logging) - LOW PRIORITY - Advanced debugging
9. Task 9 (Power Verification) - LOW PRIORITY - Power state tracking
10. Task 10 (Transaction Timing) - LOW PRIORITY - Performance analysis