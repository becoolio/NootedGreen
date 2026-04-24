# Next 10 Implementations

## Implementation Tasks

### 1. Add boot-stage marker logging to NootedGreen startup
**Files to modify**: NootedGreen/kern_green.cpp
**Function/area**: Driver entry point and early initialization
**Purpose**: Track exactly where NootedGreen initialization fails or succeeds
**Exact behavior**: Add DBGLOG statements at each stage: load, probe, start, IGPU detection, property injection
**What logs to add**: "NootedGreen: Stage X - [description]" at each initialization step
**Expected result**: Visible boot logs showing progression through NootedGreen initialization
**Failure signal**: No logs appear even with -NGreenDebug flag
**How to test**: Reboot with -v -NGreenDebug and check kernel logs
**How to rollback**: Revert changes to kern_green.cpp

### 2. Add version and Git commit print at boot
**Files to modify**: NootedGreen/kern_green.cpp
**Function/area**: Driver start routine
**Purpose**: Confirm which exact version is loaded
**Exact behavior**: Print source version, build timestamp, and Git commit if available
**What logs to add**: "NootedGreen: Version X.Y.Z loaded, Git: [commit]"
**Expected result**: Version visible in boot logs
**Failure signal**: No version info in logs
**How to test**: Check boot logs with -NGreenDebug
**How to rollback**: Remove version print statements

### 3. Add IGPU match verification and full property dump
**Files to modify**: NootedGreen/kern_green.cpp, NootedGreen/kern_gen11.cpp
**Function/area**: IGPU detection and property injection
**Purpose**: Verify IGPU matching and dump injected properties
**Exact behavior**: Log device-id, vendor-id, platform-id before and after spoofing
**What logs to add**: "IGPU: Original device-id=0xXXXX, After spoof=0xYYYY"
**Expected result**: Clear visibility into property modifications
**Failure signal**: Properties not appearing in IORegistry
**How to test**: Compare IORegistry with boot logs
**How torollback**: Remove dump code

### 4. Add AppleIntel graphics component load/match tracer
**Files to modify**: NootedGreen/kern_green.cpp
**Function/area**: Patching and hooking layer
**Purpose**: Track which AppleIntel kexts are being patched
**Exact behavior**: Log each kext that NootedGreen processes
**What logs to add**: "NootedGreen: Processing kext [name], version [ver]"
**Expected result**: Visible patching activity
**Failure signal**: No patching logs despite kexts loading
**How to test**: Check boot logs after AppleIntel kexts load
**How to rollback**: Remove tracer code

### 5. Add framebuffer attach tracer
**Files to modify**: NootedGreen/kern_gen11.cpp
**Function/area**: Framebuffer personality handling
**Purpose**: Confirm framebuffer attaches and publishes services
**Exact behavior**: Log when IOFramebuffer personality is published
**What logs to add**: "Framebuffer: Personality published, probe score [score]"
**Expected result**: Framebuffer attach confirmation
**Failure signal**: No attach confirmation but display works (unusual)
**How to test**: Check boot logs and IORegistry
**How to rollback**: Remove attach tracer

### 6. Add test flag to suppress display pipe publication
**Files to modify**: NootedGreen/Info.plist, NootedGreen/kern_*.cpp
**Function/area**: Accelerator personality handling
**Purpose**: Isolate acceleration issues by testing without display pipe
**Exact behavior**: Add boot flag -ngreenNoDisplayPipe to suppress IOAccelDisplayPipeUserClient2
**What logs to add**: "Accelerator: Display pipe suppressed by flag"
**Expected result**: Boot with no display pipe, test if that fixes hangs
**Failure signal**: Flag has no effect
**How to test**: Reboot with -ngreenNoDisplayPipe
**How to rollback**: Remove flag handling, use normal boot

### 7. Add test flag to publish only one display pipe
**Files to modify**: NootedGreen/Info.plist, NootedGreen/kern_*.cpp
**Function/area**: Accelerator personality handling
**Purpose**: Test if dual-pipe configuration causes issues
**Exact behavior**: Add boot flag -ngreenSinglePipe to limit to single pipe
**What logs to add**: "Accelerator: Single pipe mode enabled"
**Expected result**: Only one display pipe published
**Failure signal**: Flag has no effect
**How to test**: Reboot with -ngreenSinglePipe
**How to rollback**: Remove flag handling

### 8. Add IOAccelerator attach tracer
**Files to modify**: NootedGreen/kern_green.cpp, kern_gen11.cpp
**Function/area**: Accelerator personality handling
**Purpose**: Verify IOAccelerator personality attaches
**Exact behavior**: Log when accelerator service is published
**What logs to add**: "Accelerator: Service published, name [name], bundle [id]"
**Expected result**: Clear visibility into accelerator binding
**Failure signal**: No attach confirmation
**How to test**: Check boot logs for accelerator publishing
**How to rollback**: Remove attach tracer

### 9. Add forcewake and powerwell verification before ring use
**Files to modify**: NootedGreen/kern_gen11.cpp
**Function/area**: Hardware initialization
**Purpose**: Verify GPU power state before attempting ring operations
**Exact behavior**: Check and log forcewake domain status, power well states
**What logs to add**: "HW: Forcewake domains: 0xXXXX, Power wells: 0xYYYY"
**Expected result**: Visible power state before any hang
**Failure signal**: Power not ready when rings initialized
**How to test**: Check boot logs before any graphics operations
**How to rollback**: Remove power check code

### 10. Add device-id/platform-id sanity check
**Files to modify**: NootedGreen/kern_green.cpp
**Function/area**: Property injection
**Purpose**: Validate spoofed values are sensible
**Exact behavior**: Compare injected device-id against whitelist, log warnings
**What logs to add**: "Props: device-id 0x9a49 validated for TGL"
**Expected result**: Validation success or clear warning on invalid values
**Failure signal**: Invalid device-id accepted silently
**How to test**: Inject invalid device-id and check logs
**How to rollback**: Remove validation code