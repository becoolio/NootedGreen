# Boot Context README
# NootedGreen Post-Reboot Forensic Log Collection
# Version: V1.0.4
# Boot Date: 2026-05-15
# Time: 03:22:19 EST

## Purpose
Full post-reboot forensic log collection and analysis for the current boot session only.
This is a repeatable versioned debug report for the current boot.

## Boot Flags Used
- itlwm_cc=US -v -liludbgall liludump=80 keepsyms=1 debug=0x100 NGreenDebug ngreenSched=3 ngreen-dmc=tgl -ngreenTglTrace

## Key Differences from Previous Boot
- Previous version: V1.0.4_2026-05-12_2254
- Previous boot args: itlwm_cc=US -v keepsyms=1 debug=0x100 -NGreenDebug ngreen-dmc=tgl -disablegfxfirmware ngreenSched=5
- Current boot args: itlwm_cc=US -v -liludbgall liludump=80 keepsyms=1 debug=0x100 NGreenDebug ngreenSched=3 ngreen-dmc=tgl -ngreenTglTrace

## Changes
1. -liludbgall added (full Lilu debug)
2. liludump=80 added (80 lines dump)
3. -NGreenDebug changed to NGreenDebug (without hyphen prefix)
4. ngreenSched=5 changed to ngreenSched=3
5. -disablegfxfirmware removed (GuC/HuC may load now)
6. -ngreenTglTrace added (TGL tracing enabled)

## Expected Outcome
Collect all logs, compare with previous boot, determine improvements/regressions,
and produce 10 concrete implementation tasks for next reboot.