# Boot Context - V1.0.1

## Boot Date
2026-04-24

## Boot Time
01:23:XX (from logs)

## System State
System is currently booted and running

## NootedGreen Version
Source: 1.0.1
Loaded: 1.0.1
Match: YES (version incremented from 1.0.0)

## Git Commit
9d7d41b 1.0.1

## Boot Args
-v -liludbg liludump=60 keepsyms=1 debug=0x100 -NGreenDebug ngreenSched=5 e1000=0 IGLogLevel=8 -ngreenforceprops

## Key Changes from V1.0.0
- Version bump: 1.0.0 -> 1.0.1
- New flag: -ngreenforceprops added
- System profiler now shows "Intel Iris Xe Graphics" instead of "Unknown Unknown"
- Device spoofing appears more complete