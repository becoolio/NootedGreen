# Implementation Plan - V1.0.0

## Phase 1 - Confirm NootedGreen Load, Version, and Patch Routing
**Objective**: Verify NootedGreen is loading correctly and applying patches

### Code Changes
- Add boot-stage marker logging in NootedGreen start() function
- Add version/commit print at boot
- Add function routing logs

### Logs to Add
- Stage markers: load start, Lilu attach, IGPU detection, property injection, patch routing, completion
- Version and commit hash
- Function route creation events

### Test Flags
- `-liludbg` - Already active
- `-NGreenDebug` - Already active
- Add new stage markers will use existing debug infrastructure

### Expected Boot Outcome
- All stage markers visible in logs
- Version/commit printed at boot
- NootedGreen shows as loaded and matched in IORegistry

### Failure Signal
- No markers despite kext loading
- Version not printed
- Kext shows as !matched in IORegistry

### Rollback Method
- Remove DBGLOG statements added

### What Success Looks Like
- Complete initialization trace in 18_lilu_nootedgreen_logs.txt
- Clear picture of where initialization succeeds or fails

### What Regression Would Look Like
- Kext fails to load where it previously loaded
- New errors appear in logs

---

## Phase 2 - Confirm IGPU Matching and IORegistry Properties
**Objective**: Verify IGPU is detected and properties are injected correctly

### Code Changes
- Add IGPU match verification and property dump after matching
- Log device-id, vendor-id, class-code, AAPL,ig-platform-id

### Logs to Add
- IGPU match confirmation
- All relevant IGPU properties after matching

### Test Flags
- Use existing -NGreenDebug

### Expected Boot Outcome
- IGPU shows as GFX0 in IORegistry
- device-id shows spoofed value (0x9a49)
- AAPL,ig-platform-id present and correct

### Failure Signal
- IGPU not found in IORegistry
- device-id not spoofed
- platform-id missing or wrong

### Rollback Method
- Remove property logging

### What Success Looks Like
- Complete IGPU property dump in logs matching expected values

### What Regression Would Look Like
- IGPU disappears from IORegistry
- Properties missing

---

## Phase 3 - Confirm AppleIntel Graphics Component Matching
**Objective**: Verify AppleIntel graphics kexts load and match to IGPU

### Code Changes
- Add AppleIntel graphics component load tracer
- Log each AppleIntel kext load event

### Logs to Add
- AppleIntel* kext names as they load
- Match success/failure for each

### Test Flags
- None required

### Expected Boot Outcome
- AppleIntelTGLGraphics.kext loads
- AppleIntelTGLGraphicsFramebuffer.kext loads
- Components match to IGPU

### Failure Signal
- No AppleIntel kexts load
- Kexts load but don't match

### Rollback Method
- Remove tracer code

### What Success Looks Like
- Logs show AppleIntel kexts loading and matching

### What Regression Would Look Like
- AppleIntel kexts stop loading
- Matching failures appear

---

## Phase 4 - Confirm Framebuffer Attach and Display State
**Objective**: Verify framebuffer attaches and display comes online

### Code Changes
- Add framebuffer attach tracer
- Add framebuffer property publication validator

### Logs to Add
- Framebuffer class, index, properties on attach
- All published properties after injection

### Test Flags
- None required

### Expected Boot Outcome
- IOFramebuffer present in IORegistry
- Display shows online
- Resolution correct (1920x1080)

### Failure Signal
- IOFramebuffer missing
- Display offline
- Wrong resolution

### Rollback Method
- Remove tracer/validator code

### What Success Looks Like
- Clear framebuffer attach logs
- Complete property list

### What Regression Would Look Like
- Framebuffer disappears
- Display goes offline

---

## Phase 5 - Confirm CoreDisplay and WindowServer Behavior
**Objective**: Verify CoreDisplay initializes and WindowServer starts

### Code Changes
- Add CoreDisplay transaction correlation logs
- Already have some WindowServer logs from task

### Logs to Add
- CoreDisplay init events
- Display transaction completion
- WindowServer start/crash events

### Test Flags
- None required

### Expected Boot Outcome
- CoreDisplay initializes
- WindowServer starts
- No display transaction timeouts

### Failure Signal
- CoreDisplay errors
- WindowServer crashes
- Transaction timeouts

### Rollback Method
- Remove correlation logs

### What Success Looks Like
- Clean CoreDisplay init
- WindowServer running

### What Regression Would Look Like
- CoreDisplay errors increase
- WindowServer crashes

---

## Phase 6 - Confirm IOAccelerator Service Publication
**Objective**: Verify IOAccelerator service appears and publishes correctly

