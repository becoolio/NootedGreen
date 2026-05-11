00_README_BOOT_CONTEXT.txt - V1.0.4_2026-04-26_0512

Boot Context Information
=======================

This boot analysis was performed on:
- Date: Sun Apr 26 05:09:30 EST 2026
- System: MacBookPro16,2 (Dell XPS 9300 / Intel Tiger Lake)
- macOS Version: 14.8.3 (23J220)
- Kernel: Darwin 23.6.0

Previous Shutdown Cause: 5 (normal shutdown)

Boot Arguments (from nvram):
-v -liludbg liludump=60 keepsyms=1 debug=0x100 e1000=0 IGLogLevel=8 -NGreenDebug ngreenSched=3 -ngreengucfw

NootedGreen Version: 1.0.4
Git Commit: b71eba3 GUC firmware test
Git Branch: main
Build: Debug (compiled locally)

System Uptime at Collection: 8 minutes
User Session: Active (becoolio logged in)
GUI Status: REACHED - login screen displayed

Testing Focus:
=============
- This boot tests GuC firmware loading with -ngreengucfw flag
- Lilu debug flags enabled (-liludbg liludump=60)
- NootedGreen debug enabled (-NGreenDebug)

Key Observations:
=================
1. Boot completed successfully to GUI
2. No kernel panic occurred
3. No immediate GPU hang/restart
4. WindowServer started
5. Internal display detected at 1920x1080
6. GUI reached without visible degradation