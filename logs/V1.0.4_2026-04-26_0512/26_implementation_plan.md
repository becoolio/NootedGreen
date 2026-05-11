# Implementation Plan - V1.0.4_2026-04-26_0512

## Staged Implementation Plan

This plan outlines a phased approach to debug and stabilize the NootedGreen Intel Tiger Lake graphics driver.

### Phase 1 - Confirm NootedGreen Load, Version, and Patch Routing

**Goals:**
- Verify NootedGreen loads correctly
- Verify version matches source/build
- Verify patch routing works
- Add boot-stage marker logging

**Code Changes:**
- Add stage markers to all startup phases
- Add version/Git commit print at boot
- Add patch route verification logs

**Logs to Add:**
```
NootedGreen: ===== STAGE 1: START =====
NootedGreen: ===== STAGE 2: PATCH =====
NootedGreen: ===== STAGE 3: INIT =====
NootedGreen: V1.0.4 commit b71eba3
```

**Test Flags:**
- Current: -NGreenDebug (already enabled)
- Add verbose: Already enabled

**Expected Boot Outcome:**
- System boots to GUI
- Stage markers visible in dmesg
- Version info visible in dmesg

**Failure Signal:**
- Version string not visible
- Patch fails silently
- Kernel panic during early boot

**Rollback Method:**
- Remove test code additions
- Boot with existing flags

**What Success Looks Like:**
- Clear 5-stage marker sequence in dmesg
- Version visible at boot
- All patches applied successfully

**What Regression Would Look Like:**
- Stage markers missing
- Version string wrong
- New kernel panic

---

### Phase 2 - Confirm IGPU Matching and IORegistry Properties

**Goals:**
- Verify IGPU detection works
- Verify device-id spoofing works
- Verify platform-id injection works
- Dump all relevant properties

**Code Changes:**
- Add IGPU property dump after matching
- Add device-id, vendor-id, platform-id logging from kernel

**Logs to Add:**
```
NootedGreen: IGPU matched: device-id=0x9a49 vendor-id=0x8086
NootedGreen: AAPL,ig-platform-id=0x9a49
```

**Test Flags:**
- Current boot-args sufficient
- Could add -ngreenforceprops for additional injection

**Expected Boot Outcome:**
- IGPU@2 visible in IORegistry
- device-id = 0x9a49 visible
- platform-id = 0x9a49 visible
- system_profiler shows "Intel Iris Xe Graphics"

**Failure Signal:**
- IGPU not appearing
- device-id wrong or 0
- platform-id missing
- GPU not in system_profiler

**Rollback Method:**
- Check OpenCore GPU spoofing
- Test without NootedGreen

**What Success Looks Like:**
- IGPU properly matched
- All properties correct
- GPU visible in system_profiler

**What Regression Would Look Like:**
- IGPU not matched
- Wrong device ID
- GPU missing from profiler

---

### Phase 3 - Confirm AppleIntel Graphics Component Matching

**Goals:**
- Verify AppleIntelTGLGraphicsFramebuffer loads
- Verify AppleIntelTGLGraphics accelerator loads
- Trace component loading
- Confirm both attach to IGPU

**Code Changes:**
- Add component load tracer to processKext()
- Track framebuffer and accelerator separately

**Logs to Add:**
```
NootedGreen: [FB] AppleIntelTGLGraphicsFramebuffer LOADED
NootedGreen: [HW] AppleIntelTGLGraphics LOADED
NootedGreen: [HW] AppleIntelTGLGraphics initializing
```

**Test Flags:**
- Current flags sufficient

**Expected Boot Outcome:**
- Both kexts loaded (indices 139, 163)
- Both attach to IGPU
- No load failures in logs

**Failure Signal:**
- Kext fails to load
- Load failure with error code
- Attach fails

**Rollback Method:**
- Check AppleIntel kexts in /Library/Extensions
- Rebuild SLE

**What Success Looks Like:**
- Both components loaded
- Both matched to IGPU
- No load errors