### Code Changes
- Add IOAccelerator attach tracer

### Logs to Add
- Accelerator class, bundle ID, properties

### Test Flags
- None required

### Expected Boot Outcome
- IOAcceleratorFamily2 or IOAccelerator service present
- Bundle ID shows proper AppleIntel accelerator

### Failure Signal
- No accelerator service
- Generic "com.unknown.bundle" shows up

### Rollback Method
- Remove tracer

### What Success Looks Like
- Proper AppleIntel accelerator bundle ID visible

### What Regression Would Look Like
- Accelerator disappears
- Generic bundle ID appears

---

## Phase 7 - Confirm Display Pipe Behavior
**Objective**: Verify display pipe creation and identify issues

### Code Changes
- Add display pipe capability publication tracer

### Logs to Add
- Display pipe indexes and capabilities

### Test Flags
- -ngreenNoDisplayPipe (to suppress)
- -ngreenSinglePipe (single pipe mode)

### Expected Boot Outcome
- Display pipes created successfully
- No timeout/stamp errors

### Failure Signal
- Display pipe timeout
- StampIdx errors

### Rollback Method
- Remove tracer, don't use test flags

### What Success Looks Like
- Clean display pipe logs

### What Regression Would Look Like
- New pipe errors
- Timeouts appear

---

## Phase 8 - Confirm Forcewake, Power Wells, and Rings
**Objective**: Verify GPU power management and ring initialization

### Code Changes
- Add GuC/HuC/DMC firmware decision logs
- Add forcewake and powerwell verification before ring use
- Add ring state dump (RCS, BCS, VCS, VECS)

### Logs to Add
- Firmware load decisions
- Forcewake enable status
- Power well status
- Ring initialization state

### Test Flags
- None required

### Expected Boot Outcome
- Forcewake enabled
- Power wells on
- All rings initialized

### Failure Signal
- Forcewake failures
- Power well errors
- Ring initialization failures

### Rollback Method
- Remove logging code

### What Success Looks Like
- All rings show as initialized

### What Regression Would Look Like
- New ring errors
- Power management failures

---

## Phase 9 - Confirm GuC/HuC/DMC Handling
**Objective**: Verify firmware loading decisions and status

### Code Changes
- Add GuC/HuC/DMC firmware decision logs

### Logs to Add
- Which firmware requested
- Load status (success/skip/fail)
- GUC_STATUS if applicable

### Test Flags
- Potentially add -ngreen-dmc=skip

### Expected Boot Outcome
- Firmware decisions logged
- No firmware-related panics

### Failure Signal
- Firmware load failures
- Authentication failures

### Rollback Method
- Remove firmware logs

### What Success Looks Like
- Clear firmware decision trail

### What Regression Would Look Like
- New firmware errors

---

## Phase 10 - Attempt Minimal Acceleration Enablement
**Objective**: Move from Stage 4 to Stage 5+ (accelerator works)

### Code Changes
- Based on findings from phases 1-9
- Likely needs patch fixes or configuration changes

### Logs to Add
- Acceleration-specific events
- User client creation

### Test Flags
- Various based on findings

### Expected Boot Outcome
- IOAccelerator attaches properly
- User clients created
- Metal shows as available

### Failure Signal
- Still at Stage 4 or lower

### Rollback Method
- Revert code changes

### What Success Looks Like
- Metal renderer available

### What Regression Would Look Like
- Acceleration breaks further

---

## Phase 11 - Stabilize Acceleration
**Objective**: Fix any acceleration issues, reach Stage 9-10

### Code Changes
- Based on Phase 10 results
- Likely requires patches or workarounds

### Logs to Add
- Acceleration stability events

### Test Flags
- As needed

### Expected Boot Outcome
- Stable acceleration
- No crashes or hangs

### Failure Signal
- Acceleration unstable

### Rollback Method
- Revert changes

---

## Phase 12 - Metal Debugging (Only After IOAccelerator is Stable)
**Objective**: Ensure Metal works correctly

### Code Changes
- Only after Phase 11 complete
- Metal-specific fixes

### Logs to Add
- Metal initialization events

### Test Flags
- As needed

### Expected Boot Outcome
- Metal shows as supported in system_profiler
- No "No Metal renderer available" errors

### Failure Signal
- Metal still not working

### Rollback Method
- Revert changes

---

## Summary
This plan starts with observation (phases 1-4) before intervention (phases 5-12). Each phase adds diagnostic capability before asking the system to do more. This prevents breaking what already works while identifying what needs fixing.