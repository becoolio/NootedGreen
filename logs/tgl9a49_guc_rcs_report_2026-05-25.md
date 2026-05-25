# TGL 9A49 GuC/RCS Bring-up Report

## Scope

This report reviews:

- current `NootedGreen` code
- `DTK-Kexts` binaries
- `IDA_Export` artifacts
- AppleInternal tools under:
  ` /Users/becoolio/Downloads/DTK-rootfs-no_dev_no_private_no_volumes_no_templates_no_xarts_no_glue_no_borax/AppleInternal `

The goal is to answer:

1. what Apple's TGL stack expects for GuC, ring, scheduler, and display bring-up
2. whether our current code is missing important routing/policy points
3. whether Apple GuC or Linux GuC is the correct firmware direction
4. what should be implemented next to get RCS running while staying compatible with macOS / IOKit expectations

## Bottom Line

### Main conclusion

The safest and most evidence-backed way forward is:

1. keep `9A49` on `host scheduling / non-GuC` by default
2. use Apple's TGL GuC only as an explicit later experiment
3. do not use Linux GuC as the primary path for macOS bring-up
4. implement `engine preflight` and `stamp ownership` before reopening full external/display-pipe behavior

### Why

The DTK TGL accelerator binary clearly contains:

- `IGHardwareGuC`
- `IGHardwareGuCCTBuffer`
- `registerCommandTransportBuffers`
- `initDoorbells`
- `initSchedControl`
- `hostToGuCAction`
- `createUkContext`
- `submitWorkItem`
- `pauseScheduler`
- `resumeScheduler`
- `IGScheduler5`
- `IGHardwareContext::{initWithOptions, initRingGPUVirtualAddress, initRingRegisters, initRingControl, resetRingHead, updateRingTail}`
- `IGHardwareRingBuffer::{writeDWord, getStampSpace, commitStampCommand, writeStamp, submitStamp, sleepForStamp, submitToRing}`
- `IGHardwareCommandStreamer::{submitExecList, prepareExecListAndSubmit}`

This proves Apple's stack expects a very specific Apple-side scheduler and GuC interface, not a generic Linux one.

## Evidence

## 1. DTK TGL accelerator binary shows Apple-specific GuC and scheduler stack

From:

`/Users/becoolio/Documents/DTK-Kexts/AppleIntelTGLGraphics.kext/Contents/MacOS/AppleIntelTGLGraphics`

Symbol inspection shows these exported methods exist:

- `IGHardwareGuC::loadGuCBinary`
- `IGHardwareGuC::initSchedControl`
- `IGHardwareGuC::registerCommandTransportBuffers`
- `IGHardwareGuC::initDoorbells`
- `IGHardwareGuC::hostToGuCAction`
- `IGHardwareGuC::createUkContext`
- `IGHardwareGuC::submitWorkItem`
- `IGHardwareGuC::pauseScheduler`
- `IGHardwareGuC::resumeScheduler`
- `IGScheduler5::{initWithAccelerator, push, notify, checkForProgress, enableStampInterrupt, enableContextSwitchInterrupts, pause, resume}`
- `IGHardwareContext::{initWithOptions, initRingGPUVirtualAddress, initRingRegisters, initRingControl, resetRingHead, updateRingTail}`
- `IGHardwareRingBuffer::{writeDWord, getStampSpace, commitStampCommand, writeStamp, submitStamp, sleepForStamp, submitToRing, setStampIndex}`
- `IGHardwareCommandStreamer::{submitExecList, prepareExecListAndSubmit}`
- `IGAccelDisplayMachine::{init, start, probeDisplayPipes}`

This is strong evidence that Apple's bring-up model is:

1. initialize hardware context and ring state first
2. submit through Apple's ring/scheduler objects
3. optionally layer GuC transport and work queues on top
4. integrate with display management through `IGAccelDisplayMachine`

## 2. DTK binary strings show Apple has a built-in non-GuC fallback path

Important strings found in the DTK accelerator binary:

- `GraphicsSchedulerSelect`
- `SchedulerFallbackOnFirmwareFail`
- `-disablegfxfirmware`
- `[IGPU] Graphics firmware disabled via NVRAM`
- `[IGPU] Forced to use %s Scheduler`
- `[IGPU] Selected %s Scheduler`
- `[IGPU] Failed to initialize graphics firmware. Falling back to %s Scheduler`
- `IGScheduler4`
- `IGScheduler5`
- `IGGuC`

