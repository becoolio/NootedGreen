# Implementation Plan - V1.0.4_2026-04-28_2215

## Overview
The current boot shows significant improvement - display is online and Metal is supported. The system reaches desktop successfully. The implementation plan focuses on adding better diagnostic capabilities and test options for further debugging.

---

## Phase 1 - Confirm NootedGreen Load, Version, and Patch Routing
**Goal:** Verify NootedGreen is loading correctly with proper version and applying patches

- **Code changes:**
  - Add boot-stage marker logging (Task 1)
  - Add version/commit print at boot (Task 2)
  
- **Logs to add:**
  - Stage-by-stage initialization markers
  - Version string with Git commit
  
- **Test flags:** Already have -liludbg, -NGreenDebug enabled
  
- **Expected boot outcome:** Detailed initialization sequence visible in logs
  
- **Failure signal:** Missing stages, stale version, or patch failures
  
- **Rollback method:** Remove added logging code
  
- **What success looks like:** Sequential stage markers from start to attach
  
- **What regression would look like:** Stages missing, version mismatch

---

## Phase 2 - Confirm IGPU Matching and IORegistry Properties  
**Goal:** Verify IGPU is detected and matched correctly

- **Code changes:**
  - Add IGPU match verification and property dump (Task 3)
  
- **Logs to add:**
  - Device-id, vendor-id, platform-id verification
  
- **Test flags:** None needed
  
- **Expected boot outcome:** Complete IGPU property dump visible
  
- **Failure signal:** Missing properties, wrong device-id
  
- **Rollback method:** Remove property dump code
  
- **What success looks like:** All expected properties logged
  
- **What regression would look like:** Property mismatch

---

## Phase 3 - Confirm AppleIntel Graphics Component Matching
**Goal:** Track which AppleIntel graphics kexts load and match

- **Code changes:**
  - Add AppleIntel kext load/match tracer (Task 4)
  
- **Logs to add:**
  - Per-kext load and match status
  
- **Test flags:** None needed
  
- **Expected boot outcome:** All AppleIntel graphics kext loads traced
  
- **Failure signal:** Kexts loading but not matching
  
- **Rollback method:** Remove tracer code
  
- **What success looks like:** All relevant kext loads logged
  
- **What regression would look like:** Silent kext failures

---

## Phase 4 - Confirm Framebuffer Attach and Display State
**Goal:** Track framebuffer initialization

- **Code changes:**
  - Add framebuffer attach tracer (Task 5)
  - Add property publication validator (Task 6)
  
- **Logs to add:**
  - Framebuffer lifecycle events
  - Property publication confirmation
  
- **Test flags:** None needed
  
- **Expected boot outcome:** Complete framebuffer sequence visible
  
- **Failure signal:** Missing attach steps
  
- **Rollback method:** Remove logging
  
- **What success looks like:** All framebuffer stages logged
  
- **What regression would look like:** Framebuffer failures

---

## Phase 5 - Confirm CoreDisplay and WindowServer Behavior
**Goal:** Already confirmed working - current boot reaches desktop

- **Code changes:** None needed
  
- **Logs to add:** None needed currently
  
- **Test flags:** None needed
  
- **Expected boot outcome:** Desktop reached
  
- **Failure signal:** CoreDisplay or WindowServer errors
  
- **Rollback method:** N/A
  
- **What success looks like:** Display online, desktop reachable
  
- **What regression would look like:** Return to previous hang state

---

## Phase 6 - Confirm IOAccelerator Service Publication
**Goal:** Track accelerator service attachment

- **Code changes:**
  - Add IOAccelerator attach tracer (Task 10)
  
- **Logs to add:**
  - Accelerator start and user client creation
  
- **Test flags:** None needed
  
- **Expected boot outcome:** Accelerator lifecycle visible
  
- **Failure signal:** Service doesn't attach
  
- **Rollback method:** Remove tracer
  
- **What success looks like:** All accelerator stages logged
  
- **What regression would look like:** Missing accelerator events

---

## Phase 7 - Confirm Display Pipe Behavior
**Goal:** Add test options for display pipe debugging

- **Code changes:**
  - Add display pipe capability publication tracer (Task 7)
  - Add test flag to suppress display pipe (Task 8)
  - Add test flag for single pipe mode (Task 9)
  
- **Logs to add:**
  - Display pipe publication events
  
- **Test flags:**
  - -ngreenNoDisplayPipe (new)
  - -ngreenSinglePipe (new)
  
- **Expected boot outcome:** Can toggle display pipe with flags
  
- **Failure signal:** Flags ignored
  
- **Rollback method:** Remove flag handling
  
- **What success looks like:** Flag controls pipe behavior
  
- **What regression would look like:** Flags don't work

---

## Phase 8-12: (Deferred)
These phases focus on forcewake, power wells, rings, GuC/HuC/DMC, and Metal debugging. Since the current boot shows Metal working and display stable, these can be deferred unless issues emerge.

---

## Recommended Next Steps Implementation Order

1. **Immediate:** Task 1 (boot-stage markers) - helps with any future debugging
2. **Immediate:** Task 2 (version print) - confirms build correctness  
3. **Next:** Task 3 (IGPU verification) - ensures proper device detection
4. **Next:** Task 4 (kext tracer) - tracks component loading
5. **Optional:** Task 8-9 (test flags) - useful for debugging edge cases

The current boot is stable and functional. The primary value is adding diagnostic capability for any future issues.