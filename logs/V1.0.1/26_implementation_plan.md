# Implementation Plan - V1.0.1

## Phase 1 - Add Boot-Time Diagnostics (Priority: HIGH)
**Objective**: Add logging to understand NootedGreen initialization

### Code Changes
- Task 1: Add boot-stage marker logging in NootedGreen start() function
- Task 2: Add version/commit print at boot

### Logs to Add
- Stage markers: load start, Lilu attach, IGPU detection, property injection, patch routing, completion
- Version and commit hash

### Test Flags
- Use existing -NGreenDebug, -liludbg

### Expected Boot Outcome
- All stage markers visible in logs
- Version/commit printed at boot

### Failure Signal
- No markers despite kext loading

---

## Phase 2 - Verify IGPU Matching (Priority: HIGH)
**Objective**: Confirm IGPU is detected and properties injected

### Code Changes
- Task 3: Add IGPU match verification and property dump

### Logs to Add
- IGPU match confirmation
- device-id, vendor-id, class-code, AAPL,ig-platform-id

### Expected Boot Outcome
- Complete IGPU property dump showing spoofed values

---

## Phase 3 - Trace AppleIntel Kext Loading (Priority: CRITICAL)
**Objective**: Determine why AppleIntel kexts aren't loading

### Code Changes
- Task 4: Add AppleIntel graphics component load tracer
- Task 6: Add detailed device matching logging to find reject reason

### Logs to Add
- AppleIntel kext load events
- device-id/platform-id values being matched

### Expected Boot Outcome
- Clear evidence of what values AppleIntel sees and why it rejects

---

## Phase 4 - Test Different Configurations (Priority: MEDIUM)
**Objective**: Try different approaches to get AppleIntel loading

### Code Changes
- Task 7: Add -ngreennospoof flag to skip spoofing
- Task 8: Add alternative platform-id selection

### Test Flags
- -ngreennospoof (new)
- -ngreenplatform=XXXX (new)

### Expected Boot Outcome
- Can test without spoofing
- Can test different platform-ids

---

## Phase 5 - Framebuffer and Accelerator Verification (Priority: MEDIUM)
**Objective**: Verify framebuffer and accelerator attachment

### Code Changes
- Task 5: Add framebuffer attach tracer
- Task 9: Add framebuffer property publication validator
- Task 10: Add IOAccelerator attach tracer

### Logs to Add
- Framebuffer attach events
- Published properties
- Accelerator attach events

---

## Phase 6-12
Same as V1.0.0 - will only become relevant after AppleIntel kexts load

---

## Key Difference from V1.0.0
This version added -ngreenforceprops but AppleIntel kexts still don't load. The focus must be on Tasks 1-6 to get diagnostic visibility into why AppleIntel rejects the GPU.