# Next 10 Implementations - V1.0.4_2026-04-28_2215

## Task 1: Add Boot-Stage Marker Logging Across NootedGreen Startup
- **File(s) to modify:** NootedGreen entry point files (kern_*.cpp)
- **Function/area to modify:** Plugin start function and early initialization
- **Purpose:** Track exactly where NootedGreen initialization reaches
- **Exact behavior to implement:** Add systematic boot-stage printf/log markers at each initialization step (start, init, probe, match, attach)
- **What logs to add:** "NootedGreen: Stage X - [description]" at each major initialization step
- **Expected result:** Detailed boot sequence visible in kernel logs
- **Failure signal:** Missing stages in logs indicate hang location
- **How to test after reboot:** Check kernel logs for sequential stage markers
- **How to rollback if it fails:** Remove added logging calls

## Task 2: Add NootedGreen Build/Version/Git Commit Print at Boot
- **File(s) to modify:** NootedGreen version source or main entry
- **Function/area to modify:** Plugin startup code
- **Purpose:** Verify exact binary version running
- **Exact behavior to implement:** Print source commit hash, build version, build timestamp at boot
- **What logs to add:** "NootedGreen: version=X.Y.Z commit=abcdefg built=YYYY-MM-DD HH:MM"
- **Expected result:** Version info clearly visible in boot logs
- **Failure signal:** Missing version info indicates stale build
- **How to test after reboot:** grep boot logs for version string
- **How to rollback if it fails:** Remove version print statement

## Task 3: Add IGPU Match Verification and Full Property Dump
- **File(s) to modify:** NootedGreen IGPU handling code
- **Function/area to modify:** IGPU matching/probing function
- **Purpose:** Verify correct GPU detection and show all relevant properties
- **Exact behavior to implement:** After matching IGPU, dump device-id, vendor-id, class-code, AAPL,ig-platform-id, framebuffer properties
- **What logs to add:** "NootedGreen: IGPU matched id=0xXXXX platform-id=0xXXXX"
- **Expected result:** Complete IGPU property dump visible
- **Failure signal:** Missing properties indicate incomplete matching
- **How to test after reboot:** Check logs for property values
- **How to rollback if it fails:** Remove dump code

## Task 4: Add AppleIntel Graphics Component Load/Match Tracer
- **File(s) to modify:** NootedGreen patch routing code
- **Function/area to modify:** processKext or similar routing function
- **Purpose:** Trace which AppleIntel kexts are being loaded and matched
- **Exact behavior to implement:** Log each AppleIntel graphics kext load, match success/failure
- **What logs to add:** "NootedGreen: Loaded [kextname] match=[success/fail]"
- **Expected result:** All AppleIntel graphics kext loads traced
- **Failure signal:** Unmatched kexts without explanation
- **How to test after reboot:** Check for traced kext loads
- **How to rollback if it fails:** Remove tracer code

## Task 5: Add Framebuffer Attach Tracer
- **File(s) to modify:** NootedGreen framebuffer handling
- **Function/area to modify:** Framebuffer probe/attach code
- **Purpose:** Track framebuffer attach sequence
- **Exact behavior to implement:** Log framebuffer probe, start, and publish events
- **What logs to add:** "NootedGreen:Framebuffer probe/start/publish"
- **Expected result:** Clear framebuffer lifecycle visible
- **Failure signal:** Missing stages indicate failure point
- **How to test after reboot:** Check logs for framebuffer events
- **How to rollback if it fails:** Remove logging

## Task 6: Add Framebuffer Property Publication Validator
- **File(s) to modify:** NootedGreen framebuffer property injection code
- **Function/area to modify:** After property injection
- **Purpose:** Confirm injected properties are visible to system
- **Exact behavior to implement:** After injecting properties, verify they're in IORegistry
- **What logs to add:** "NootedGreen: Property [name] published successfully"
- **Expected result:** All injected properties confirmed visible
- **Failure signal:** Property missing indicates injection failure
- **How to test after reboot:** Compare logged vs actual IORegistry
- **How to rollback if it fails:** Remove validation code

## Task 7: Add Display Pipe Capability Publication Tracer
- **File(s) to modify:** NootedGreen accelerator handling
- **Function/area to modify:** Display pipe publish code
- **Purpose:** Track display pipe publication
- **Exact behavior to implement:** Log display pipe capability publication
- **What logs to add:** "NootedGreen: DisplayPipe capabilities published"
- **Expected result:** Display pipe events visible
- **Failure signal:** No publication indicates hang point
- **How to test after reboot:** Check logs for pipe events
- **How to rollback if it fails:** Remove logging

## Task 8: Add Test Flag to Suppress Display Pipe Publication
- **File(s) to modify:** NootedGreen accelerator code
- **Function/area to modify:** Display pipe creation
- **Purpose:** Allow testing without display pipe to isolate issues
- **Exact behavior to implement:** Check boot-args for -ngreenNoDisplayPipe flag; if present, skip display pipe publication
- **What logs to add:** "NootedGreen: Display pipe suppressed by boot flag"
- **Expected result:** System boots without display pipe if flag present
- **Failure signal:** Flag ignored, pipe still published
- **How to test after reboot:** Boot with flag, verify no pipe
- **How to rollback if it fails:** Remove flag handling

## Task 9: Add Test Flag to Publish Only One Display Pipe
- **File(s) to modify:** NootedGreen framebuffer code
- **Function/area to modify:** Framebuffer instance creation
- **Purpose:** Simplify configuration for debugging
- **Exact behavior to implement:** Check boot-args for -ngreenSinglePipe; if present, only publish first framebuffer
- **What logs to add:** "NootedGreen: Single pipe mode active"
- **Expected result:** Only one pipe visible when flag set
- **Failure signal:** Both pipes still published
- **How to test after reboot:** Boot with flag, check pipe count
- **How to rollback if it fails:** Remove flag handling

## Task 10: Add IOAccelerator Attach Tracer
- **File(s) to modify:** NootedGreen accelerator handling
- **Function/area to modify:** IOAccelerator attach function
- **Purpose:** Track accelerator service attachment
- **Exact behavior to implement:** Log accelerator service start and user client creation
- **What logs to add:** "NootedGreen: IOAccelerator start/userclient"
- **Expected result:** Accelerator lifecycle visible in logs
- **Failure signal:** Missing stages indicate hang location
- **How to test after reboot:** Check logs for accelerator events
- **How to rollback if it fails:** Remove logging