**What Regression Would Look Like:**
- Component load fails
- Only partial load
- New errors in logs

---

### Phase 4 - Confirm Framebuffer Attach and Display State

**Goals:**
- Verify framebuffer attaches
- Verify internal display detected
- Verify EDID parsed
- Verify resolution mode available

**Code Changes:**
- Add framebuffer attach tracer
- Add EDID/VBT/OpRegion parse verification

**Logs to Add:**
```
NootedGreen: Framebuffer attached
NootedGreen: Display detected: LCD
NootedGreen: EDID parsed
NootedGreen: Display mode: 1920x1080
```

**Test Flags:**
- Current flags sufficient

**Expected Boot Outcome:**
- AppleIntelFramebufferController registered
- Color LCD in system_profiler
- Online: Yes display status
- 1920x1080 resolution shown

**Failure Signal:**
- Framebuffer not appearing
- No display in system_profiler
- Offline display
- Wrong resolution

**Rollback Method:**
- Test without -NGreenDebug

**What Success Looks Like:**
- Display working
- Native resolution
- Framebuffer operational

**What Regression Would Look Like:**
- Black screen
- No display
- Wrong resolution

---

### Phase 5 - Confirm CoreDisplay and WindowServer Behavior

**Goals:**
- Verify WindowServer starts
- Verify display transaction completes
- Check for transaction timeouts
- Check for stamp errors

**Code Changes:**
- Add display transaction correlation logs
- Add stamp advancement logging

**Logs to Add:**
```
NootedGreen: Display transaction initiated
NootedGreen: Display transaction complete
NootedGreen: Stamp idx advancing
```

**Test Flags:**
- Already running -v

**Expected Boot Outcome:**
- GUI reaches login window
- WindowServer PID visible
- No transaction timeout
- Session active

**Failure Signal:**
- WindowServer crash
- Transaction timeout in logs
- Stamp error
- Blue bar hang

**Rollback Method:**
- Test with -v only
- Check ReportCrash

**What Success Looks Like:**
- Full GUI achieved
- WindowServer running
- Session logged in

**What Regression Would Look Like:**
- WindowServer crash
- Hang at blue bar
- Transaction timeout

---

### Phase 6 - Confirm IOAccelerator Service Publication

**Goals:**
- Verify IOAcceleratorFamily2 loads
- Verify IntelAccelerator matches
- Verify MetalPluginName published

**Code Changes:**
- Add accelerator attach tracer
- Add user client publish logs

**Logs to Add:**
```
NootedGreen: IOAcceleratorFamily2 loaded
NootedGreen: IntelAccelerator matched
NootedGreen: Metal: AppleIntelTGLGraphicsMTLDriver available
```

**Test Flags:**
- Current flags sufficient

**Expected Boot Outcome:**
- IOAcceleratorFamily2 loaded
- IntelAccelerator attached
- Metal visible in system_profiler
- VRAM detected

**Failure Signal:**
- IOAccelerator not loaded
- No Metal in profiler
- No VRAM reported

**Rollback Method:**
- Check kext loading order

**What Success Looks Like:**
- Full acceleration infrastructure
- Metal supported
- VRAM present

**What Regression Would Look Like:**
- No Metal
- Basic display only

---

### Phase 7 - Confirm Display Pipe Behavior

**Goals:**
- Verify display pipe created
- Check pipe configuration
- Verify no pipe-specific failures

**Code Changes:**
- Add display pipe capability publication
- Add pipe index logging

**Logs to Add:**
```
NootedGreen: DisplayPipe created: idx=0
NootedGreen: DisplayPipe cap: transactionsSupported=1
NootedGreen: DisplayPipe created: idx=1 (if dual pipe)
```

**Test Flags:**
- Consider -ngreenNoDisplayPipe flag
- Consider -ngreenSinglePipe flag

**Expected Boot Outcome:**
- Display pipe created logs present
- No pipe creation timeout
- Both pipes (if applicable) created

