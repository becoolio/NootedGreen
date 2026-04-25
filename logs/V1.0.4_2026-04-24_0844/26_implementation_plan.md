# Staged Implementation Plan - V1.0.4_2026-04-24_0844

## Overview
Based on the forensic analysis of V1.0.4, the system is in a GOOD STATE with display working and Metal supported. The following staged plan will help advance from Stage 7 to Stage 10 (stable acceleration).

---

## Phase 1: Confirm NootedGreen Load, Version, and Patch Routing
**Goal**: Ensure NootedGreen loads correctly and publishes version/build info

### Code Changes
- Add version print statement in `NootedGreen::init()`
- Add boot-stage marker DBGLOG statements throughout startup

### Logs to Add
```
[NootedGreen] Version: 1.0.4 starting
[NootedGreen] Stage 1: Lilu callback registered
[NootedGreen] Stage 2: IGPU device detected
[NootedGreen] Stage 3: Properties injected
[NootedGreen] Stage 4: Framebuffer callbacks registered
[NootedGreen] init complete
```

### Test Flags
- `-liludbg` (already active)
- `-NGreenDebug` (already active)

### Expected Boot Outcome
Visible version and stage markers in kernel log

### Failure Signal
No markers appear despite kext loading

### Rollback Method
Remove DBGLOG statements, rebuild

### Success Looks Like
NootedGreen version printed at boot, stage markers visible

### Regression Would Look Like
Panic on boot, NootedGreen not loading, version mismatch

---

## Phase 2: Confirm IGPU Matching and IORegistry Properties
**Goal**: Verify device-id spoofing and property injection are working

### Code Changes
- Add device validation in IGPU detection code
- Add property dump logging

### Logs to Add
```
[NootedGreen] IGPU device-id: 0x9a49
[NootedGreen] IGPU vendor-id: 0x8086
[NootedGreen] Injecting AAPL,ig-platform-id: 0x12345678
[NootedGreen] Property injection complete
```

### Test Flags
No additional flags needed

### Expected Boot Outcome
Device properties visible and validated

### Failure Signal
Device ID mismatch, properties not injected

### Rollback Method
Remove dump statements

### Success Looks Like
Correct device ID and all properties visible in IORegistry

### Regression Would Look Like
IGPU not appearing, wrong device ID

---

## Phase 3: Confirm AppleIntel Graphics Component Matching
**Goal**: Track which AppleIntel components are processed

### Code Changes
- Add kext processing tracer in `Plugin::processKext()`

### Logs to Add
```
[NootedGreen] Processing: AppleIntelTGLGraphics
[NootedGreen] Match: YES for AppleIntelTGLGraphics
[NootedGreen] Processing: AppleIntelTGLGraphicsFramebuffer
[NootedGreen] Match: YES for AppleIntelTGLGraphicsFramebuffer
```

### Test Flags
No additional flags needed

### Expected Boot Outcome
Visible component matching logs

### Failure Signal
Component not being processed or match failing

### Rollback Method
Remove tracer statements

### Success Looks Like
All AppleIntel components logged as matched

### Regression Would Look Like
Components not matching or not being processed

---

## Phase 4: Confirm Framebuffer Attach and Display State
**Goal**: Verify framebuffer services are published

### Code Changes
- Add framebuffer callback tracer
- Add display state logging

### Logs to Add
```
[NootedGreen] Framebuffer callback: AppleIntelFramebufferController
[NootedGreen] Framebuffer services published: 3 framebuffers
[NootedGreen] Display 0: AAPL,boot-display = YES
[NootedGreen] Display online: YES
```

### Test Flags
No additional flags needed

### Expected Boot Outcome
Framebuffers attached and display online

### Failure Signal
No attachment logs, display offline

### Rollback Method
Remove tracer statements

### Success Looks Like
All 3 framebuffer instances attached, display online

### Regression Would Look Like
Framebuffer attach failure, display offline

---

## Phase 5: Confirm CoreDisplay and WindowServer Behavior
**Goal**: Verify display services complete initialization

### Code Changes
- Add display transaction correlation logs

### Logs to Add
```
[NootedGreen] Display transaction started
[NootedGreen] Display transaction completed
[NootedGreen] WindowServer ready
```

### Test Flags
No additional flags needed

### Expected Boot Outcome
Display transaction completes, WindowServer starts

### Failure Signal
Transaction timeout, WindowServer not starting

### Rollback Method
Remove timing logs

### Success Looks Like
Transaction completes without timeout, GUI active

### Regression Would Look Like
Watchdog timeout, GUI hang

---

## Phase 6: Confirm IOAccelerator Service Publication
**Goal**: Verify accelerator services attach

### Code Changes
- Add accelerator attach tracer