This is critical.

It means Apple already expects:

1. scheduler selection as a policy knob
2. firmware-disabled operation as a valid configuration
3. firmware failure fallback as a valid configuration

That directly supports using scheduler 5 first for `9A49`.

## 3. DTK strings show GuC is not just a firmware blob, but a full Apple transport model

Relevant DTK strings include:

- `IGHardwareGuCCTBuffer`
- `hostToGuCAction`
- `Failed to send bind-work-queue %d notice to firmware`
- `Request to send Host -> Gfx firmware message when the firmware channel is not active!`
- `Fatal firmware communication error: cannot re-establish work queue bindings.`
- `Unrecoverable firmware error: unexpected %s CSB entry.`

This strongly suggests Apple's GuC path depends on:

1. Apple-specific context/work-queue plumbing
2. Apple-specific command transport buffers
3. Apple-specific bind/unbind sequencing
4. Apple-specific expectations around CSB handling

So even if a Linux GuC blob could be loaded into hardware, that would not prove it would interoperate correctly with Apple's scheduler, work queue, and IOKit acceleration model.

## 4. IDA export of TGL framebuffer confirms startup-mode and enableController importance

From:

- `IDA_Export/AppleIntelTGLGraphicsFramebuffer/13_selected_pseudocode/0x3a6f4_AppleIntelFramebuffer_enableController(void).cpp.txt`
- `IDA_Export/AppleIntelTGLGraphicsFramebuffer/13_selected_pseudocode/0x52c2a_AppleIntelFramebufferController_enableController(AppleIntelFramebuffer_).cpp.txt`

Relevant observed details:

- `AppleIntelFramebuffer::enableController` exists and is substantial
- `AppleIntelFramebuffer::setStartupDisplayMode` exists in exports/names
- `enableController` references:
  - `AAPL0%d,IgnoreConnection`
  - `AAPL0%d,no-hotplug-interrupt`
  - `AAPL,LCD-PowerState-ON`
  - `IOFBNeedsRefresh`
- `AppleIntelFramebufferController::enableController` probes ports via `AppleIntelPort::probePortState`

This strongly supports:

1. startup-mode preservation being a real Apple path, not speculation
2. connector-specific gating being aligned with the Apple framebuffer stack
3. external port probing being tied to framebuffer enablement and not something to leave uncontrolled

## 5. AppleInternal tools reinforce port-specific AUX/DPCD/EDID debugging

From AppleInternal:

- `AppleGraphicsControl/dptool`
- `AppleGraphicsControl/dpdump`
- `AppleGraphicsControl/edidtool`
- `AppleGraphicsControl/fbtweak`
- `AppleGraphicsControl/gputail`
- `AppleGraphicsControl/gpucrashinfo`

Useful findings:

### `dptool` / `dpdump`

Strings show explicit support for:

- EDID reading
- AUX access
- DPCD parsing
- DisplayPort capability inspection
- link training related fields
- panel and power related data

This reinforces that Apple validates display links through targeted AUX/DPCD tooling.

### `fbtweak`

Its strings show:

- port targeting
- forced display mode setting

This reinforces that targeted per-port framebuffer experimentation is a normal Apple internal workflow.

### `gpucrashinfo`

Simple string extraction was not very informative from this artifact.

### `AppleIntelFramebuffer` diags

The AppleInternal tree contains:

- `AppleIntelTGLGraphicsFramebufferDiags.pax`

This strongly suggests Apple has dedicated internal framebuffer diagnostic bundles for exactly this class of problem, even though we did not unpack them here.

## GuC firmware choice

## Recommendation

Use `Apple's TGL GuC path and Apple-compatible firmware only`, and only as an opt-in experiment after host scheduling is stable.

Do `not` use Linux GuC as the main bring-up direction.

## Why Linux GuC is the wrong primary target

Linux GuC is designed for Linux i915/xe's own userspace/kernel contract.

Apple's TGL stack expects:

1. Apple scheduler classes (`IGScheduler4`, `IGScheduler5`, `IGGuC`)
2. Apple transport objects (`IGHardwareGuCCTBuffer`)
3. Apple work-queue binding logic
4. Apple firmware load path with CSS/public-key/hash workflow
5. Apple IOKit acceleration object lifecycle

The DTK binary strings and exports show that Apple's GuC path is tightly integrated with Apple's own object model.

So the real choices are not:

1. Linux GuC vs Apple GuC blob

They are:

1. host scheduler first
2. Apple GuC later if and only if the host path is stable

## What firmware should be used if we do test GuC?

The firmware should be:

1. the Apple TGL-compatible firmware path the Apple TGL driver already expects
2. loaded through the Apple driver's own loader path
3. not a Linux GuC blob arbitrarily swapped in

Your current project already has Apple-oriented GuC material:

- `NootedGreen/IGGucBinary.h`
- firmware load wrappers and related state in `kern_gen11.cpp/.hpp`

That is the correct direction if GuC is tested at all.

## How to get RCS running while staying compatible with IOKit expectations

## Recommended sequence

### Phase 1: make host scheduling stable

Default policy for spoofed `9A49`:

1. `GraphicsSchedulerSelect = 5`
2. GuC not requested by default
3. external probing gated
4. internal display only

The reason is simple: this keeps the submission path closest to Apple's own non-firmware fallback behavior.

### Phase 2: validate BCS and RCS under host scheduling

Before any GuC experiment, prove:

1. ring GPU VA is valid
2. ring register programming is valid
3. head/tail can advance
4. stamp completion works
5. macOS can still interact through IOKit / IOAccelerator objects without hanging

The DTK binary strongly suggests the right stamp-related control points are in:

- `IGHardwareRingBuffer::getStampSpace`
- `IGHardwareRingBuffer::commitStampCommand`
- `IGHardwareRingBuffer::writeStamp`
- `IGHardwareRingBuffer::submitStamp`
- `IGHardwareRingBuffer::sleepForStamp`
- `IGHardwareRingBuffer::setStampIndex`

These are better targets for ownership and bootstrap testing than anonymous shared stamp reuse.

### Phase 3: only then test Apple GuC

If BCS and RCS work under host scheduling and internal display is stable, then test Apple GuC as an explicit opt-in path.

Test criteria should be:

1. internal-only
2. no external probing
3. explicit `NGreenTryGuC`
4. explicit scheduler logging
5. command submission and CSB progress logs enabled

If GuC causes:

- firmware init failure
- CT/work-queue binding failure
- CSB mismatch
- no ring advancement

then immediately fall back to host scheduling.

## What our current code still misses

## Important missing routing / policy points

The current codebase has partial groundwork, but it is missing some important hooks for the proposed next phase.

### 1. `IGHardwareGuC::initSchedControl` exists in Apple binary, but our wrapper is not routed

In our code:

- `wrapInitSchedControl()` exists
- `orgInitSchedControl` exists

But there is no active route for it in the TGL path.

This is a gap.

If we want to safely suppress or validate GuC scheduler initialization, `initSchedControl` is an important hook point.

### 2. `IGHardwareContext::initWithOptions` is not currently hooked for policy classification

The panic stack you provided went through:

- `IGHardwareContext::initWithOptions`
- `IGHardwareExtendedContext::initWithOptions`

But our code does not currently attach policy there.

This is a major gap if we want phase-based bootstrap.

### 3. ring bootstrap/stamp functions are not being used as policy hooks

The DTK binary exposes the exact stamp/ring functions that should matter for a bootstrap smoke test:

- `getStampSpace`
- `commitStampCommand`
- `writeStamp`
- `submitStamp`
- `sleepForStamp`
- `setStampIndex`

Our current code does not hook these as a first-class bootstrap policy layer.

This is likely why stamp ownership remains weak and diagnostic waits are coarse.

### 4. `initRing*`, `submitToRing`, and exec-list submission are only conditionally routed for trace

In the TGL path, these are routed only when `isRcsEngineTraceEnabled()` is true:

- `initRingGPUVirtualAddress`
- `initRingRegisters`
- `initRingControl`
- `resetRingHead`
- `updateRingTail`
- `IGHardwareRingBuffer::init`
- `submitToRing`
- `submitExecList`
- `prepareExecListAndSubmit`

That is fine for tracing, but it means policy enforcement is still not always-on.

For real phase gating, they need to be used as policy hooks, not just trace hooks.

### 5. `AppleIntelFramebuffer::enableController` and `setStartupDisplayMode` are not actively used as the gating layer

The framebuffer binary clearly contains these functions.

Our current code has an `enableController` wrapper declaration/implementation, but it is not actually routed in the active TGL path.

That means we are still missing the cleanest Apple-aligned place to enforce:

1. startup-mode preservation
2. per-framebuffer connector gating
3. no external bring-up during early unstable acceleration

