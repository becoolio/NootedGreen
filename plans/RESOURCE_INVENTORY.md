# Resource Inventory — DTK, IDA, and Reverse Engineering Assets

## Asset Tree Overview

```
~/Documents/
├── DTK-Kexts/                          # DTK kexts (x86_64!) — same MD5 as project sle/
│   ├── AppleIntelTGLGraphics.kext/
│   ├── AppleIntelTGLGraphicsFramebuffer.kext/
│   ├── AppleIntelTGLGraphicsMTLDriver.bundle/
│   ├── AppleIntelTGLGraphicsGLDriver.bundle/
│   ├── AppleIntelTGLGraphicsVADriver.bundle/
│   ├── AppleIntelTGLGraphicsVAME.bundle/
│   ├── AppleIntelGraphicsShared.bundle/
│   ├── IOGPUFamily.kext/               # DTK version 16.2
│   ├── IOAcceleratorFamily2.kext/
│   ├── IOGraphicsFamily.kext/
│   ├── IOSurface.kext/
│   ├── AppleGraphicsControl.kext/
│   ├── AppleGFXHDA.kext/
│   ├── AppleBacklight*.kext/
│   ├── AppleMCCSControl.kext/
│   ├── AppleDCP*.kext/                  # Apple Silicon display (not useful)
│   ├── AGXMetalA*.bundle/               # Apple Silicon GPU (not useful)
│   └── 40+ other kexts
│
├── IDA_Export/                          # Structured IDA analysis exports
│   ├── AppleIntelTGLGraphicsFramebuffer/    # ** MOST EXTENSIVE **
│   │   ├── 00_context.txt
│   │   ├── 01_functions.tsv             # 300K — all function addresses + names
│   │   ├── 02_names.tsv                 # 370K — named locations
│   │   ├── 03_strings.tsv               # 540K — all string literals
│   │   ├── 04_interesting_strings.tsv   # 210K — filtered important strings
│   │   ├── 05_imports.tsv               # 63K — external symbol dependencies
│   │   ├── 06_segments.tsv              # Segment layout
│   │   ├── 07_vtables_metaclasses.tsv   # 38K — vtable + metaclass info
│   │   ├── 08_interesting_string_xrefs.tsv  # 416K — cross-references
│   │   ├── 08b_functions_from_string_xrefs.tsv  # 57K
│   │   ├── 09_selected_callgraph.tsv    # 39MB — call graph relationships
│   │   ├── 11_comments.tsv              # 172K — IDA analyst comments
│   │   ├── 12_entrypoints_and_exports.tsv # 438K
│   │   ├── 13_selected_pseudocode/      # ** 1829 FILES ** of decompiled C
│   │   │   ├── 0x*.cpp.txt             # One per selected function
│   │   │   └── ... (1829 total)
│   │   ├── 15_function_string_refs.tsv  # 605K
│   │   ├── 16_interesting_names.tsv     # 288K
│   │   ├── 17_local_types_best_effort.txt
│   │   ├── 18_original_binary_and_metadata/
│   │   │   └── Contents_MacOS/AppleIntelTGLGraphicsFramebuffer.i64  # IDA DB
│   │   ├── 19_selected_asm/             # ** 1829 FILES ** matching asm
│   │   ├── 20_function_offset_accesses.tsv  # ** 7MB — this+offset mining **
│   │   ├── 21_constants_split.tsv       # ** 23MB — MMIO register constants **
│   │   ├── 22_class_method_index.tsv    # 279K — class→method mapping
│   │   ├── 23_register_table_candidates.tsv  # 179K — register table entries
│   │   ├── 24_ai_structure_recovery_prompt.txt
│   │   ├── notes/                       # Analyst notes
│   │   │   ├── class_relationships.md
│   │   │   ├── next_ida_type_import_plan.md
│   │   │   ├── offset_evidence.md
│   │   │   └── register_constant_matches.md
│   │   └── recovered_headers/           # ** RECOVERED STRUCTURES **
│   │       ├── AppleIntelFramebufferController_Recovered.h
│   │       ├── AppleIntelPortHAL_Recovered.h
│   │       ├── AppleIntelPowerWell_Recovered.h
│   │       └── appleintel_tgl_recovered.h
│   │
│   ├── AppleIntelTGLGraphics_kasan/     # Accelerator (kasan debug build)
│   │   ├── 01_functions.txt            # 412K
│   │   ├── 02_names.txt                # 1.1M
│   │   ├── 03_strings.txt              # 212K
│   │   ├── 09_selected_callgraph.txt   # 55MB
│   │   ├── 10_selected_constants.txt   # 3.7MB — accelerator MMIO constants
│   │   ├── 13_selected_pseudocode/     # ** 841 FILES **
│   │   │   ├── 0x1cb140_IGAccelDisplayMachine_MetaClass_MetaClass(void).cpp.txt
│   │   │   ├── 0x1cc6be_IGAccelDisplayPipe_startScaledMode(...).cpp.txt
│   │   │   ├── 0x1cdfee_IGAccelDisplayPipe_IGAccelDisplayPipe(void).cpp.txt
│   │   │   ├── 0xa459a_blit3d_initialize_scratch_space(...).cpp.txt
│   │   │   ├── 0x18b8b2_IGHardwareRingBufferMedia_operator_delete(...).cpp.txt
│   │   │   ├── 0x1e6a2a_IGHardwareRingBufferMain_getStampSpace(void).cpp.txt
│   │   │   ├── 0xf1e3a_IGHardwareRingBuffer_dumpRegisters(void).cpp.txt
│   │   │   ├── 0xe6918_IGHardwareRingBuffer_getFlushTLBSpace(void).cpp.txt
│   │   │   ├── 0x58bc6_IGGuC_isGpuIdle(void).cpp.txt
│   │   │   ├── 0x85812_IntelAccelerator_initHardwareStatusPageRegisters(void).cpp.txt
│   │   │   ├── 0x6bb8c_IGHardwareGuC_dumpContextInfo(uint).cpp.txt
│   │   │   ├── 0x47a66_IGFlushNotifyInterruptEventSource_initWithOptions(...).cpp.txt
│   │   │   ├── 0x62210_IGHardwareCommandStreamer4_hardwareCommandStreamer(...).cpp.txt
│   │   │   ├── 0x13e0d6_IGAccelGLContext_contextStart(void).cpp.txt
│   │   │   ├── 0x18e9dc_IGAccelResource_init(...).cpp.txt
│   │   │   ├── 0xec174_IGHardwareRingBuffer_drvErrLog(...).cpp.txt
│   │   │   ├── 0x82eee_IntelAccelerator_initMemoryManager(void).cpp.txt
│   │   │   └── 0xeb768_IGHardwareRingBuffer_checkEngineStatus(...).cpp.txt
│   │   ├── 15_function_string_refs.txt # 220K
│   │   ├── 16_interesting_names.txt    # 613K
│   │   └── (remaining 24 export files)
│   │
│   ├── AppleIntelTGLGraphicsGLDriver/   # OpenGL driver IDA export
│   ├── AppleIntelTGLGraphicsMTLDriver/  # Metal driver IDA export
│   └── AppleIntelTGLGraphicsVADriver/   # Video acceleration IDA export
│
├── DTK-rootfs/                           # Full DTK root filesystem
│   ├── kernel.development.j314s          # ** **DTK kernel dSYM (12MB)
│   ├── kernel.development.t8103*         # M1 kernel dSYMs
│   └── System/Library/Extensions/        # ** All Intel GPU kexts **
│       ├── AppleIntelTGLGraphics*.kext/  # Full set
│       ├── AppleIntelICL*.kext/          # ICL for comparison
│       ├── AppleIntelKBL*.kext/          # KBL for comparison
│       └── AppleIntelSKL*.kext/          # SKL for comparison
│
├── Project FIles/NootedGreen/            # ** MAIN ANALYSIS SANDBOX **
│   ├── DTK-Sources/                      # ** GHIDRA DECOMPILED C SOURCES **
│   │   ├── AppleIntelTGLGraphicsFramebuffer.c      # 5.1 MB
│   │   ├── AppleIntelTGLGraphicsFramebuffer.h       # 121K — recovered header
│   │   ├── AppleIntelTGLGraphics_kasan.c            # 15.6 MB — accelerator!
│   │   ├── AppleIntelTGLGraphics_kasan.h             # 40K — recovered header
│   │   ├── AppleIntelTGLGraphicsMTLDriver.c          # 6.1 MB
│   │   ├── AppleIntelTGLGraphicsMTLDriver.h          # 216K — recovered header
│   │   ├── AppleIntelTGLGraphicsGLDriver.c           # 100 MB — HUGE
│   │   ├── AppleIntelTGLGraphicsGLDriver.h           # 75K
│   │   ├── AppleIntelTGLGraphicsVADriver.c           # 8.2 MB
│   │   ├── AppleIntelTGLGraphicsVADriver.h           # 9.6K
│   │   ├── AppleIntelTGLGraphicsVAME.c               # 27K
│   │   ├── AppleIntelTGLGraphicsVAME.h               # 3.2K
│   │   ├── IOGPUFamily_kasan.c                       # 2.9 MB
│   │   ├── IOGPUFamily_kasan.h                       # 79K
│   │   ├── IOAcceleratorFamily2_kasan.c              # 8.1 MB
│   │   ├── IOAcceleratorFamily2_kasan.h              # 110K
│   │   ├── IOGraphicsFamily_kasan.c                  # 3.5 MB
│   │   ├── IOGraphicsFamily_kasan.h                  # 306K
│   │   ├── IOSurface_kasan.c                         # 2.0 MB
│   │   ├── IOSurface_kasan.h                         # 72K
│   │   ├── AppleBacklight_kasan.c                    # 180K
│   │   ├── AppleGPUWrangler.c                        # 149K
│   │   ├── AppleDPDisplay_kasan.c                    # 1.4 MB
│   │   └── (14 more support kext sources)
│   │
│   ├── Intel-PRM-TGL/                   # ** OFFICIAL INTEL HARDWARE DOCS **
│   │   ├── tgl-vol01-preface.pdf
│   │   ├── tgl-vol02a-command-reference-instructions.pdf     # 14MB
│   │   ├── tgl-vol02b-command-reference-enumerations.pdf
│   │   ├── tgl-vol02c-registers-part1.pdf                    # 12MB
│   │   ├── tgl-vol02c-registers-part2.pdf                    # 14MB
│   │   ├── tgl-vol02d-structures.pdf
│   │   ├── tgl-vol03-gpu-overview.pdf
│   │   ├── tgl-vol04-configurations.pdf
│   │   ├── tgl-vol05-memory-data-formats.pdf
│   │   ├── tgl-vol06-memory-views.pdf
│   │   ├── tgl-vol07-memory-cache.pdf
│   │   ├── tgl-vol08-command-stream-programming.pdf
│   │   ├── tgl-vol09-render-engine.pdf              # 9.8MB
│   │   ├── tgl-vol10-copy-engine.pdf
│   │   ├── tgl-vol11-media-engine.pdf               # 5MB
│   │   ├── tgl-vol12-display-engine.pdf             # 6MB — KEY for FB work
│   │   ├── tgl-vol13-general-assets.pdf
│   │   └── tgl-vol14-workarounds.pdf
│   │
│   ├── i915_xe_extracted/               # Linux i915/Xe driver code
│   │   ├── (137 files — register headers, init sequences, workarounds)
│   │
│   ├── mac-gfx-research-main/           # Apple graphics architecture research
│   │   ├── README.md
│   │   └── (architecture analysis docs)
│   │
│   ├── HookCase/                        # Full HookCase framework
│   ├── Display_Merge/                   # Display merge nub experiments
│   ├── docs/                            # Documentation
│   ├── logs/                            # 137 log files from test boots
│   └── subagents/                       # Subagent scripts
│
│   **Excluded from scope (TGL+Sonoma only):** 
│   ICL/KBL/SKL/BDW/HSW legacy kexts, AGX Apple Silicon, AMD/NVIDIA,
│   Ventura-specific patches.
│
├── sonoma-Kexts/                        # Sonoma reference kexts
│   ├── IOGPUFamily.kext/                # For interface comparison
│   ├── IOAcceleratorFamily2.kext/
│   ├── IOGraphicsFamily.kext/
│   ├── IOSurface.kext/
│   ├── AppleBacklight.kext/
│   ├── AppleGFXHDA.kext/
│   └── (13 more reference kexts)
│
├── IDA_Export.zip                       # 470MB — full IDA export archive
├── DTK-Kexts.zip                        # 413MB — DTK kext archive
│
├── GhidraProjects/
│   ├── TGL_DTK_Reverse.gpr              # Ghidra project for DTK TGL
│   └── TGL_DTK_Reverse.rep/            # Ghidra repo data
│
├── TGL_Ghidra_Headers/
│   └── tgl_recovery_minimal.h           # Minimal recovered structures
│
├── HookCase-TigerLakeHookLibrary/       # Userspace tracing scaffold
│   ├── hook.mm                          # Hook implementation
│   ├── hook.dylib                       # Pre-built hook library
│   └── Makefile                         # Build for hook library
│
├── Materials/
│   └── FakeIrisXE-main/                 # Reference Lilu plugin project
│       ├── FakeIrisXE/                  # Source code (42 files)
│       └── build/                       # Pre-built output
│
├── decompkernelcache/                   # Decompressed kernel cache
├── drm/                                 # DRM subsystem research
├── dsce/                                # DSCE (Display Stream Compression Engine)
├── live_sonoma_full_boot_log.txt        # 126MB — full Sonoma boot log
│
└── Graphics/
    └── GPUSDocumentation/               # GPU documentation HTML
```