### Logs to Add
```
[NootedGreen] IOAcceleratorFamily2 attaching
[NootedGreen] IntelAccelerator match: YES
[NootedGreen] Accelerator services published
```

### Test Flags
No additional flags needed

### Expected Boot Outcome
IOAccelerator services present in IORegistry

### Failure Signal
No accelerator services, IntelAccelerator count = 0

### Rollback Method
Remove tracer statements

### Success Looks Like
IOAcceleratorFamily2 and accelerator services attached

### Regression Would Look Like
Accelerator not matching

---

## Phase 7: Confirm Display Pipe Behavior
**Goal**: Verify display pipes are created successfully

### Code Changes
- Add display pipe creation logging
- Add AAPL,DisplayPipe verification

### Logs to Add
```
[NootedGreen] Display pipe 0: enabled
[NootedGreen] Display pipe 1: enabled
[NootedGreen] Display pipe 2: enabled
[NootedGreen] All display pipes created
```

### Test Flags
`ngreenSched=5` (already active)

### Expected Boot Outcome
All display pipes enabled (AAPL,DisplayPipe = 0xFFFF)

### Failure Signal
Display pipe creation fails, pipe count mismatch

### Rollback Method
Remove pipe logging

### Success Looks Like
All 3 pipes enabled, no timeouts

### Regression Would Look Like
Single pipe only, timeout on creation

---

## Phase 8: Confirm Forcewake, Power Wells, and Rings
**Goal**: Verify GPU power and execution hardware is ready

### Code Changes
- Add power state verification logging
- Add ring state logging

### Logs to Add
```
[NootedGreen] Forcewake domain 0: active
[NootedGreen] Power well 0: on
[NootedGreen] RCS: initialized
[NootedGreen] BCS: initialized
```

### Test Flags
No additional flags needed

### Expected Boot Outcome
Power wells and rings initialized

### Failure Signal
Power well off, ring not responding

### Rollback Method
Remove power logging

### Success Looks Like
All power domains and rings ready

### Regression Would Look Like
Power failure, ring timeout

---

## Phase 9: Confirm GuC/HuC/DMC Handling
**Goal**: Verify firmware handling decisions

### Code Changes
- Add firmware decision logging

### Logs to Add
```
[NootedGreen] GuC: skipped (not required for Gen11)
[NootedGreen] HuC: skipped (not required)
[NootedGreen] DMC: loaded (required for Gen11)
```

### Test Flags
No additional flags needed

### Expected Boot Outcome
Firmware handling documented

### Failure Signal
Firmware loading failure, authentication error

### Rollback Method
Remove firmware logging

### Success Looks Like
Firmware state clearly logged

### Regression Would Look Like
Firmware errors in log

---

## Phase 10: Attempt Minimal Acceleration Enablement
**Goal**: Verify Metal rendering works

### Code Changes
- Add Metal initialization verification

### Logs to Add
```
[NootedGreen] Metal plugin: AppleIntelTGLGraphics
[NootedGreen] Metal: initialized
[NootedGreen] GPU family: Intel Gen11
```

### Test Flags
No additional flags needed

### Expected Boot Outcome
Metal device available for rendering

### Failure Signal
Metal not available, shader compilation failure

### Rollback Method
Remove Metal logging

### Success Looks Like
Metal working for rendering

### Regression Would Look Like
Metal not supported, fallback to software rendering

---

## Phase 11: Stabilize Acceleration
**Goal**: Resolve any remaining acceleration issues

### Code Changes
Based on Phase 10 findings

### Test Flags
Based on Phase 10 findings

### Expected Boot Outcome
Stable acceleration with Metal

### Failure Signal
Rendering artifacts, GPU hangs

### Rollback Method
Revert to previous working state

### Success Looks Like
No rendering issues, stable GPU utilization

### Regression Would Look Like
New artifacts introduced

---

## Phase 12: Metal Debugging (Only after Phase 11)
**Goal**: Optimize Metal performance

### Code Changes
Based on testing results

### Test Flags
Based on testing needs

### Expected Boot Outcome
Optimal Metal performance

### Failure Signal
Performance issues

### Rollback Method
Revert optimizations

### Success Looks Like
Smooth graphics, no artifacts

### Regression Would Look Like
Performance regression

---

## Summary
The current V1.0.4 boot represents a GOOD WORKING STATE with display functional and Metal supported. The staged implementation plan will:
1. First add comprehensive logging to track behavior
2. Verify each stage is working correctly
3. Only advance to acceleration testing when previous stages are confirmed
4. Allow easy rollback at any stage

**Current Recommendation**: Proceed with Phases 1-3 to add logging, then Phase 6-7 to verify accelerator and display pipe services before attempting full acceleration testing.