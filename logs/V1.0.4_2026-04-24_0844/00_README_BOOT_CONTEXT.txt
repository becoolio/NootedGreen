# NOOTEDGREEN POST-REBOOT FORENSIC LOG COLLECTION
## Version: V1.0.4_2026-04-24_0844

### Collection Context
- **Boot Date**: Fri Apr 24 08:41:41 2026
- **Collection Date**: Fri Apr 24 08:44:00 2026
- **Uptime**: 4 minutes at collection time
- **macOS Version**: 14.8.3 (Build 23J220)
- **Kernel**: Darwin 23.6.0 (xnu-10063.141.1.708.2)
- **Hardware**: MacBookPro16,2

### Boot Arguments in Use
```
-v -liludbg liludump=60 keepsyms=1 debug=0x100 -NGreenDebug ngreenSched=5 e1000=0 IGLogLevel=8 -ngreenforceprops
```

### Debug Flags Active
- `-v` - Verbose boot
- `-liludbg` - Lilu debug mode
- `liludump=60` - Lilu dump every 60 seconds
- `keepsyms=1` - Keep symbols for panic
- `debug=0x100` - Debug flags
- `-NGreenDebug` - NootedGreen debug mode
- `ngreenSched=5` - Scheduler debug level 5
- `IGLogLevel=8` - Intel graphics log level 8
- `-ngreenforceprops` - Force property injection

### System State at Collection
- GUI reached: YES (login screen accessible)
- User logged in: YES (session active)
- WindowServer running: YES
- Display online: YES

### Purpose of This Collection
This collection is a post-reboot forensic analysis of the V1.0.4 boot session.
The system has been running for approximately 4 minutes at collection time.
All logs are from the current boot session only.