**Failure Signal:**
- Pipe creation hang
- Stamp timeout
- Index-specific failure

**Rollback Method:**
- Try -ngreenSinglePipe
- Try -ngreenNoDisplayPipe

**What Success Looks Like:**
- Pipe logs visible
- No timeout
- Display stable

**What Regression Would Look Like:**
- Stamp timeout
- Pipe hang
- Index 32 timeout

---

### Phase 8 - Confirm Forcewake, Power Wells, and Rings

**Goals:**
- Verify forcewake succeeds
- Verify power wells enabled
- Verify ring initialization
- Check RCS/BCS/VCS/VECS states

**Code Changes:**
- Add forcewake verification
- Add ring state dump MMIO read

**Logs to Add:**
```
NootedGreen: Forcewake: success
NootedGreen: Power well: enabled
NootedGreen: RCS ring: HEAD=0 TAIL=0 CTL=0x80800000
NootedGreen: BCS ring: initialized
```

**Test Flags:**
- Current: May need enabling

**Expected Boot Outcome:**
- Forcewake logs present
- Power well logs present
- Ring states visible

**Failure Signal:**
- Forcewake failure
- Power well errors
- Ring hang

**Rollback Method:**
- Skip forcewake via boot flag (if exists)
- Fall back to VESA

**What Success Looks Like:**
- All rings visible
- No errors on boot

**What Regression Would Look Like:**
- Forcewake errors
- Ring hangs
- Missing ring state

---

### Phase 9 - Confirm GuC/HuC/DMC Handling

**Goals:**
- Verify GuC loading decision
- Verify HuC handling decision
- Verify DMC handling decision
- Check for authentication issues

**Code Changes:**
- Add firmware decision logs (already visible)
- Add any GuC-specific error tracking

**Logs to Add (already present):**
```
NootedGreen: firmware patch[GuC.full]: wrote ... bytes
NootedGreen: firmware patch[HuC]: pattern not found
NootedGreen: GuC loaded
```

**Test Flags:**
- Current boot has -ngreengucfw

**Expected Boot Outcome:**
- GuC firmware attempt visible
- HuC skip visible
- No firmware error panics

**Failure Signal:**
- GuC authentication failure
- HuC error
- Firmware panic

**Rollback Method:**
- Boot without GuC flag
- Use alternate DMC

**What Success Looks Like:**
- Firmware decisions logged
- No panics from firmware
- Stable boot

**What Regression Would Look Like:**
- Firmware panic
- Authentication errors
- Stuck GuC

---

### Phase 10 - Attempt Minimal Acceleration Enablement

**Goals:**
- Verify Metal works with basic tests
- Verify no black screen
- Run benchmark if stable

**Code Changes:**
- May need test applications

**Test Flags:**
- -allow3d (may be needed explicitly if not present)

**Expected Boot Outcome:**
- Metal appears in system_profiler
- No GPU hang under light load
- Display stable over time

**Failure Signal:**
- GPU hang under load
- Metal errors
- Accelerated mode fail

**Rollback Method:**
- Boot with -igfxvesa
- Use software rendering

**What Success Looks Like:**
- Metal benchmarks work
- GPU stable

**What Regression Would Look Like:**
- Heavy load hangs
- GPU restarts

---

## Phase Summary

| Phase | Target | Current Status | Priority |
|-------|-------|-------------|---------|
| 1 | Load/Version | HIGH | Need markers |
| 2 | IGPU Match | HIGH | Need dump |
| 3 | Components | HIGH | Need tracer |
| 4 | Framebuffer | DONE | Working |
| 5 | WindowServer | DONE | Working |
| 6 | IOAccelerator | DONE | Working |
| 7 | Display Pipe | MEDIUM | Test flags |
| 8 | Rings/FW | LOW | Need dump |
| 9 | Firmware | PARTIAL | Working |
| 10 | Acceleration | LOW | Done if stable |