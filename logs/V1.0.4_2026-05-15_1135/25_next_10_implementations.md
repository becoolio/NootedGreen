# Next 10 Implementation Tasks

## Task 1: Add Boot-Stage Marker Logging in NootedGreen
- **File(s) to modify**: NootedGreen/kern_gen11.cpp, NootedGreen/kern_main.cpp
- **Function/class/area**: init(), processKext(), probe() methods
- **Purpose**: Track NootedGreen initialization stages
- **Exact behavior**: Add DBGLOG statements at each boot stage (stage 1-5)
- **What logs to add**: Stage markers, timing info, key decisions
- **Expected result**: Clear stage-by-stage boot timeline in logs
- **Failure signal**: Missing stage markers in verbose boot
- **How to test**: Reboot with -v and -liludbgall, check logs
- **Rollback**: Remove DBGLOG statements if too verbose

## Task 2: Add Version/Git Commit Print at Boot
- **File(s) to modify**: NootedGreen/kern_main.cpp
- **Function/class/area**: pluginStart()
- **Purpose**: Verify correct version loaded
- **Exact behavior**: Print version string, Git commit (if available), build date
- **What logs to add**: "[NootedGreen] Version: X.Y.Z, Commit: ABC123, Built: YYYY-MM-DD"
- **Expected result**: Version visible in dmesg/kext logs
- **Failure signal**: Version not printed or wrong version
- **How to test**: Reboot, check dmesg for version line
- **Rollback**: Comment out version print code

## Task 3: Add IGPU Match Verification and Property Dump
- **File(s) to modify**: NootedGreen/kern_gen11.cpp
- **Function/class/area**: IGPU device matching
- **Purpose**: Verify IGPU matched correctly and properties injected
- **Exact behavior**: Log device-id, vendor-id, platform-id, AAPL,ig-platform-id
- **What logs to add**: "[NootedGreen] IGPU Match: device-id=0xXXXX, platform-id=0xXXXX, model=XXXX"
- **Expected result**: All IGPU properties visible in logs
- **Failure signal**: Properties missing or wrong values
- **How to test**: Reboot, check for IGPU match log lines
- **Rollback**: Remove property dump logging

## Task 4: Add AppleIntel Graphics Component Load Tracer
- **File(s) to modify**: NootedGreen/kern_gen11.cpp
- **Function/class/area**: processKext() callback
- **Purpose**: Track which AppleIntel kexts are loading
- **Exact behavior**: Log when AppleIntel kexts are processed and matched
- **What logs to add**: "[NootedGreen] processKext: bundleid=XXXX, matched=YES/NO"
- **Expected result**: All AppleIntel kext processing visible
- **Failure signal**: No log output for AppleIntel kexts
- **How to test**: Reboot, check for processKext lines
- **Rollback**: Remove tracer code

## Task 5: Add Framebuffer Attach Tracer
- **File(s) to modify**: NootedGreen/kern_gen11.cpp
- **Function/class/area**: framebuffer personality publishing
- **Purpose**: Track when framebuffer services appear
- **Exact behavior**: Log when AppleIntelFramebufferController starts
- **What logs to add**: "[NootedGreen] Framebuffer Controller started"
- **Expected result**: Framebuffer startup logged
- **Failure signal**: No framebuffer log
- **How to test**: Reboot, check for framebuffer lines
- **Rollback**: Remove tracer

## Task 6: Add Framebuffer Property Publication Validator
- **File(s) to modify**: NootedGreen/kern_gen11.cpp
- **Function/class/area**: Framebuffer property injection
- **Purpose**: Verify all expected properties are published
- **Exact behavior**: Log each property as it's published (display count, connector info)
- **What logs to add**: "[NootedGreen] Property: XXXX = YYYY"
- **Expected result**: All properties logged during framebuffer init
- **Failure signal**: Properties missing from logs
- **How to test**: Reboot, verify all expected properties
- **Rollback**: Remove validation logging

## Task 7: Add Display Pipe Capability Publication Tracer
- **File(s) to modify**: NootedGreen/kern_gen11.cpp
- **Function/class/area**: DisplayPipe service publishing
- **Purpose**: Track when display pipe services are created
- **Exact behavior**: Log IOAccelDisplayPipeCapabilities publication
- **What logs to add**: "[NootedGreen] DisplayPipe capabilities published"
- **Expected result**: Display pipe creation logged
- **Failure signal**: No log output
- **How to test**: Reboot, check for display pipe lines
- **Rollback**: Remove tracer code

## Task 8: Add Test Flag to Suppress Display Pipe Publication
- **File(s) to modify**: NootedGreen/kern_gen11.cpp, kern.hpp
- **Function/class/area**: Boot flag parsing
- **Purpose**: Allow testing without display pipe to isolate issues
- **Exact behavior**: Add -ngreenNoDisplayPipe boot flag
- **What logs to add**: "[NootedGreen] DisplayPipe publication suppressed"
- **Expected result**: No IOAccelDisplayPipe services when flag set
- **Failure signal**: Display pipe still created
- **How to test**: Reboot with flag, verify no display pipe
- **Rollback**: Remove flag handling code

## Task 9: Add Test Flag to Publish Only One Display Pipe
- **File(s) to modify**: NootedGreen/kern_gen11.cpp, kern.hpp
- **Function/class/area**: Boot flag parsing
- **Purpose**: Limit to single pipe for debugging
- **Exact behavior**: Add -ngreenSinglePipe boot flag
- **What logs to add**: "[NootedGreen] Single pipe mode enabled"
- **Expected result**: Only one display pipe instance
- **Failure signal**: Multiple pipes still created
- **How to test**: Reboot with flag, verify single pipe
- **Rollback**: Remove flag handling code

## Task 10: Add GuC/HuC/DMC Firmware Decision Logs
- **File(s) to modify**: NootedGreen/kern_gen11.cpp
- **Function/class/area**: Firmware loading decisions
- **Purpose**: Track which firmware is loaded/skipped
- **Exact behavior**: Log GuC/HuC/DMC load decisions and results
- **What logs to add**: "[NootedGreen] GuC: SKIP/LOAD/ERROR, HuC: SKIP/LOAD/ERROR, DMC: SKIP/LOAD/ERROR"
- **Expected result**: Clear firmware decision trace
- **Failure signal**: No firmware logs visible
- **How to test**: Reboot, check for firmware logs
- **Rollback**: Remove firmware logging

## Implementation Priority Order
1. Task 2 (Version print) - Quick verification win
2. Task 1 (Stage markers) - Foundation for debugging
3. Task 3 (IGPU dump) - Verify matching works
4. Task 10 (Firmware logs) - Important for current state
5. Task 4 (Component tracer) - Track kext loading
6. Task 5 (Framebuffer tracer) - Track display init
7. Task 6 (Property validator) - Verify injection
8. Task 7 (DisplayPipe tracer) - Track pipe creation
9. Task 8 (NoDisplayPipe flag) - Test isolation tool
10. Task 9 (SinglePipe flag) - Test isolation tool