---

## Asset Value Assessment

### Critical — Use daily during implementation

| Asset | Why |
|---|---|
| **DTK-Sources/** — `AppleIntelTGLGraphicsFramebuffer.c` (5.1MB) | Complete decompiled C of the framebuffer kext. Contains every function, class method, vtable, and register access pattern. Primary reference for understanding display pipeline. |
| **DTK-Sources/** — `AppleIntelTGLGraphics_kasan.c` (15.6MB) | Complete decompiled C of the accelerator kext. GPU scheduling, GuC, forcewake, memory management, ring setup. Primary reference for understanding GPU init. |
| **DTK-Sources/** — `IOGPUFamily_kasan.c` + `.h` | IOGPUFamily kernel class implementations. Critical for understanding what methods the TGL kext calls and what interfaces need to be bridged for Sonoma. |
| **Intel PRM Vol 12** — Display Engine | Official Intel documentation for display registers, port configurations, PHY sequences, panel power sequencing, pipe timing. Essential for understanding what the FB code is doing. |
| **Intel PRM Vol 2c** — Registers (both parts) | Complete register map for TGL. Cross-reference for every MMIO address in the decompiled code. |
| **IDA_Export/** — `20_function_offset_accesses.tsv` (7MB) | Every `this+offset` and `register+offset` access in the FB binary. Directly tells you structure field layouts. |
| **IDA_Export/** — `21_constants_split.tsv` (23MB) | Every MMIO register constant used in the FB binary, split into low32/high32. Register table mining. |

### High — Weekly reference

| Asset | Why |
|---|---|
| **DTK-Sources/** — `IOAcceleratorFamily2_kasan.c` (8.1MB) | IOAccelF2 interface implementation for vtable offset analysis |
| **DTK-Sources/** — `IOGraphicsFamily_kasan.c` (3.5MB) | IOFramebuffer base class implementation |
| **DTK-Sources/** — `IOSurface_kasan.c` (2.0MB) | IOSurface implementation for API compatibility |
| **DTK-Sources/** — `AppleIntelTGLGraphicsMTLDriver.c` (6.1MB) | Metal driver for understanding GPU command submission path |
| **DTK-Sources/** — `AppleIntelTGLGraphicsVADriver.c` (8.2MB) | Video acceleration driver |
| **IDA_Export/\*/13_selected_pseudocode/** (2670 files total) | Targeted decompilation with IDA analyst comments |
| **IDA_Export/\*/recovered_headers/** | Recovered C structures with field offsets (H/M/L confidence) |
| **IDA_Export/\*/notes/** | Analyst reasoning, offset evidence, next steps |
| **Intel PRM Vol 9** — Render Engine | RCS pipe, 3D pipeline, shader cores |
| **Intel PRM Vol 10** — Copy Engine | BCS blitter engine |
| **Intel PRM Vol 11** — Media Engine | VCS/VECS video encode/decode |
| **i915_xe_extracted/** | Linux register definitions for cross-referencing |

### Medium — Useful for specific tasks

| Asset | Why |
|---|---|
| **DTK kernel dSYM** (`kernel.development.j314s`) | IOGPU/IOFramebuffer kernel symbols for reverse engineering |
| **Sonoma kexts** | Binary comparison for interface delta analysis |
| **HookCase-TigerLakeHookLibrary** | Userspace tracing for GL/Metal/VA driver crashes |
| **FakeIrisXE** | Reference implementation for Lilu plugin patterns |
| **TGL_Ghidra_Headers/** | Minimal recovered structures as starting point |
| **DTK rootfs ICL/KBL/SKL kexts** | Cross-generation comparison for interface evolution |
| **DTK-Kexts IOGPUFamily (v16.2)** | IOGPU interface at the time TGL was current |

### Low — Available if needed

| Asset | Why |
|---|---|
| **AGX (Apple Silicon) GPU bundles** | Apple GPU architecture (different from Intel) but shares IOGPU interface |
| **live_sonoma_full_boot_log.txt** | Crash analysis reference |
| **NootedBlue** | Legacy platform code patterns |
| **Graphics GPUSDocumentation** | Generic GPU documentation |

---

## Quick Reference: Most Important Files

### For understanding display init (Phase 3 priority)
```
DTK-Sources/AppleIntelTGLGraphicsFramebuffer.c   → search for:
  - AppleIntelFramebufferController::start
  - AppleIntelFramebufferController::probeDisplayPipes
  - AppleIntelPortHAL::init
  - AppleIntelPortHAL::probePortMode
  - AppleIntelPowerWell::init
  - AppleIntelPowerWell::hwSetPowerWellStatePG

IDA notes/register_constant_matches.md           → MMIO register aliases
IDA notes/offset_evidence.md                     → Structure field verification
```

### For understanding GPU init (Phase 4 priority)
```
DTK-Sources/AppleIntelTGLGraphics_kasan.c       → search for:
  - IntelAccelerator::start
  - IntelAccelerator::initMemoryManager
  - IntelAccelerator::initHardwareStatusPageRegisters
  - IGGuC::loadGuCBinary
  - IGHardwareGuC::gucInit
  - IGHardwareRingBuffer::initRingRegisters
  - IGHardwareCommandStreamer::hardwareCommandStreamer
  - IGHardwareBlit3DContext::initialize
```

### For understanding interface delta (Phase 0 priority)
```
DTK-Sources/IOGPUFamily_kasan.c                  → all IOGPU class implementations
DTK-Sources/IOAcceleratorFamily2_kasan.c         → all IOAccelF2 class implementations
Sonoma kexts/*/Contents/MacOS/*                  → binary comparison targets
```

### For MMIO register understanding (ongoing)
```
Intel-PRM-TGL/tgl-vol02c-registers-part1.pdf    → register descriptions
Intel-PRM-TGL/tgl-vol02c-registers-part2.pdf
Intel-PRM-TGL/tgl-vol12-display-engine.pdf       → display-specific registers
i915_xe_extracted/                               → Linux definitions for cross-ref
IDA FBT 21_constants_split.tsv                   → MMIO constants from FB binary
IDA FBT 23_register_table_candidates.tsv         → register table entries
```

---

## Recommendations

1. **Phase 0 (Interface Delta)** must be done first. Compare the DTK-Sources IOGPUFamily_kasan.c against Sonoma's IOGPUFamily binary using nm and vtable analysis. Every shifted vtable entry is a patch target.

2. **Use the IDA_Export feature files before the decompiled C** for structure recovery. The `20_function_offset_accesses.tsv` (this+offset mining) is cleaner than hunting through Ghidra decompilation for field offsets.

3. **PCIe register block offsets** in the TGL FB (0x640xxx range identified in PortHAL recovered headers) should be cross-referenced with Intel PRM Vol 2c to confirm register purposes.

4. **The 100MB GLDriver.c decompilation** is likely overkill for most work — focus on the FB and Accelerator decompilations first. The GLDriver is relevant mainly for Phase 5 (userspace).

5. **Use HookCase for DYLD patch testing** — hook the graphics bundles before applying Lilu kernel-side DYLD patches to observe behavior.
