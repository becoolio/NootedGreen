# Boot Context for V1.0.4_2026-04-28_2215

This is a post-reboot forensic log collection for the current boot session.

## Boot Context
- macOS Version: 14.8.3 (Sonoma)
- Build: 23J220
- Kernel: Darwin 23.6.0
- Model: MacBookPro16,2
- Boot Time: Tue Apr 28 21:49:48 2026
- Current Time: Tue Apr 28 22:15:44 2026
- Uptime: 26 mins

## Boot Args
-v -liludbg liludump=60 keepsyms=1 debug=0x100 e1000=0 IGLogLevel=0xe -NGreenDebug ngreenSched=3 -ngreenforceprops

## Key Findings
- System booted successfully to desktop
- Display is online with 1920x1080 resolution
- Metal is supported according to system_profiler
- No panic observed in this boot
- Previous boot showed panic info in NVRAM (AAPL,PanicInfo)