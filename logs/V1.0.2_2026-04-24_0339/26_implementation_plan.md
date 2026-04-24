# Implementation Plan

## Phase 1 - Confirm NootedGreen load, version, and patch routing
**Goal**: Verify NootedGreen is fully operational

### Code Changes
- Add boot-stage marker logging throughout kern_green.cpp
- Add version print at startup
- Add function routing verification

### Logs to Add
- "NootedGreen: Starting driver"
- "NootedGreen: Version X.Y.Z loaded"
- "NootedGreen: Routing function [name]"

### Test Flags
- -NGreenDebug (already active)
- -liludbg (already active)

### Expected Boot Outcome
- Detailed logs visible in kernel log output
- Version confirmation in logs

### Failure Signal
- No NootedGreen logs despite boot flags
- Version not visible

### Rollback Method
- Revert changes to kern_green.cpp

### Success Criteria
- NootedGreen initialization sequence fully visible in logs

### Regression Detection
- If logs stop appearing, initialization regressed

---

## Phase 2 - Confirm IGPU matching and IORegistry properties
**Goal**: Verify device spoofing works correctly

### Code Changes
- Add IGPU property dump before/after spoofing
- Add device-id/platform-id validation

### Logs to Add
- "IGPU: Original device-id=0xXXXX"
- "IGPU: Spoofed to 0xYYYY"
- "IGPU: Platform-id=0xZZZZ"

### Test Flags
- Continue with -NGreenDebug

### Expected Boot Outcome
- Clear visibility into property modifications

### Failure Signal
- Properties not appearing in IORegistry

### Rollback Method
- Remove dump code

### Success Criteria
- device-id 0x9a49 confirmed in logs and IORegistry

### Regression Detection
- Mismatch between logged and actual IORegistry values

---

## Phase 3 - Confirm AppleIntel graphics component matching
**Goal**: Track which AppleIntel kexts are being patched

### Code Changes
- Add kext processing tracer
- Log each kext NootedGreen processes

### Logs to Add
- "NootedGreen: Processing [kext name]"
- "NootedGreen: Patch applied to [function]"

### Test Flags
- Continue with -NGreenDebug

### Expected Boot Outcome
- Visible patching activity in boot logs

### Failure Signal
- No patching logs despite AppleIntel kexts loading

### Rollback Method
- Remove tracer code

### Success Criteria
- All relevant AppleIntel kexts logged as processed

### Regression Detection
- AppleIntel kexts load but no patching logs

---

## Phase 4 - Confirm framebuffer attach and display state
**Goal**: Verify framebuffer publishes correctly

### Code Changes
- Add framebuffer attach tracer
- Add display properties publication logging

### Logs to Add
- "Framebuffer: Personality published"
- "Framebuffer: Display modes published"

### Test Flags
- Continue with -NGreenDebug

### Expected Boot Outcome
- Framebuffer attach confirmation

### Failure Signal
- No attach confirmation

### Rollback Method
- Remove attach tracer

### Success Criteria
- IOFramebuffer visible in IORegistry

### Regression Detection
- Framebuffer disappears from IORegistry

---

## Phase 5 - Confirm CoreDisplay and WindowServer behavior
**Goal**: Track display manager initialization

### Code Changes
- Add CoreDisplay transaction logging (if possible via hooking)
- Track WindowServer start

### Logs to Add
- "CoreDisplay: Transaction started"
- "WindowServer: Started"

### Test Flags
- Already tracking via system state

### Expected Boot Outcome
- WindowServer reaches login screen

### Failure Signal
- WindowServer crashes

### Rollback Method
- N/A (observational only)

### Success Criteria
- GUI reaches login screen

### Regression Detection
- Hang at blue progress bar

---

## Phase 6 - Confirm IOAccelerator service publication
**Goal**: Verify accelerator binds to correct service

### Code Changes
- Add IOAccelerator attach tracer
- Log which accelerator bundle is used

### Logs to Add
- "Accelerator: Service published"
- "Accelerator: Using bundle [bundle-id]"

### Test Flags
- Continue with -NGreenDebug

### Expected Boot Outcome
- IOAccelerator service visible in IORegistry

### Failure Signal
- Generic accelerator binds instead of Intel

### Rollback Method
- Remove tracer

### Success Criteria
- IntelAccelerator service visible

### Regression Detection
- com.unknown.bundle appears instead

---

## Phase 7 - Confirm display pipe behavior
**Goal**: Isolate display pipe issues

### Code Changes
- Add -ngreenNoDisplayPipe flag handling
- Add -ngreenSinglePipe flag handling

### Logs to Add
- "Display pipe: Suppressed by flag"
- "Display pipe: Single pipe mode"

### Test Flags
- -ngreenNoDisplayPipe (new)
- -ngreenSinglePipe (new)

### Expected Boot Outcome
- May fix hangs if display pipe is problematic

### Failure Signal
- Flag has no effect

### Rollback Method
- Remove flag handling code

### Success Criteria
- Boot completes with or without display pipe

### Regression Detection
- New flags cause boot failure

---

## Phase 8 - Confirm forcewake, power wells, and rings
**Goal**: Verify GPU hardware initialization

### Code Changes
- Add power state verification code
- Add ring initialization logging

### Logs to Add
- "HW: Forcewake=0xXXXX"
- "HW: Power wells=0xYYYY"
- "HW: RCS ring initialized"

### Test Flags
- Continue with -NGreenDebug

### Expected Boot Outcome
- Power state visible in logs

### Failure Signal
- Power not ready

### Rollback Method
- Remove power check code

### Success Criteria
- All power domains report ready

### Regression Detection
- Power hangs appear

---

## Phase 9 - Confirm GuC/HuC/DMC handling
**Goal**: Verify firmware loading

### Code Changes
- Add firmware decision logging
- Track which firmware is loaded/skipped

### Logs to Add
- "Firmware: GuC [loaded/skipped/error]"
- "Firmware: DMC [loaded/skipped/error]"

### Test Flags
- Continue with -NGreenDebug

### Expected Boot Outcome
- Firmware status visible

### Failure Signal
- Firmware errors

### Rollback Method
- Remove firmware logging

### Success Criteria
- Firmware handling complete

### Regression Detection
- New firmware errors appear

---

## Phase 10 - Attempt minimal acceleration enablement
**Goal**: Achieve Stage 6+ acceleration

### Based on findings from Phases 1-9
- If IOAccelerator binds generically: Investigate Info.plist personality
- If display pipe causes hang: Test -ngreenNoDisplayPipe
- If rings fail: Check power initialization order
- If Metal fails: Verify plugin loading

### Test Flags
- Based on Phase 7-9 findings

### Expected Boot Outcome
- IOAccelDisplayPipeUserClient2 visible
- Metal showing Intel-specific support

### Failure Signal
- Still using generic acceleration

### Rollback Method
- Revert to previous working state

### Success Criteria
- Stage 6+ acceleration achieved

---

## Phase 11 - Stabilize acceleration
**Goal**: Fix any remaining acceleration issues

### Based on Phase 10 results
- Fix any identified blockers
- Iterate on firmware/power/ring fixes

### Test Flags
- Refine based on findings

### Expected Boot Outcome
- Stable acceleration (Stage 9-10)

---

## Phase 12 - Only after IOAccelerator is sane, continue Metal debugging
**Goal**: Finalize Metal support

### Prerequisites
- IOAccelerator binding correctly
- Display pipe stable
- Rings functional

### Test Flags
- Full debug flags

### Expected Boot Outcome
- Metal shows Intel-specific support (not AGXMetalA12)
- Full GPU acceleration working