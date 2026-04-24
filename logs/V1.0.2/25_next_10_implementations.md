# Implementation Tasks - V1.0.2

## Task 1: Add Boot-Stage Marker Logging
- **File(s)**: NootedGreen.cpp, XCommon.cpp
- **Purpose**: Track progress through startup
- **Current**: NootedGreen loads but doesn't print startup stage markers
- **Implement**: Add DBG_PRINT statements at each major stage
- **Test**: Look for "[NootedGreen] Stage X" in kernel logs

## Task 2: Add NootedGreen Build/Version Print at Boot
- **Files(s)**: NootedGreen.cpp
- **Purpose**: Confirm which binary is loaded
- **Implement**: Add early init message with version, build, Git commit
- **Test**: Check kernel logs immediately after Lilu loads

## Task 3: Add IGPU Match Verification
- **Files(s)**: IGPU.cpp or platform-specific file
- **Purpose**: Verify device match and property injection
- **Implement**: Dump detected device-id, platform-id, vendor-id
- **Test**: Verify spoofed values in IORegistry

## Task 4: Add AppleIntel Graphics Component Tracer
- **Files(s)**: PatcherSupport.cpp or matching code
- **Purpose**: Track if AppleIntel kexts are being matched
- **Implement**: Log when patching attempts fail/succeed
- **Test**: Check for match attempts in V1.0.2 kernel logs

## Task 5: Add Framebuffer Attach Tracer
- **Files(s)**: TGL/Framebuffer.cpp
- **Purpose**: Trace framebuffer attach to IGPU
- **Implement**: Log when framebuffer starts
- **Test**: Look for "Framebuffer started" in logs

## Task 6: Investigate com.unknown.bundle Accelerator
- **Files(s)**: TGL/Accelerator.cpp
- **Purpose**: Why is generic accelerator used instead of AppleIntel?
- **Implement**: Add logs to trace accelerator matching
- **Test**: Check IOAccelerator bundle identifier

## Task 7: Add Property Publication Validator
- **Files(s)**: Platform config files + injection code
- **Purpose**: Verify injected properties are visible
- **Implement**: Dump properties after injection
- **Test**: Compare injected vs IORegistry properties

## Task 8: Verify Metal Plugin Loading
- **Files(s)**: TGL/Accelerator.cpp (IORVARendererID)
- **Purpose**: Understand why AGXMetalA12 (not Intel Metal)
- **Implement**: Add log for Metal plugin selection
- **Test**: Check MetalPluginName in IORegistry

## Task 9: Add Display Pipe Publication Tracer
- **Files(s)**: TGL/Accelerator.cpp
- **Purpose**: Trace display pipe capability publication
- **Implement**: Log when IOAccelDisplayPipeCapabilities are published
- **Test**: Check for display pipe in IORegistry

## Task 10: Verify Forcewake/Powerwell/Ring Initialization
- **Files(s)**: TGL/Hardware.cpp or equivalent
- **Purpose**: Confirm GPU power/sync is ready
- **Implement**: Add logs for power state transitions
- **Test**: Check dmesg for forcewake messages