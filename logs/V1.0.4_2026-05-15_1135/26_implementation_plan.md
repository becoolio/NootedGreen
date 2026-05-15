# Implementation Plan - Staged Approach

## Phase 1: Confirm NootedGreen Load, Version, and Patch Routing
### Code Changes
- Add version print in pluginStart()
- Add stage markers in init() method
- Add routing confirmation log

### Logs to Add
- "[NootedGreen] Version: X.Y.Z"
- "[NootedGreen] Stage X starting/complete"
- "[NootedGreen] Routes applied: X/Y"

### Test Flags
- Keep existing debug flags
- Ensure -liludbgall present

### Expected Boot Outcome
- Version visible in kernel logs
- All 5 stages complete
- All routes applied

### Failure Signal
- Version not visible
- Stage incomplete
- Routes missing

### Rollback
- Remove version print and stage markers

### Success Criteria
- NootedGreen v1.0.4 visible
- All stages logged
- Routes confirmed

### Regression Signal
- Boot slower
- Too much log spam
- Version wrong

---

## Phase 2: Confirm IGPU Matching and IORegistry Properties
### Code Changes
- Add IGPU property dump after matching
- Log device-id, platform-id, AAPL,ig-platform-id

### Logs to Add
- "[NootedGreen] IGPU device-id=0xXXXX"
- "[NootedGreen] IGPU platform-id=0xXXXX"
- "[NootedGreen] IGPU model=XXXX"

### Test Flags
- Keep existing debug flags

### Expected Boot Outcome
- IGPU matched correctly
- All properties visible in logs
- device-id=0x9a49 confirmed

### Failure Signal
- Wrong device-id
- Wrong platform-id
- Missing properties

### Rollback
- Remove property dump

### Success Criteria
- Correct IGPU properties logged
- Matches expected values

### Regression Signal
- Matching broken
- Wrong device detected

---

## Phase 3: Confirm AppleIntel Graphics Component Matching
### Code Changes
- Add processKext() tracer for AppleIntel bundles
- Log each bundle processed and matched status

### Logs to Add
- "[NootedGreen] processKext: AppleIntelTGLGraphics"
- "[NootedGreen] AppleIntelTGLGraphics matched=YES"

### Test Flags
- Keep existing debug flags

### Expected Boot Outcome
- All AppleIntel kexts logged
- Matching confirmed

### Failure Signal
- Missing kext log
- Matching failed

### Rollback
- Remove tracer

### Success Criteria
- All expected kexts logged
- Matching confirmed

### Regression Signal
- Kexts not loaded
- Matching fails

---

## Phase 4: Confirm Framebuffer Attach and Display State
### Code Changes
- Add framebuffer controller startup tracer
- Add property publication validator

### Logs to Add
- "[NootedGreen] FramebufferController started"
- "[NootedGreen] Property: XXXX published"

### Test Flags
- Keep existing debug flags

### Expected Boot Outcome
- Framebuffer attached
- Properties published
- Display online

### Failure Signal
- No framebuffer log
- Properties missing

### Rollback
- Remove tracers

### Success Criteria
- Framebuffer startup logged
- Properties validated

### Regression Signal
- Framebuffer fails to attach
- Display offline

---

## Phase 5: Confirm CoreDisplay and WindowServer Behavior
### Code Changes
- Add display pipe publication tracer
- Add transaction readiness log

### Logs to Add
- "[NootedGreen] DisplayPipe capabilities published"
- "[NootedGreen] Display ready for WindowServer"

### Test Flags
- Keep existing debug flags

### Expected Boot Outcome
- Display pipe services appear
- WindowServer starts
- IOAccelerationUserClient created

### Failure Signal
- Display pipe not logged
- WindowServer delayed

### Rollback
- Remove tracer

### Success Criteria
- Display pipe logged
- WindowServer connected

### Regression Signal
- WindowServer stuck
- Display pipe timeout

---

## Phase 6: Confirm IOAccelerator Service Publication
### Code Changes
- Add IOAccelDisplayPipeUserClient2 creation tracer
- Log accelerator service state

### Logs to Add
- "[NootedGreen] IOAccelDisplayPipeUserClient2 created"
- "[NootedGreen] Accelerator services ready"

### Test Flags
- Keep existing debug flags

