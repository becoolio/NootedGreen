# Next 10 Implementation Tasks - V1.0.1

## Task 1: Add Boot-Stage Marker Logging in NootedGreen
- **File(s) to modify**: NootedGreen.cpp or main plugin files
- **Function/area**: start() function, init() function
- **Purpose**: Add logging markers at each stage of NootedGreen initialization
- **Exact behavior**: Print "NootedGreen: Stage X - [description]" at each major step
- **What logs to add**: At minimum: load start, Lilu attach, IGPU detection, property injection, patch routing, completion
- **Expected result**: Visible boot-stage markers in kernel logs
- **Failure signal**: No markers appear despite NootedGreen loading
- **How to test after reboot**: Check kernel logs for NootedGreen markers
- **How to rollback if it fails**: Remove added DBGLOG statements

## Task 2: Add Version/Commit Print at Boot
- **File(s) to modify**: NootedGreen.cpp
- **Function/area**: start() function
- **Purpose**: Print exact version and Git commit at boot
- **Exact behavior**: DBGLOG("NootedGreen: Version %s, Commit %s", version, commit)
- **What logs to add**: Version string, Git commit hash
- **Expected result**: Version and commit visible in kernel logs
- **Failure signal**: No version info in logs despite kext load
- **How to test after reboot**: Search logs for "NootedGreen: Version"
- **How to rollback if it fails**: Remove version print statements

## Task 3: Add IGPU Match Verification and Property Dump
- **File(s) to modify**: NootedGreen plugin matching code
- **Function/area**: IGPU probe/match function
- **Purpose**: Log when IGPU is matched and dump all relevant properties
- **Exact behavior**: Log device-id, vendor-id, class-code, AAPL,ig-platform-id after match
- **What logs to add**: All IGPU properties after matching
- **Expected result**: Complete IGPU property dump in logs
- **Failure signal**: No IGPU properties logged
- **How to test after reboot**: Check for "IGPU matched" and property values in logs
- **How to rollback if fails**: Remove property dump code

## Task 4: Add AppleIntel Graphics Component Load Tracer
- **File(s) to modify**: NootedGreen plugin
- **Function/area**: Process matching callbacks
- **Purpose**: Trace when AppleIntel graphics kexts load and match
- **Exact behavior**: Log each AppleIntel kext load event
- **What logs to add**: AppleIntel* kext names as they load
- **Expected result**: Logs show which AppleIntel kexts loaded
- **Failure signal**: No AppleIntel kext load events in logs
- **How to test after reboot**: Check kernel logs for AppleIntel kext events
- **How to rollback if fails**: Remove tracing code

## Task 5: Add Framebuffer Attach Tracer
- **File(s) to modify**: NootedGreen framebuffer handling code
- **Function/area**: Framebuffer matching
- **Purpose**: Trace when IOFramebuffer or AppleIntelFramebuffer attaches
- **Exact behavior**: Log framebuffer attach event with properties
- **What logs to add**: Framebuffer class, index, properties
- **Expected result**: Clear framebuffer attach logs
- **Failure signal**: No framebuffer attach logs
- **How to test after reboot**: Check for framebuffer attach events in logs
- **How to rollback if fails**: Remove tracer code

## Task 6: Investigate Why AppleIntel Kexts Not Loading
- **File(s) to modify**: NootedGreen device matching code
- **Function/area**: IGPU device-id/platform-id matching logic
- **Purpose**: Determine why AppleIntel kexts reject the spoofed GPU
- **Exact behavior**: Add detailed logging of device-id/platform-id values being matched
- **What logs to add**: Exact values being used for matching, reject reasons if visible
- **Expected result**: Clear evidence of what values AppleIntel is seeing and why it rejects
- **Failure signal**: Cannot determine reject reason
- **How to test after reboot**: Check logs for device matching details
- **How to rollback if fails**: Remove debug logging

## Task 7: Add Test Mode to Skip Device Spoofing
- **File(s) to modify**: NootedGreen configuration
- **Function/area**: Boot argument handling
- **Purpose**: Allow testing with real device-id to see if AppleIntel loads
- **Exact behavior**: Check for -ngreennospoof boot arg, skip device-id spoofing if present
- **What logs to add**: "Device spoofing disabled"
- **Expected result**: AppleIntel kexts load with real device-id (or different behavior)
- **Failure signal**: Flag has no effect
- **How to test after reboot**: Set flag, check if AppleIntel kexts load
- **How to rollback if fails**: Don't set the flag

## Task 8: Add Different Platform-ID Options
- **File(s) to modify**: NootedGreen platform-id configuration
- **Function/area**: Platform ID selection
- **Purpose**: Test different platform-ids that might work with TigerLake
- **Exact behavior**: Allow selecting alternative platform-ids via boot args
- **What logs to add**: Platform-id being used
- **Expected result**: Different platform-ids can be tested easily
- **Failure signal**: AppleIntel still doesn't load
- **How to test after reboot**: Try different platform-ids
- **How to rollback if fails**: Use default platform-id

## Task 9: Add Framebuffer Property Publication Validator
- **File(s) to modify**: NootedGreen framebuffer handling
- **Function/area**: After property injection
- **Purpose**: Verify and log all published framebuffer properties
- **Exact behavior**: Enumerate and log all framebuffer properties after injection
- **What logs to add**: List of published properties
- **Expected result**: Complete property list in logs
- **Failure signal**: Missing expected properties
- **How to test after reboot**: Compare against expected properties list
- **How to rollback if fails**: Remove validation code

## Task 10: Add IOAccelerator Attach Tracer
- **File(s) to modify**: NootedGreen accelerator handling
- **Function/area**: Accelerator matching
- **Purpose**: Trace IOAccelerator attach events
- **Exact behavior**: Log when IOAcceleratorFamily2 or IOAccelerator attaches
- **What logs to add**: Accelerator class, bundle ID, properties
- **Expected result**: Accelerator attach logs visible
- **Failure signal**: No accelerator logs
- **How to test after reboot**: Check for accelerator attach events
- **How to rollback if fails**: Remove tracer code