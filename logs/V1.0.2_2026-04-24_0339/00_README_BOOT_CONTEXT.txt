# Boot Context - V1.0.2_2026-04-24_0339

## Test Context
This is a post-reboot forensic analysis for NootedGreen on a TigerLake (Intel 11th gen) laptop.

## System
- Hardware: MacBookPro16,2 (Dell Laptop with TigerLake CPU)
- macOS: 14.8.3 (Sonoma)
- Kernel: 23.6.0

## Purpose
Collect boot logs and analyze graphics initialization for NootedGreen driver debugging.

## Version
NootedGreen v1.0.2 - with uncommitted local changes to kern_gen11.cpp and kern_green.cpp

## Boot Configuration
- SIP: Disabled (development configuration)
- Boot args: -v -liludbg liludump=60 keepsyms=1 debug=0x100 -NGreenDebug ngreenSched=5 e1000=0 IGLogLevel=8 -ngreenforceprops

## Previous Version
logs/V1.0.2/ - V1.0.2 report showing partial success with display working but generic acceleration