### Expected Boot Outcome
- Accelerator user client created
- Services ready

### Failure Signal
- No user client log
- Services not ready

### Rollback
- Remove tracer

### Success Criteria
- User client logged
- Accelerator functional

### Regression Signal
- Accelerator fails
- No user client

---

## Phase 7: Confirm Display Pipe Behavior
### Code Changes
- Add display pipe creation success/failure log
- Log any errors during creation

### Logs to Add
- "[NootedGreen] DisplayPipe created successfully"
- "[NootedGreen] DisplayPipe error: XXXX"

### Test Flags
- Test with -ngreenSinglePipe flag (after implementation)

### Expected Boot Outcome
- Display pipe created
- No fatal errors
- System continues

### Failure Signal
- Display pipe creation fails
- System hangs

### Rollback
- Add -ngreenNoDisplayPipe flag for fallback

### Success Criteria
- Display pipe succeeds
- No hang

### Regression Signal
- Display pipe timeout
- System freeze

---

## Phase 8: Confirm Forcewake, Power Wells, and Rings
### Code Changes
- Add forcewake/power well state dump
- Add ring initialization log

### Logs to Add
- "[NootedGreen] Forcewake: XXXX"
- "[NootedGreen] PowerWell: XXXX"
- "[NootedGreen] RCS/VCS/VECS initialized"

### Test Flags
- Keep existing flags (no -disablegfxfirmware currently)

### Expected Boot Outcome
- Forcewake enabled
- Power wells active
- Rings initialized

### Failure Signal
- Forcewake stuck
- Power wells failed

### Rollback
- Add -disablegfxfirmware if GuC issues

### Success Criteria
- All subsystems initialized
- No errors

### Regression Signal
- Forcewake timeout
- Ring hang

---

## Phase 9: Confirm GuC/HuC/DMC Handling
### Code Changes
- Add firmware decision logs
- Add GuC/HuC/DMC status logs

### Logs to Add
- "[NootedGreen] GuC: SKIP/LOAD/ERROR"
- "[NootedGreen] HuC: SKIP/LOAD/ERROR"
- "[NootedGreen] DMC: LOADED/ERROR"

### Test Flags
- Current: no -disablegfxfirmware
- Test: add -disablegfxfirmware if issues

### Expected Boot Outcome
- DMC loaded
- GuC/HuC status clear
- No firmware panics

### Failure Signal
- GuC fails to load
- DMC fails to load

### Rollback
- Add -disablegfxfirmware to skip GuC/HuC

### Success Criteria
- Firmware decisions clear
- System stable

### Regression Signal
- Firmware timeout
- GPU hang

---

## Phase 10: Attempt Minimal Acceleration Enablement
### Code Changes
- Ensure all Phase 1-9 checks pass
- No new code changes needed for basic acceleration

### Test Flags
- Add -allow3d to test full acceleration
- Keep verbose flags for debugging

### Expected Boot Outcome
- Metal accelerated
- 3D applications work
- No artifacts

### Failure Signal
- Metal crashes
- 3D not working
- Artifacts visible

### Rollback
- Remove -allow3d if issues
- Revert to -igfxvesa fallback

### Success Criteria
- Metal works
- 3D applications functional

### Regression Signal
- Artifacts
- Crash on 3D load

---

## Phase 11: Stabilize Acceleration
### Code Changes
- Based on Phase 10 results
- Fix any acceleration issues found

### Test Flags
- Standard boot (no extra flags)
- Monitor stability

### Expected Boot Outcome
- Full acceleration stable
- No crashes
- Performance acceptable

### Failure Signal
- Acceleration crashes
- Performance poor

### Rollback
- Revert to previous stable version

### Success Criteria
- 100% stable acceleration
- No crashes

### Regression Signal
- Frequent crashes
- Poor performance

---

## Phase 12: Metal Debugging (Only After Phase 11)
### Code Changes
- Advanced Metal debugging
- Only if Metal-specific issues found

### Test Flags
- Metal-specific debug flags
- Performance profiling

### Expected Boot Outcome
- Metal fully functional
- All features work

### Failure Signal
- Metal-specific bugs
- Feature gaps

### Rollback
- Use fallback renderer

### Success Criteria
- All Metal features work
- No bugs

### Regression Signal
- Metal bugs
- Feature broken