# NootedGreen

Lilu plugin for Intel iGPU acceleration on macOS — Haswell through Raptor Lake, via Tiger Lake driver spoofing.

## What it does

Patches Apple's Tiger Lake (Gen12) graphics drivers to work with newer Intel iGPUs. Handles device-id spoofing, MMIO addressing, ForceWake, GPU topology, display controller init, combo PHY calibration, and GT workarounds.

## Status

**Work in progress.** Framebuffer controller starts, combo PHY calibration patched, accelerator ring initialises. Display pipeline under active development.

## Requirements

- [Lilu](https://github.com/acidanthera/Lilu) 1.7.2+
- macOS Sonoma 14.x (Gen11/Gen12 targets) or macOS 10.12–14.x (legacy NootedBlue targets)
- Supported Intel iGPU (see **Compatibility** below)
- Discrete GPU disabled via `disable-gpu` on its PCI path (if present)

## Boot args

```
-v keepsyms=1 debug=0x100 IGLogLevel=8 -wegdbg -NGreenDebug -liludbg liludump=60 -nbdyldoff ngreen-dmc=skip -allow3d -disablegfxfirmware
```

| Arg | Purpose |
|---|---|
| `-NGreenDebug` | Enable NootedGreen debug logging |
| `-disablegfxfirmware` | Disable GuC/HuC firmware loading |
| `-nbdyldoff` | Disable DYLD patches |
| `ngreen-dmc=skip` | Skip DMC firmware |
| `-allow3d` | Force 3D acceleration |
| `-nbwegcoex` | Enable WEG coexistence mode (run alongside WhateverGreen, skips overlapping routes) |
| `IGLogLevel=8` | Maximum Intel GPU driver logging |
| `-wegdbg` | Enable WhateverGreen debug logging |
| `-liludbg` | Enable Lilu debug logging |
| `liludump=60` | Dump Lilu logs after 60 seconds |

## Compatibility

NootedBlue legacy support (fully preserved):

- **Haswell** (10.12+)
- **Broadwell / Braswell** (10.14+)
- **Gemini Lake** (10.14+)

NootedGreen (Gen11/Gen12 — TGL driver spoofing):

- **Tiger Lake** (macOS Sonoma 14.x) — reported working
- **Raptor Lake-P** (macOS Sonoma 14.x) — work in progress
- **Ice Lake** — untested, may work
- **Rocket Lake** — untested, may work
- **Alder Lake** — untested, may work

## Building

Open `NootedGreen.xcodeproj` and select the **NootedGreen** scheme to build the Gen11/Gen12 plugin, or one of the original NootedBlue schemes for legacy hardware. Build with Xcode.

## Authors

- **Stefano Giammori** ([@sgiammori](https://github.com/sgiammori)) — reverse engineering, driver development, hardware testing
- **Claude Sonnet** (Anthropic) — AI pair-programming, code generation, debug analysis
- **Claude Opus 4.6** via GitHub Copilot — AI pair-programming, code generation, debug analysis

## License

[Thou Shalt Not Profit License 1.0](LICENSE)