### 6. `IGAccelDisplayMachine::probeDisplayPipes` is only solved and indirectly suppressed, not fully policy-owned

We currently solve display-machine symbols and recently added a skip for `probeDisplayPipes` in the rescue path when external gating is active.

That is useful, but it is not full ownership of the native display-machine bring-up path.

So this is only a partial fix, not the final model.

## Assessment of the changes already added

The recent TGL-only safety changes are directionally correct:

1. spoofed safe scheduler policy
2. explicit `NGreenTryGuC`
3. external gate in the display-pipe rescue path

But they are only the first slice.

They do not yet implement:

1. BCS smoke test
2. RCS smoke test
3. stamp ownership partitioning
4. `initSchedControl` control
5. `enableController` / startup-mode gating at the native framebuffer layer

So they reduce risk, but they do not yet realize the full sequencing model.

## Recommended next implementation order

### Step 1

Route and control:

- `IGHardwareGuC::initSchedControl`

Goal:

1. make spoofed no-GuC policy harder to bypass
2. stop partial GuC scheduler initialization earlier than today

### Step 2

Add always-on policy routes for:

- `IGHardwareContext::initWithOptions`
- `initRingGPUVirtualAddress`
- `initRingRegisters`
- `initRingControl`
- `resetRingHead`
- `updateRingTail`
- `submitToRing`
- `submitExecList`
- `prepareExecListAndSubmit`

Goal:

1. move from trace-only to policy-enforced bootstrap

### Step 3

Implement a BCS-only bootstrap smoke path first.

Use dedicated high stamp indexes and ownership.

Only after BCS is known-good should RCS bootstrap be opened.

### Step 4

Route framebuffer-side gating in the real Apple-aligned places:

- `AppleIntelFramebuffer::enableController`
- `AppleIntelFramebuffer::setStartupDisplayMode`

Goal:

1. preserve startup mode on Port A
2. keep Port B closed until engine preflight passes

### Step 5

Only after host-scheduler RCS works:

1. test Apple GuC with `NGreenTryGuC`
2. do not test Linux GuC as the primary path

## Testing matrix recommendation

### Baseline

Boot args:

`-disablegfxfirmware`

Expectation:

1. internal-only stable framebuffer path
2. external gate active

### Host scheduler internal-only bring-up

Boot args:

`-allow3d -disablegfxfirmware -NGreenNoExternal ngreenSched=5`

Expectation:

1. no firmware scheduler path
2. no GuC init attempts except blocked/stubbed
3. BCS then RCS bootstrap logs

### Apple GuC experiment

Boot args:

`-allow3d -NGreenTryGuC -NGreenNoExternal`

Expectation:

1. explicit GuC path logging
2. no external probing
3. immediate fallback if firmware / scheduler / CT path is not fully ready

### What not to do yet

Do not use a Linux GuC blob as the default experimental path.

That may produce hardware activity but will not prove compatibility with Apple's scheduler, transport, and IOKit expectations.

## Final answers

### Did I verify against DTK-Kexts / IDA_Export / AppleInternal guidance?

Yes.

The strongest concrete guidance came from:

1. DTK `AppleIntelTGLGraphics` exported symbols and strings
2. IDA export of TGL framebuffer `enableController` and startup-mode related paths
3. AppleInternal `dptool` / `dpdump` / `fbtweak` tooling

### Which GuC should be used?

Use `Apple's TGL GuC path`, not Linux GuC, and only after host scheduling is stable.

### How do we test which path will get RCS running and still behave the way macOS expects?

Test order should be:

1. host scheduler first, internal only, no external probing
2. BCS smoke test
3. RCS smoke test
4. only then Apple GuC opt-in

That preserves the IOKit/IOAccelerator/IOFramebuffer contract while minimizing moving parts.

### What did we miss in our code so far?

Main misses still are:

1. `initSchedControl` not routed
2. `IGHardwareContext::initWithOptions` not policy-owned
3. stamp/ring bootstrap methods not policy-owned
4. exec-list/ring policy hooks only routed under trace mode
5. `enableController` / `setStartupDisplayMode` not yet used for real connector gating

## Suggested next patch sequence

1. route and gate `IGHardwareGuC::initSchedControl`
2. turn ring/context/submit hooks from trace-only into always-on policy hooks
3. implement BCS bootstrap smoke path
4. implement RCS bootstrap smoke path
5. move external gating into framebuffer `enableController` / startup-mode layer
