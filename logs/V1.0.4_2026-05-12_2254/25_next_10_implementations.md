# Next 10 Concrete Implementation Tasks

## 1. Add Platform-ID Injection
- **File(s) to modify**: NootedGreen source (IGPU property handling)
- **Function/class/area**: `processPatcher` / IGPU property seeding
- **Purpose**: Inject correct platform-id (currently 0x00000000)
- **Exact behavior**: Add AAPL,ig-platform-id property to IGPU for TGL
- **Logs to add**: Print platform-id value after injection
- **Expected result**: platform-id shows correct value (e.g., 0x8A987600)
- **Failure signal**: platform-id still shows 0x00000000
- **How to test after reboot**: Check dmesg for "platform-id=0x" value
- **How to rollback if fails**: Remove injection code

## 2. Add CoreDisplay DisplayPipe Error Tracer
- **File(s) to modify**: NootedGreen source (display pipe handling)
- **Function/class/area**: V111 tracing
- **Purpose**: Capture CoreDisplay DisplayPipe hash table error cause
- **Exact behavior**: Log more details about display pipe creation failure
- **Logs to add**: Dump display pipe state on error
- **Expected result**: More details on why CoreDisplay fails
- **Failure signal**: Error persists
- **How to test after reboot**: Check WindowServer logs for error details
- **How to rollback if fails**: Disable extra logging

## 3. Add IOAccelDisplayPipeUserClient2 Creation Tracer
- **File(s) to modify**: NootedGreen source (accelerator handling)
- **Function/class/area**: Accelerator personality handling
- **Purpose**: Verify user client creation
- **Exact behavior**: Log when user client is created/destroyed
- **Logs to add**: Print user client creation events
- **Expected result**: See user client creation in logs
- **Failure signal**: No creation logged
- **How to test after reboot**: Check ioreg for user client
- **How to rollback if fails**: Remove tracer

## 4. Add RCS Ring State Dump
- **File(s) to modify**: NootedGreen source (MMIO handling)
- **Function/class/area**: DisplayPipe MMIO logging
- **Purpose**: Verify render ring initialization
- **Exact behavior**: Dump RCS ring registers (HEAD, TAIL, CTL, STATUS)
- **Logs to add**: RCS ring state before/after acceleration init
- **Expected result**: Ring registers show valid values
- **Failure signal**: Ring shows stuck/zero values
- **How to test after reboot**: Check dmesg for RCS state
- **How to rollback if fails**: Remove ring dump

## 5. Add Framebuffer Property Publication Validator
- **File(s) to modify**: NootedGreen source (framebuffer personality)
- **Function/class/area**: Framebuffer personality publishing
- **Purpose**: Verify all required properties published
- **Exact behavior**: Validate framebuffer has AAPL,slot-name,model,etc.
- **Logs to add**: Property validation results
- **Expected result**: All properties validated
- **Failure signal**: Missing properties
- **How to test after reboot**: Compare ioreg vs expected
- **How to rollback if fails**: Disable validation

## 6. Add Device-ID Spoof Verification
- **File(s) to modify**: NootedGreen source (IGPU matching)
- **Function/class/area**: `processPatcher` device matching
- **Purpose**: Verify device-id spoof applied correctly
- **Exact behavior**: Check before/after spoof values
- **Logs to add**: Print original and spoofed device-id
- **Expected result**: Original shows true ID, spoof shows 0x9a49
- **Failure signal**: Values don't change
- **How to test after reboot**: Check dmesg logs
- **How to rollback if fails**: Remove spoof code

## 7. Add Link Training Status Verification
- **File(s) to modify**: NootedGreen source (AUX handling)
- **Function/class/area**: V90L4R logging
- **Purpose**: Verify link training parameters
- **Exact behavior**: Log link rate, bpp, lane count
- **Logs to add**: Already present but add verification
- **Expected result**: LinkRate=24, bpp=10, laneCount=2
- **Failure signal**: Different values
- **How to test after reboot**: Check dmesg V90L4R messages
- **How to rollback if fails**: Remove verification

## 8. Add WindowServer Display Policy Log
- **File(s) to modify**: NootedGreen source (WindowServer hooks)
- **Function/class/area**: Display policy handling
- **Purpose**: Verify display policy applied correctly
- **Exact behavior**: Log display policy decisions
- **Logs to add**: Policy enable/disable decisions
- **Expected result**: Policy enabled
- **Failure signal**: Policy disabled
- **How to test after reboot**: Check dmesg policy messages
- **How to rollback if fails**: Remove policy logging

## 9. Add Acceleration Gate Verification
- **File(s) to modify**: NootedGreen source (initialization)
- **Function/class/area**: Stage 1 initialization
- **Purpose**: Verify acceleration gate status at boot
- **Exact behavior**: Check -allow3d boot flag status
- **Logs to add**: Acceleration gate status
- **Expected result**: Acceleration gate determined correctly
- **Failure signal**: Gate wrong
- **How to test after reboot**: Check dmesg gate messages
- **How to rollback if fails**: Remove gate check

## 10. Add Build Version Verification
- **File(s) to modify**: NootedGreen source (startup logging)
- **Function/class/area**: Initial boot logging
- **Purpose**: Print Git commit/build info at boot
- **Exact behavior**: Add build info to start message
- **Logs to add**: Commit hash, build date
- **Expected result**: Version info visible
- **Failure signal**: No version info
- **How to test after reboot**: Check dmesg startup
- **How to rollback if fails**: Remove version print