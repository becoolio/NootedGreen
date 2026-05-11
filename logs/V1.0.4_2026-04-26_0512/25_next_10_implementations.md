# Next 10 Implementations - V1.0.4_2026-04-26_0512

Based on the analysis, here are 10 concrete implementation tasks for the next phase:

## Task 1: Add Boot Stage Marker Logging Across All Startup Phases
- **Files to modify**: NootedGreen.cpp, Internal.cpp, or corresponding startup code
- **Function/area**: `start()` method and early initialization
- **Purpose**: Add granular boot-stage "===== STAGE X =====" markers at each phase
- **Exact behavior**: Print stage markers at start, IGPU detection, framebuffer publish, accelerator init, display ready
- **Logs to add**: "NootedGreen: ===== STAGE 1: EARLY INIT =====" through all phases
- **Expected result**: Clear visual separation of boot phases in dmesg
- **Failure signal**: Stage markers missing from output
- **How to test**: Reboot and check dmesg for marker lines

## Task 2: Add NootedGreen Build/Version/Git Commit Print at Boot
- **Files to modify**: NootedGreen.cpp or main entry point
- **Function/area**: Early init or driver start
- **Purpose**: Print exact build version and Git info at boot
- **Exact behavior**: Print "NootedGreen: V1.0.4 build __DATE__ __TIME__ commit GIT_HASH"
- **Logs to add**: Build info at top of dmesg output
- **Expected result**: Version visible in every boot dmesg
- **Failure signal**: No version in logs
- **How to test**: Reboot and check for version line

## Task 3: Add IGPU Match Verification and Property Dump
- **Files to modify**: IGPU configuration/matching code
- **Function/area**: `processPatcher()` or IGPU matching
- **Purpose**: Dump all relevant IGPU properties after matching
- **Exact behavior**: Print device-id, vendor-id, class-code, AAPL,ig-platform-id, etc.
- **Logs to add**: IGPU properties after match
- **Expected result**: All IGPU properties visible
- **Failure signal**: Properties missing
- **How to test**: Reboot and check IGPU properties in dmesg

## Task 4: Add AppleIntel Graphics Component Load/Match Tracer
- **Files to modify**: Framebuffer and accelerator attach code
- **Function/area**: `processKext()` method
- **Purpose**: Trace all AppleIntel graphics kext loading
- **Exact behavior**: Log when framebuffer loads, accelerator loads, any failures
- **Logs to add**: "AppleIntelTGLGraphicsFramebuffer LOADED", "AppleIntelTGLGraphics LOADED"
- **Expected result**: Clear component load sequence
- **Failure signal**: Missing load logs
- **How to test**: Reboot and verify load logs

## Task 5: Add Framebuffer Property Publication Validator
- **Files to modify**: Framebuffer personality publication code
- **Function/area**: Publishing code
- **Purpose**: Verify and log all published properties
- **Exact behavior**: Print `AAPL,slot-name`, `device-id`, `framebuffer-count`, etc.
- **Logs to add**: All published properties
- **Expected result**: Publication visibility
- **Failure signal**: Publication silent or partial
- **How to test**: Reboot and verify publication

## Task 6: Add Test Flag to Suppress Display Pipe Publication
- **Files to modify**: Driver configuration or start method
- **Function/area**: Driver flags processing
- **Purpose**: Add boot arg `-ngreenNoDisplayPipe` to skip pipe setup
- **Exact behavior**: Check for flag, skip pipe creation if present
- **Logs to add**: "DisplayPipe disabled by flag" if suppressed
- **Expected result**: No pipe logs with flag set
- **Failure signal**: Pipe still created
- **How to test**: Boot with -ngreenNoDisplayPipe

## Task 7: Add Test Flag to Publish Only One Display Pipe
- **Files to modify**: Framebuffer code or configuration
- **Function/area**: Pipe creation loop
- **Purpose**: Add boot arg `-ngreenSinglePipe` to limit pipes
- **Exact behavior**: Only create pipe 0, skip others if flag set
- **Logs to add**: "Single pipe mode enabled"
- **Expected result**: Only one pipe visible
- **Failure signal**: Multiple pipes still
- **How to test**: Boot with -ngreenSinglePipe

## Task 8: Add Display Pipe Capability Publication Tracer
- **Files to modify**: Display pipe creation code
- **Function/area**: Pipe capability setup
- **Purpose**: Log all pipe capabilities being published
- **Exact behavior**: Print capability flags being set
- **Logs to add**: "DisplayPipe cap: transactionsSupported=1 displayPipeSupported=1"
- **Expected result**: Visibility of what's enabled
- **Failure signal**: Silent capability setup
- **How to test**: Reboot and check logs

## Task 9: Add Ring State Dump Before Display Pipe Creation
- **Files to modify**: Ring initialization or display code
- **Function/area**: Pre-pipe ring setup
- **Purpose**: Log ring states before pipe creation
- **Exact behavior**: Read and print RCS_HEAD, RCS_TAIL, RCS_CTL, ERROR_GEN6
- **Logs to add**: Ring MMIO values
- **Expected result**: Ring state visible
- **Failure signal**: No ring dump
- **How to test**: Reboot and check ring logs

## Task 10: Add GuC/HuC/DMC Firmware Decision Logs
- **Files to modify**: Firmware loading code
- **Function/area**: Firmware initialization
- **Purpose**: Log all firmware handling decisions
- **Exact behavior**: Print "GuC: load/skip/error", "HuC: load/skip", "DMC: load/skip"
- **Logs to add**: Firmware decisions at boot
- **Expected result**: Clear firmware path visible
- **Failure signal**: Firmware handling silent
- **How to test**: Reboot and check firmware logs

## Recommended Priority

1. **HIGH**: Task 1 (stage markers) - Most critical for debugging
2. **HIGH**: Task 2 (version print) - Basic visibility  
3. **HIGH**: Task 4 (component tracer) - Graphics flow debugging
4. **MEDIUM**: Task 3 (IGPU dump) - Platform verification
5. **MEDIUM**: Task 9 (ring dump) - Ring debugging
6. **MEDIUM**: Task 10 (firmware decision) - GuC debugging
7. **LOW**: Task 6/7 (test flags) - Optional debugging aids
8. **LOW**: Task 5/8 (property validation) - Advanced