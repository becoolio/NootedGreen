# Implementation Plan - Staged Approach

## Phase 1 - Confirm NootedGreen Load, Version, and Patch Routing
**Objective**: Verify NootedGreen loads correctly with proper version and patches

**Code changes**:
- Add build version print at startup (Task #10)

**Logs to add**:
- Git commit hash
- Build timestamp

**Test flags**: None (current flags sufficient)

**Expected boot outcome**: All stages complete, version visible in dmesg

**Failure signal**: Stage fails, version not visible

**Rollback method**: Revert to previous source

**Success looks like**: "NootedGreen V1.0.4 - STARTING" shows commit hash

**Regression looks like**: Missing version info, stages fail

---

## Phase 2 - Confirm IGPU Matching and IORegistry Properties
**Objective**: Verify IGPU matches and properties are correct

**Code changes**:
- Add device-id spoof verification (Task #6)
- Add platform-id injection (Task #1)

**Logs to add**:
- Original device-id before spoof
- Spoofed device-id after injection
- platform-id value after injection

**Test flags**: None (current flags sufficient)

**Expected boot outcome**: device-id=0x9a49, platform-id shows correct value

**Failure signal**: device-id not spoofed, platform-id still 0x00000000

**Rollback method**: Remove injection code

**Success looks like**: "device-id=0x9a49 platform-id=0x8A987600" (example)

**Regression looks like**: platform-id shows 0x00000000

---

## Phase 3 - Confirm AppleIntel Graphics Component Matching
**Objective**: Verify AppleIntel kexts load and match to IGPU

**Code changes**: None (already working)

**Logs to add**:
- Framebuffer property publication validator (Task #5)

**Test flags**: None

**Expected boot outcome**: All AppleIntel kexts load and match

**Failure signal**: Kexts fail to load

**Rollback method**: N/A

**Success looks like**: "Framebuffer personality published", "Accelerator personality published"

**Regression looks like**: Kext matching failures

---

## Phase 4 - Confirm Framebuffer Attach and Display State
**Objective**: Verify framebuffer attaches and display is online

**Code changes**: None (already working)

**Logs to add**:
- Link training status verification (Task #7)

**Test flags**: None

**Expected boot outcome**: Framebuffer attaches, display online, link trained

**Failure signal**: Framebuffer fails, display offline

**Rollback method**: N/A

**Success looks like**: "V90L4R: realTGL linkRate=24 bpp=10 laneCount=2"

**Regression looks like**: Link training failures

---

## Phase 5 - Confirm CoreDisplay and WindowServer Behavior
**Objective**: Verify CoreDisplay initializes and WindowServer starts

**Code changes**:
- Add CoreDisplay DisplayPipe error tracer (Task #2)

**Logs to add**:
- DisplayPipe error details
- CoreDisplay transaction state

**Test flags**: -NGreenDebug (already set)

**Expected boot outcome**: CoreDisplay initializes without fatal errors

**Failure signal**: CoreDisplay errors become fatal

**Rollback method**: Remove error tracer

**Success looks like**: "V111 fbcontroller post-registerService" without errors

**Regression looks like**: Fatal CoreDisplay errors, WindowServer crash

---

## Phase 6 - Confirm IOAccelerator Service Publication
**Objective**: Verify IOAccelerator services publish correctly

**Code changes**:
- Add IOAccelDisplayPipeUserClient2 creation tracer (Task #3)

**Logs to add**:
- User client creation events
- Accelerator service state

**Test flags**: None

**Expected boot outcome**: IOAccelerator publishes, user client created

**Failure signal**: Services don't publish

**Rollback method**: Remove tracer

**Success looks like**: IOAccelerator visible in ioreg with user clients

**Regression looks like**: IOAccelerator not visible

---

## Phase 7 - Confirm Display Pipe Behavior
**Objective**: Verify display pipe creates successfully

**Code changes**:
- Add RCS ring state dump (Task #4)

**Logs to add**:
- RCS ring register values
- Display pipe state

**Test flags**: None

**Expected boot outcome**: Display pipe created, RCS ring active

**Failure signal**: Display pipe fails, RCS stuck

**Rollback method**: Remove ring dump

**Success looks like**: RCS_HEAD/RCS_TAIL advancing

**Regression looks like**: RCS stuck at 0, DisplayPipe error

---

## Phase 8 - Confirm Forcewake, Power Wells, and Rings
**Objective**: Verify GPU power management working

**Code changes**: None (already working)

**Logs to add**: None needed

**Test flags**: None

**Expected boot outcome**: Power wells respond, forcewake works

**Failure signal**: Power wells stuck

**Rollback method**: N/A

**Success looks like**: "releaseDoorbell called" messages

**Regression looks like**: Power well timeouts

---

## Phase 9 - Confirm GuC/HuC/DMC Handling
**Objective**: Verify firmware handling correct

**Code changes**: None (already working - DMC loaded)

**Logs to add**: None needed

**Test flags**: -disablegfxfirmware (already set)

**Expected boot outcome**: DMC loads, GuC skipped (as intended)

**Failure signal**: DMC fails to load

**Rollback method**: N/A

**Success looks like**: "TGL DMC loaded"

**Regression looks like**: DMC load failures

---

## Phase 10 - Attempt Minimal Acceleration Enablement
**Objective**: Enable acceleration features

**Code changes**:
- Add acceleration gate verification (Task #9)

**Logs to add**:
- Acceleration gate status

**Test flags**: Consider adding -allow3d

**Expected boot outcome**: Acceleration enabled, Metal functional

**Failure signal**: Acceleration fails, Metal broken

**Rollback method**: Remove -allow3d flag

**Success looks like**: Metal fully functional, system_profiler shows GPU

**Regression looks like**: Black screen, acceleration broken

---

## Phase 11 - Stabilize Acceleration
**Objective**: Fix any acceleration issues

**Code changes**: Based on findings from Phase 10

**Logs to add**: As needed

**Test flags**: As needed

**Expected boot outcome**: Stable acceleration

**Failure signal**: Acceleration unstable

**Rollback method**: Revert changes

---

## Phase 12 - Metal Debugging (Only after Phase 11)
**Objective**: Fix any Metal issues

**Code changes**: Based on findings

**Logs to add**: Metal-specific tracing

**Test flags**: As needed

**Expected boot outcome**: Metal fully functional

**Failure signal**: Metal broken

**Rollback method**: Revert changes