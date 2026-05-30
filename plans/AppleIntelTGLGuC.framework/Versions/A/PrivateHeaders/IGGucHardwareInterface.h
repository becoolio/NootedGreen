#ifndef IG_HW_IFC_H
#define IG_HW_IFC_H

#define IG_GUC_DEBUG_GPU_HANG 1



typedef uint32_t IGVirtualAddress32;
typedef uint64_t IGVirtualAddress64;



#ifdef __cplusplus

// The Framebuffer driver doesn't compile against C++11 or later,
// but must share the PM data structures with the Accelerator and
// the GuC uKernel.  Key off something only IOFB defines to know
// that we're not supposed to use the newer C++ standard.
#ifdef DEBUGCLASS
#define static_assert(EXPR, MSG) _Static_assert(EXPR, MSG)
#endif // IOFB

#if !defined(GUC_UKERNEL_BUILD)

#ifndef DEBUGCLASS
#include "IGFX.h"
#endif // IOFB

// These are defined in ucMacros.h for MIA
#define IG_QUOTEME_(X)                #X
#define IG_QUOTEME(X)                 IG_QUOTEME_(X)
#define IG_KBYTES(X)                  ((X) * 1024)
#define IG_ARRAY_COUNT(X)             (sizeof((X)) / sizeof((X)[0]))
#define IG_BITFIELD_BIT(BIT)          (1)
#define IG_BITFIELD_RANGE(END, START) ((END) - (START) + 1)
#define IG_BITFIELD32_BIT(BIT)        (1U << (BIT))

// These are defined in ucTypes.h for MIA
#define IG_FALSE 0
#define IG_TRUE  1

#define IG_PAGE_SIZE IG_KBYTES(4)

#else

#include "ucRegOffsets.h"
#include "ucRegStructs.h"

#endif // !GUC_UKERNEL_BUILD

// From IntelDefines.h, copied here to deal with cross-compiling and order-of-includes.
#ifndef _INTEL_CACHE_LINE_SIZE_DEFINED
    #define _INTEL_CACHE_LINE_SIZE_DEFINED
    #define CACHE_LINE_SHIFT 6
    #define CACHE_LINE_SIZE  (1 << CACHE_LINE_SHIFT)
#endif



#define IG_IA_CACHE_LINE_SIZE  (CACHE_LINE_SIZE)

#define IG_MIA_CACHE_LINE_SHIFT 4
#define IG_MIA_CACHE_LINE_SIZE (1 << IG_MIA_CACHE_LINE_SHIFT)



#define IG_FOREACH_SCHED_PRIORITY(IG_DEF_SCHED_PRI_INIT)                            \
    IG_DEF_SCHED_PRI_INIT(High      , "HIGH"       , 4)                             \
    IG_DEF_SCHED_PRI_INIT(NormalHigh, "NORMAL_HIGH", 3)                             \
    IG_DEF_SCHED_PRI_INIT(Normal    , "NORMAL"     , 2)                             \
    IG_DEF_SCHED_PRI_INIT(Low       , "LOW"        , 1)                             \



#define IG_DECL_SCHED_PRI_ENUM(NM, SZ, CRED)                                        \
    kIGSchedPriority ## NM,



#define IG_DECL_SCHED_PRI_MASK(NM, SZ, CRED)                                        \
    kIGSchedPriorityMask ## NM = (1U << kIGSchedPriority ## NM),                    \



#define IG_DECL_SCHED_PRI_NAME(NM, SZ, CRED)                                        \
    SZ,

#define IG_DECL_SCHED_PRI_DEFAULT_CREDITS(NM, SZ, CRED)                             \
    kIGSchedPriority ## NM ## DefaultCredits = CRED,



#define IG_FOREACH_HW_CS_DESC(IG_DEF_HW_CS_INIT)                                    \
    IG_DEF_HW_CS_INIT(RCS  , Render , 80)                                           \
    IG_DEF_HW_CS_INIT(CCS  , Compute, 80)                                           \
    IG_DEF_HW_CS_INIT(BCS  , Blit   ,  8)                                           \
    IG_DEF_HW_CS_INIT(VCS0 , Media  ,  8)                                           \
    IG_DEF_HW_CS_INIT(VCS2 , Media  ,  8)                                           \
    IG_DEF_HW_CS_INIT(VECS0, Media  ,  8)                                           \



#define IG_DECL_HW_CS_TYPE(CS, LABEL, SZ)                                           \
    kIGHwCsType ## CS,                                                              \



#define IG_DECL_HW_CS_MASK(CS, LABEL, SZ)                                           \
    kIGHwCsMask ## CS = (1U << kIGHwCsType ## CS),                                  \



#define IG_DEFINE_HW_CS_DESC_INITIALIZER_COMMON(CS, LABEL, SZ)                      \
    kIGHwCsType ## CS,                                                              \
    kIGHwCsMask ## CS,                                                              \
    IG_QUOTEME(CS),                                                                 \
    IG_QUOTEME(LABEL),                                                              \
    GFXREG_ ## CS ## _ELSP,                                                         \
    GFXREG_ ## CS ## _EL_SUBMIT_QUEUE,                                              \
    GFXREG_ ## CS ## _EL_CONTROL,                                                   \
    GFXREG_ ## CS ## _EXECLIST_STATUS,                                              \
    GFXREG_ ## CS ## _CTXT_ST_PTR,                                                  \
    GFXREG_ ## CS ## _CTXT_ST_BUF,                                                  \
    GFXREG_ ## CS ## _CTXT_ST_PORT,                                                 \
    GFXREG_ ## CS ## _CTXT_ST_FIFO_ST,                                              \
    GFXREG_COMMAND_ ## CS ## _HWS_PGA,                                              \
    GFXREG_ ## CS ## _GFX_MODE,                                                     \
    GFXREG_ ## CS ## _RESET_CTRL,                                                   \
    GFXREG_CONTROL_ ## CS ## _EIR,                                                  \
    GFXREG_CONTROL_ ## CS ## _EMR,                                                  \
    GFXREG_ ## CS ## _TIMESTAMP,                                                    \
    GFXREG_ ## CS ## _GPR_0,                                                        \
    FUSE_GT_ ## CS,                                                                 \
    IG_KBYTES(SZ),                                                                  \
    IG_STAMP_INDEX_RANGE_ ## CS ## _MIN,                                            \
    IG_STAMP_INDEX_RANGE_ ## CS ## _MIN + IG_STAMP_INDEX_RANGE_ ## CS ## _SIZE - 1, \
    IG_STAMP_INDEX_RANGE_ ## CS ## _SIZE                                            \



#define IG_DEFINE_HW_CS_DESC_INITIALIZER_KEXT(CS, SZ)                               \
    kIGFBContextSwitchInterruptType ## CS,                                          \
    kIGFBFlushNotifyInterruptType ## CS,                                            \
    kIGFBErrorInterruptType ## CS,                                                  \
    GFXREG_ ## CS ## _FORCEWAKE_REQ,                                                \
    GFXREG_ ## CS ## _FORCEWAKE_ACK,                                                \



#if !defined(GUC_UKERNEL_BUILD)

#define IG_DEFINE_HW_CS_DESC_INITIALIZER(CS, LABEL, SZ)                             \
{                                                                                   \
    IG_DEFINE_HW_CS_DESC_INITIALIZER_COMMON(CS, LABEL, SZ),                         \
    IG_DEFINE_HW_CS_DESC_INITIALIZER_KEXT(CS, SZ)                                   \
},                                                                                  \

#else

#define IG_DEFINE_HW_CS_DESC_INITIALIZER(CS, LABEL,  SZ)                            \
{                                                                                   \
    IG_DEFINE_HW_CS_DESC_INITIALIZER_COMMON(CS, LABEL, SZ)                          \
},                                                                                  \

#endif // !GUC_UKERNEL_BUILD



//
// IOAccelFIFOChannel stamp index to hardware command streamer mapping:
//
// RCS : [ 0, 23]
// CCS : [24, 31]
// BCS : [32, 35]
// VCS : [36, 51], divided equally between 2 instances
// VECS: [52, 59]
// SW  : [60, 63]
//
#define IG_STAMP_INDEX_RANGE_RCS_MIN                0
#define IG_STAMP_INDEX_RANGE_RCS_SIZE               24

#define IG_STAMP_INDEX_RANGE_CCS_MIN                (IG_STAMP_INDEX_RANGE_RCS_MIN + IG_STAMP_INDEX_RANGE_RCS_SIZE)
#define IG_STAMP_INDEX_RANGE_CCS_SIZE               8

#define IG_STAMP_INDEX_RANGE_BCS_MIN                (IG_STAMP_INDEX_RANGE_CCS_MIN + IG_STAMP_INDEX_RANGE_CCS_SIZE)
#define IG_STAMP_INDEX_RANGE_BCS_SIZE               4

#define IG_STAMP_INDEX_RANGE_VCS0_MIN               (IG_STAMP_INDEX_RANGE_BCS_MIN + IG_STAMP_INDEX_RANGE_BCS_SIZE)
#define IG_STAMP_INDEX_RANGE_VCS0_SIZE              8

#define IG_STAMP_INDEX_RANGE_VCS2_MIN               (IG_STAMP_INDEX_RANGE_VCS0_MIN + IG_STAMP_INDEX_RANGE_VCS0_SIZE)
#define IG_STAMP_INDEX_RANGE_VCS2_SIZE              8

#define IG_STAMP_INDEX_RANGE_VECS0_MIN              (IG_STAMP_INDEX_RANGE_VCS2_MIN + IG_STAMP_INDEX_RANGE_VCS2_SIZE)
#define IG_STAMP_INDEX_RANGE_VECS0_SIZE             8

#define IG_STAMP_INDEX_RANGE_SW_MIN                 (IG_STAMP_INDEX_RANGE_VECS0_MIN + IG_STAMP_INDEX_RANGE_VECS0_SIZE)
#define IG_STAMP_INDEX_RANGE_SW_SIZE                4

#define IG_MAX_STAMP_INDEX                          (IG_STAMP_INDEX_RANGE_SW_MIN + IG_STAMP_INDEX_RANGE_SW_SIZE)



#ifndef DEBUGCLASS

/*!
 * @abstract The hardware command streamer type identifier.
 *
 * @const kIGHwCsTypeRCS  The render command streamer.
 * @const kIGHwCsTypeCCS  The compute command streamer.
 * @const kIGHwCsTypeBCS  The blit command streamer.
 * @const kIGHwCsTypeVCSn The nth instance of the video command streamer.
 * @const kIGHwCsTypeVECS The video enhancement command streamer.
 * @const kIGNumHwCsTypes The absolute maximum number of hardware command streamers.
 *
 */
enum IGHwCsType : uint8_t
{
    IG_FOREACH_HW_CS_DESC(IG_DECL_HW_CS_TYPE)

    kIGNumHwCsTypes
};



/*!
 * @abstract The HW design replicates some command streamers.
 *
 * @const kIGMaxHwInstancesRCS  The maximum possible number of hardware instances of the render pipe.
 * @const kIGMaxHwInstancesCCS  The maximum possible number of hardware instances of the compute pipe.
 * @const kIGMaxHwInstancesBCS  The maximum possible number of hardware instances of the blitter.
 * @const kIGMaxHwInstancesVCS  The maximum possible number of hardware instances of the VDBOX.
 * @const kIGMaxHwInstancesVECS The maximum possible number of hardware instances of the VEBOX.
 */
static const uint8_t kIGMaxHwInstancesRCS       = 1;
static const uint8_t kIGMaxHwInstancesCCS       = 1;
static const uint8_t kIGMaxHwInstancesBCS       = 1;
static const uint8_t kIGMaxHwInstancesVCS       = 2;
static const uint8_t kIGMaxHwInstancesVECS      = 1;

static const uint8_t kIGMaxHwEngines            = ( kIGMaxHwInstancesRCS  +
                                                    kIGMaxHwInstancesCCS  +
                                                    kIGMaxHwInstancesBCS  +
                                                    kIGMaxHwInstancesVCS  +
                                                    kIGMaxHwInstancesVECS );



static_assert(kIGMaxHwEngines == kIGNumHwCsTypes, "Scheduled HW engine and command streamer count mismatch");



/*!
 * @const IG_STAMP_INDEX_TO_HWCS_LUT Channel to hardware command streamer mapping.
 */
static const IGHwCsType IG_STAMP_INDEX_TO_HWCS_LUT[IG_MAX_STAMP_INDEX] =
{
    kIGHwCsTypeRCS,     //  0
    kIGHwCsTypeRCS,     //  1
    kIGHwCsTypeRCS,     //  2
    kIGHwCsTypeRCS,     //  3
    kIGHwCsTypeRCS,     //  4
    kIGHwCsTypeRCS,     //  5
    kIGHwCsTypeRCS,     //  6
    kIGHwCsTypeRCS,     //  7
    kIGHwCsTypeRCS,     //  8
    kIGHwCsTypeRCS,     //  9
    kIGHwCsTypeRCS,     // 10
    kIGHwCsTypeRCS,     // 11
    kIGHwCsTypeRCS,     // 12
    kIGHwCsTypeRCS,     // 13
    kIGHwCsTypeRCS,     // 14
    kIGHwCsTypeRCS,     // 15
    kIGHwCsTypeRCS,     // 16
    kIGHwCsTypeRCS,     // 17
    kIGHwCsTypeRCS,     // 18
    kIGHwCsTypeRCS,     // 19
    kIGHwCsTypeRCS,     // 20
    kIGHwCsTypeRCS,     // 21
    kIGHwCsTypeRCS,     // 22
    kIGHwCsTypeRCS,     // 23

    kIGHwCsTypeCCS,     // 24
    kIGHwCsTypeCCS,     // 25
    kIGHwCsTypeCCS,     // 26
    kIGHwCsTypeCCS,     // 27
    kIGHwCsTypeCCS,     // 28
    kIGHwCsTypeCCS,     // 29
    kIGHwCsTypeCCS,     // 30
    kIGHwCsTypeCCS,     // 31

    kIGHwCsTypeBCS,     // 32
    kIGHwCsTypeBCS,     // 33
    kIGHwCsTypeBCS,     // 34
    kIGHwCsTypeBCS,     // 35

    kIGHwCsTypeVCS0,    // 36
    kIGHwCsTypeVCS0,    // 37
    kIGHwCsTypeVCS0,    // 38
    kIGHwCsTypeVCS0,    // 39
    kIGHwCsTypeVCS0,    // 40
    kIGHwCsTypeVCS0,    // 41
    kIGHwCsTypeVCS0,    // 42
    kIGHwCsTypeVCS0,    // 43
    kIGHwCsTypeVCS2,    // 44
    kIGHwCsTypeVCS2,    // 45
    kIGHwCsTypeVCS2,    // 46
    kIGHwCsTypeVCS2,    // 47
    kIGHwCsTypeVCS2,    // 48
    kIGHwCsTypeVCS2,    // 49
    kIGHwCsTypeVCS2,    // 50
    kIGHwCsTypeVCS2,    // 51

    kIGHwCsTypeVECS0,   // 52
    kIGHwCsTypeVECS0,   // 53
    kIGHwCsTypeVECS0,   // 54
    kIGHwCsTypeVECS0,   // 55
    kIGHwCsTypeVECS0,   // 56
    kIGHwCsTypeVECS0,   // 57
    kIGHwCsTypeVECS0,   // 58
    kIGHwCsTypeVECS0,   // 59

    kIGNumHwCsTypes,    // 60
    kIGNumHwCsTypes,    // 61
    kIGNumHwCsTypes,    // 62
    kIGNumHwCsTypes,    // 63
};



/*!
 * @const IG_STAMP_INDEX_TO_HWCS_LUT_SCI Channel to hardware command streamer mapping for single-channel-index mode.
 */
static const IGHwCsType IG_STAMP_INDEX_TO_HWCS_LUT_SCI[kIGMaxHwEngines] =
{
    kIGHwCsTypeRCS,     //  0
    kIGHwCsTypeCCS,     //  1
    kIGHwCsTypeBCS,     //  2
    kIGHwCsTypeVCS0,    //  3
    kIGHwCsTypeVCS2,    //  4
    kIGHwCsTypeVECS0,   //  5
};



inline IGHwCsType IG_GET_HWCSTYPE_FOR_STAMP_INDEX(uint32_t stampIndex, uint32_t sci)
{
    if (sci)
    {
        return IG_STAMP_INDEX_TO_HWCS_LUT_SCI[stampIndex];
    }
    else
    {
        return IG_STAMP_INDEX_TO_HWCS_LUT[stampIndex];
    }
}



/*!
 * @abstract Hardware command streamer bit masks.
 *
 * @const kIGHwCsMaskRCS    Corresponds to RCS.
 * @const kIGHwCsMaskCCS    Corresponds to CCS.
 * @const kIGHwCsMaskBCS    Corresponds to BCS.
 * @const kIGHwCsMaskVCSn   Corresponds to the nth instance of VCS.
 * @const kIGHwCsMaskVECS   Corresponds to VECS.
 * @const kIGHwCsMaskAll    Corresponds to all command streamers.
 * @const kIGHwCsMaskRender For GuC firmware internal use only.
 * @const kIGHwCsMaskMedia  For GuC firmware internal use only.
 */
enum : uint32_t
{
    IG_FOREACH_HW_CS_DESC(IG_DECL_HW_CS_MASK)

    kIGHwCsMaskAll    = (1U << kIGNumHwCsTypes) - 1,

    kIGHwCsMaskRender = (  kIGHwCsMaskRCS
                         | kIGHwCsMaskCCS),

    kIGHwCsMaskMedia  = (  kIGHwCsMaskVCS0
                         | kIGHwCsMaskVCS2
                         | kIGHwCsMaskVECS0 )
};



#endif // IOFB



/*!
 * @abstract Scheduler priority level.
 */
enum
{
    IG_FOREACH_SCHED_PRIORITY(IG_DECL_SCHED_PRI_ENUM)

    kIGNumSchedPriorities,

    kIGSchedPriorityDefault = kIGSchedPriorityNormal,
    kIGSchedPriorityInvalid = 0x7F
};

typedef uint8_t IGSchedPriority;



/*!
 * @abstract Scheduler priority level mask.
 *
 * @discussion
 * Used by the PM sub-module components.
 */
enum IGSchedPriorityMask : uint32_t
{
    IG_FOREACH_SCHED_PRIORITY(IG_DECL_SCHED_PRI_MASK)

    kIGSchedPriorityMaskAll = (1U << kIGNumSchedPriorities) - 1,

    kIGSchedPriorityMaskForceDWORD = 0x7FFFFFFF
};

static_assert(sizeof(IGSchedPriorityMask) == sizeof(uint32_t), "SchedPriorityMask size mismatch");


enum IGSchedPriorityCredits : uint32_t
{
    IG_FOREACH_SCHED_PRIORITY(IG_DECL_SCHED_PRI_DEFAULT_CREDITS)
};


#if LOG_ENABLE || INTEL_LOG_ENABLE
/*!
 * @abstract Scheduler priority level names.
 */
static const char * kIGSchedPriorityName[] =
{
    IG_FOREACH_SCHED_PRIORITY(IG_DECL_SCHED_PRI_NAME)
    "INVALID"
};
#endif

// Reserve 4 unique channel indexes for each of high, normal_high and low priority

#define IG_SCHED_PRI_STAMP_INDEX_SHIFT              2
#define IG_SCHED_PRI_HIGH_STAMP_INDEX_MIN           IG_STAMP_INDEX_RANGE_RCS_MIN
#define IG_SCHED_PRI_HIGH_STAMP_INDEX_MAX           (IG_SCHED_PRI_HIGH_STAMP_INDEX_MIN + ((1 << IG_SCHED_PRI_STAMP_INDEX_SHIFT) - 1))
#define IG_SCHED_PRI_NORMAL_HIGH_STAMP_INDEX_MIN    (IG_SCHED_PRI_HIGH_STAMP_INDEX_MAX + 1)
#define IG_SCHED_PRI_NORMAL_HIGH_STAMP_INDEX_MAX    (IG_SCHED_PRI_NORMAL_HIGH_STAMP_INDEX_MIN + ((1 << IG_SCHED_PRI_STAMP_INDEX_SHIFT) - 1))
#define IG_SCHED_PRI_NORMAL_STAMP_INDEX_MIN         (IG_SCHED_PRI_NORMAL_HIGH_STAMP_INDEX_MAX + 1)
#define IG_SCHED_PRI_NORMAL_STAMP_INDEX_MAX         (IG_SCHED_PRI_LOW_STAMP_INDEX_MIN - 1)
#define IG_SCHED_PRI_LOW_STAMP_INDEX_MIN            (IG_SCHED_PRI_LOW_STAMP_INDEX_MAX + 1 - (1 << IG_SCHED_PRI_STAMP_INDEX_SHIFT))
#define IG_SCHED_PRI_LOW_STAMP_INDEX_MAX            (IG_STAMP_INDEX_RANGE_RCS_MIN + IG_STAMP_INDEX_RANGE_RCS_SIZE - 1)

#define IG_GET_SCHED_PRI_FOR_STAMP_INDEX(INDEX)     kStampIndexPriorityMappingTableRCS[INDEX >> IG_SCHED_PRI_STAMP_INDEX_SHIFT]

static_assert(IG_SCHED_PRI_NORMAL_STAMP_INDEX_MAX > IG_SCHED_PRI_NORMAL_STAMP_INDEX_MIN, "StampIndex :: Priority allocation mismatch!");


// Each index in this table maps to 4 stamp indexes
static const uint8_t kStampIndexPriorityMappingTableRCS[] =
{
    kIGSchedPriorityHigh,
    kIGSchedPriorityNormalHigh,
    kIGSchedPriorityNormal,
    kIGSchedPriorityNormal,
    kIGSchedPriorityNormal,
    kIGSchedPriorityLow
};
static_assert(sizeof(kStampIndexPriorityMappingTableRCS) == (IG_STAMP_INDEX_RANGE_RCS_SIZE / (1 << IG_SCHED_PRI_STAMP_INDEX_SHIFT)) * sizeof(uint8_t), "StampIndex :: Priority Table size mismatch!");



static const uint8_t kStampIndexPriorityMinTable[] =
{
    IG_SCHED_PRI_HIGH_STAMP_INDEX_MIN,
    IG_SCHED_PRI_NORMAL_HIGH_STAMP_INDEX_MIN,
    IG_SCHED_PRI_NORMAL_STAMP_INDEX_MIN,
    IG_SCHED_PRI_LOW_STAMP_INDEX_MIN
};
static_assert(sizeof(kStampIndexPriorityMinTable) == kIGNumSchedPriorities, "StampIndex :: Priority Min Table size mismatch!");



#ifndef DEBUGCLASS



/*!
 * @abstract Doorbell status.
 *
 * @discussion
 * This is specifically typed as an unsigned 32-bit integer for explicit data layout and ease
 * of interopration with the status field within @link IGDoorbell @/link.
 */
enum IGDoorbellStatus : uint32_t
{
    kIGDoorbellDisabled,
    kIGDoorbellEnabled
};



#endif // IOFB



// Matches GFXSKU_CAP_INDEX (from IGFXCaps.h)
enum IGGfxGtType
{
    kIGGfxTypeGT1,
    kIGGfxTypeGT2,
    kIGGfxTypeGT3,
    kIGGfxTypeGT4,

    kIGNumGfxGtTypes
};



#ifndef DEBUGCLASS

#pragma pack(push, 1)



enum : uint32_t
{
    // Log Buffers -- descriptors piggy-back work queue descriptor memory.
    kIGGucPerfLogBufWqdIndex            = IG_MAX_STAMP_INDEX + 1,
    kIGGucPrintfLogBufWqdIndex          = IG_MAX_STAMP_INDEX + 2,
    kIGGucAssertionBufWqdIndex          = IG_MAX_STAMP_INDEX + 3,
    kIGGucRetiredItemsWqdIndex          = IG_MAX_STAMP_INDEX + 4,

    // Work Queues
    kIGGucMaxWorkQueueEntries           = 512,  /* must be power of 2 */
    kIGGucMaxWorkQueueCoalesceEntries   = kIGGucMaxWorkQueueEntries / 2,
    kIGGucWorkQueueCapacityMask         = kIGGucMaxWorkQueueEntries - 1
};

// Corresponds to GUC_STATUS (0x0C000) bits [23:16] (P24C Core Status).
enum IGP24CCoreStatus : uint32_t
{
    kIGP24CCoreStatusDefault            = 0x00,
    kIGP24CCoreStatusRc6Exit            = 0x01,
    kIGP24CCoreStatusHaltRequested      = 0x02,
    kIGP24CCoreStatusIsrStatus          = 0x04,

    kIGP24CCoreStatusShift              = 0x10
};

// Corresponds to GUC_STATUS (0x0C000) bits [15:8] (uKernel/uOS Status).
enum IGUkStatus : uint32_t
{
    kIGUkStatusDefault                    = 0x00,
    kIGUkStatusStart                      = 0x01,
    kIGUkStatusErrorDeviceIdMismatch      = 0x02,
    kIGUkStatusErrorPlatformConfiguration = 0x03,
    kIGUkStatusGdtInstallDone             = 0x04,
    kIGUkStatusIdtInstallDone             = 0x08,
    kIGUkStatusLapicInitDone              = 0x10,
    kIGUkStatusGucInterruptSetupDone      = 0x20,
    kIGUkStatusGeneralException           = 0xF9,
    kIGUkStatusExceptionGucCtrlInvalid    = 0xFA,
    kIGUkStatusExceptionForceWakeTimeout  = 0xFB,
    kIGUkStatusAssertionFailed            = 0xFC,

    kIGUkStatusReady                      = 0xF0,

    kIGUkStatusExceptionMask              = 0xF8,
    kIGUkStatusShift                      = 0x00,
};


/*!
 * @discussion
 * This structure is written by the kext and read by the uKernel.
 */
struct IGGucCtrl
{
    union
    {
        struct
        {
            uint32_t fGfxGtType                             : IG_BITFIELD_RANGE(  6,  0 ); // IGGfxGtType
            uint32_t fGfxCoreFamily                         : IG_BITFIELD_RANGE( 12,  7 ); // IGGfxCoreFamily
            uint32_t fRevId                                 : IG_BITFIELD_RANGE( 28, 13 ); // 0 unless stepping-specific WA(s) are required in uKernel.
            uint32_t fDeviceInfoReserved                    : IG_BITFIELD_RANGE( 31, 29 ); // Unused
        };
        uint32_t fDeviceInfo;
    };

    union
    {
        struct
        {
            uint32_t fPerfLogBufSizeInPages4Blocks          : IG_BITFIELD_RANGE(  7,  0 ); // 0x1 == 16K, 0x80 == 2M
            uint32_t fPerfLogBufInitParamsReserved          : IG_BITFIELD_RANGE( 11,  8 );
            uint32_t fPerfLogBufGraphicsAddress             : IG_BITFIELD_RANGE( 31, 12 ); // Page-aligned performance log buffer graphics address.
        };
        uint32_t fPerfLogBufInitParams;
    };

    union
    {
        struct
        {
            uint32_t fAssertionBufSizeInPages4Blocks        : IG_BITFIELD_RANGE(  7,  0 ); // 0x1 == 16K, 0x80 == 2M
            uint32_t fAssertionBufInitParamsReserved        : IG_BITFIELD_RANGE( 11,  8 );
            uint32_t fAssertionBufGraphicsAddress           : IG_BITFIELD_RANGE( 31, 12 ); // Page-aligned assertion buffer graphics address.
        };
        uint32_t fAssertionBufInitParams;
    };

    union
    {
        struct
        {
            uint32_t fPmStatsBufSizeInPages4Blocks          : IG_BITFIELD_RANGE(  7,  0 ); // 0x1 == 16K, 0x80 == 2M
            uint32_t fPmStatsBufInitParamsReserved          : IG_BITFIELD_RANGE( 11,  8 );
            uint32_t fPmStatsBufGraphicsAddress             : IG_BITFIELD_RANGE( 31, 12 ); // Page-aligned performance log buffer graphics address.
        };
        uint32_t fPmStatsBufInitParams;
    };

    union
    {
        struct
        {
            uint32_t fCrashLogBufSizeInPages4Blocks         : IG_BITFIELD_RANGE(  7,  0 ); // 0x1 == 16K, 0x80 == 2M
            uint32_t fCrashLogBufInitParamsReserved         : IG_BITFIELD_RANGE( 11,  8 );
            uint32_t fCrashLogBufGraphicsAddress            : IG_BITFIELD_RANGE( 31, 12 ); // Page-aligned performance log buffer graphics address.
        };
        uint32_t fCrashLogBufInitParams;
    };

    union
    {
        struct
        {
            uint32_t fStampMemoryLengthInPages             : IG_BITFIELD_RANGE(  7,  0 );
            uint32_t fStampMemoryInitParamsReserved        : IG_BITFIELD_RANGE( 11,  8 );
            uint32_t fStampMemoryGraphicsAddress           : IG_BITFIELD_RANGE( 31, 12 );
        };
        uint32_t fStampMemoryInitParams;
    };

    union
    {
        struct
        {
            uint32_t fSingleChannelIndex                    : IG_BITFIELD_BIT  (      0 );
            uint32_t fDisableCoalescing                     : IG_BITFIELD_BIT  (      1 );
            uint32_t fUseMessageChannelWorkaround           : IG_BITFIELD_BIT  (      2 );
            uint32_t fEnablePreemption                      : IG_BITFIELD_BIT  (      3 );
            uint32_t fEnableCreditOverride                  : IG_BITFIELD_BIT  (      4 );
            uint32_t fDisablePageFaultHandling              : IG_BITFIELD_BIT  (      5 );
            uint32_t fWAIdleOnPreemptionDelay               : IG_BITFIELD_BIT  (      6 );
            uint32_t fDisableSpeculativeWaitQueueSubmission : IG_BITFIELD_BIT  (      7 );
            uint32_t fHighPriorityCredits                   : IG_BITFIELD_RANGE( 13,  8 );
            uint32_t fNormalHighPriorityCredits             : IG_BITFIELD_RANGE( 19, 14 );
            uint32_t fNormalPriorityCredits                 : IG_BITFIELD_RANGE( 25, 20 );
            uint32_t fLowPriorityCredits                    : IG_BITFIELD_RANGE( 31, 26 );
        };
        uint32_t fConfigAndWaTable;
    };

    union
    {
        struct
        {
            uint32_t fWorkQueueDescInitParamsReserved       : IG_BITFIELD_RANGE( 11,  0 );
            uint32_t fWorkQueueDescGraphicsAddress          : IG_BITFIELD_RANGE( 31, 12 );
        };
        uint32_t fWorkQueueDescInitParams;
    };

    union
    {
        struct
        {
            uint32_t Low;
            uint32_t High;
        };
        uint64_t Value;
    } fDoorbellGpa[kIGMaxHwEngines];

    union
    {
        struct
        {
            uint32_t fNumSQIDI                              : IG_BITFIELD_RANGE(  5,  0 );;
            uint32_t fDoorbellBufInitParamsReserved         : IG_BITFIELD_RANGE( 11,  6 );
            uint32_t fDoorbellUkAddress                     : IG_BITFIELD_RANGE( 31, 12 ); // Page-aligned Doorbell address
        };
        uint32_t fDoorbellBufInitParams;
    };

    union
    {
        struct
        {
            uint32_t fPrintfLogBufSizeInPages4Blocks        : IG_BITFIELD_RANGE(  7,  0 ); // 0x1 == 16K, 0x80 == 2M
            uint32_t fPrintfLogBufInitParamsReserved        : IG_BITFIELD_RANGE( 11,  8 );
            uint32_t fPrintfLogBufGraphicsAddress           : IG_BITFIELD_RANGE( 31, 12 ); // Page-aligned printf log buffer graphics address.
        };
        uint32_t fPrintfLogBufInitParams;
    };

    union
    {
        struct
        {
            uint32_t fRetiredItemsBufSizeInPages4Blocks     : IG_BITFIELD_RANGE(  7,  0 ); // 0x1 == 16K, 0x80 == 2M
            uint32_t fRetiredItemsBufInitParamsReserved     : IG_BITFIELD_RANGE( 11,  8 );
            uint32_t fRetiredItemsBufGraphicsAddress        : IG_BITFIELD_RANGE( 31, 12 ); // Page-aligned retired items buffer graphics address.
        };
        uint32_t fRetiredItemsBufInitParams;
    };

    uint32_t fRetiredWorkItemNotificationThreshhold;
    uint16_t fPreemptionDelayIntervalUS;
    uint16_t fPreemptionTimerIntervalUS;

    uint32_t fEnabledCommandStreamerMask;

    uint32_t padForAlignment[6];

    template <typename T>
    T * cast_to(IGVirtualAddress32 va) const
    {
        return reinterpret_cast<T *>(va << 12);
    }

    template <typename T>
    T *cast_to(IGVirtualAddress32 va, uint32_t index) const
    {
        T *p = this->cast_to<T>(va);
        return &p[index];
    }
};

// Ensure that the IGGucCtrl structure is padded to a multiple of IG_IA_CACHE_LINE_SIZE
static_assert(0 == (sizeof(IGGucCtrl) % IG_IA_CACHE_LINE_SIZE), "IGGucCtrl size mismatch");

// An IGSubmissionsRetired structure is used to track GT progress on a given command
// streamer even though multiple HW contexts with different stamp indices (as far as
// the host is concerned) may be in use.  With each HW ctx completion on a command
// streamer, it's submissions retired  will be incremented and written to host-visible
// memory.  The host can read that value if IOAF asserts that a stamp is taking
// too long and ensure that the value is advancing and if so, prevent a GPU Restart
// from being initiated.
struct IGSubmissionsRetired
{
    uint32_t cookie;
    uint8_t  reservedCacheAlignment[IG_IA_CACHE_LINE_SIZE - sizeof(uint32_t)];
};
// Ensure that each Submissions retired memory is padded to a multiple of IG_IA_CACHE_LINE_SIZE
static_assert((sizeof(IGSubmissionsRetired) % IG_IA_CACHE_LINE_SIZE) == 0, "IGSubmissionsRetired size mismatch");

// The IGGucCommPage is a shared resource in Global GTT for providing
// initialization and control/configuration information from the Host to the GuC
// and also for providing useful information from the GuC back to the host.
// Currently, the submissions retired are the only information communicated from
// GuC to host.
struct IGGucCommPage
{
    IGGucCtrl            GucCtrl;
    IGSubmissionsRetired csSubmissionsRetired[kIGMaxHwEngines];
    uint32_t             csLiveMask;
    uint8_t              reservedCacheAlignmentAfterCsLiveMask[IG_IA_CACHE_LINE_SIZE - sizeof(uint32_t)];
    uint32_t             csBlockedMask;
    uint8_t              reservedCacheAlignmentAfterBlockedCSMask[IG_IA_CACHE_LINE_SIZE - sizeof(uint32_t)];
    uint32_t             doorbellReceiveCount;
    uint8_t              reservedCacheAlignmentAfterDoorbellReceiveCount[IG_IA_CACHE_LINE_SIZE - sizeof(uint32_t)];
    uint32_t             fDoorbellOffset[kIGNumHwCsTypes];
    uint8_t              dbPadding[IG_IA_CACHE_LINE_SIZE - sizeof(uint32_t) * kIGMaxHwEngines];
};

// Ensure that the csLiveMask field is IA cache-line aligned.
static_assert(0 == (__builtin_offsetof(IGGucCommPage, csLiveMask) % IG_IA_CACHE_LINE_SIZE), "Command streamer live mask misaligned");
static_assert(0 == (__builtin_offsetof(IGGucCommPage, doorbellReceiveCount) % IG_IA_CACHE_LINE_SIZE), "Doorbell receive count misaligned");
static_assert(0 == (__builtin_offsetof(IGGucCommPage, csBlockedMask) % IG_IA_CACHE_LINE_SIZE), "Blocked CS Mask misaligned");
static_assert(0 == (__builtin_offsetof(IGGucCommPage, fDoorbellOffset) % IG_IA_CACHE_LINE_SIZE), "fDoorbellOffset misaligned");
static_assert(IG_PAGE_SIZE >= sizeof(IGGucCommPage), "IGGucCommPage > Page Size");

enum IGGucToHostMessageType : uint32_t
{
    kIGGucToHostGpaToHpaXlationError                = IG_BITFIELD32_BIT(0),
    kIGGucToHostException                           = IG_BITFIELD32_BIT(1),
    kIGGucToHostAssertion                           = IG_BITFIELD32_BIT(2),
    kIGGucToHostUnexpectedCsbEntry                  = IG_BITFIELD32_BIT(3),

    kIGGucToHostFlushPerfLogBuffer                  = IG_BITFIELD32_BIT(4),
    kIGGucToHostFlushPrintfLogBuffer                = IG_BITFIELD32_BIT(5),
    kIGGucToHostPerfLogThreshholdCrossed            = IG_BITFIELD32_BIT(6)
};



/*!
 * @abstract Host-to-GuC Message Type
 */
enum IGHostToGucMessageType : uint8_t
{
    kIGHostToGucActionNoop                          = 0x00,

    // Actions
    kIGHostToGucActionRequestInitDone               = 0x01,
    kIGHostToGucActionSystemWillSleep               = 0x02,
    kIGHostToGucActionSystemDidWake                 = 0x03,
    kIGHostToGucActionGetPmStatsSnapshot            = 0x04,
    kIGHostToGucActionBindWorkQueue                 = 0x05,
    kIGHostToGucActionUnbindWorkQueue               = 0x06,
    kIGHostToGucActionSetPowerManagement            = 0x07,
    kIGHostToGucActionWaitForIdle                   = 0x08,
    kIGHostToGucActionPing                          = 0x09,
    kIGHostToGucActionDumpStatus                    = 0x0A,
    kIGHostToGucActionSetPerfLogEnable              = 0x0B,
    kIGHostToGucActionGpuResetPrepare               = 0x0C,
    kIGHostToGucActionGpuResetComplete              = 0x0D,
    kIGHostToGucActionPause                         = 0x0E,
    kIGHostToGucActionResume                        = 0x0F,
    kIGHostToGucActionValidateHucKernels            = 0x10,
    kIGHostToGucActionUpdateConfig                  = 0x11,
    kIGHostToGucActionSignalDisplayStampUpdate      = 0x12,
    kIGHostToGucActionGetActiveContexts             = 0x13,

    // Masks
    kIGHostToGucActionTypeMask                      = 0x7F,
    kIGHostToGucActionResponseTypeMask              = 0x80,

    // Special response(s)
    kIGHostToGucActionResponseInitDone              = (kIGHostToGucActionResponseTypeMask | kIGHostToGucActionRequestInitDone)
};

/*!
 * @abstract Host-to-GuC Message Header.
 */
struct IGHostToGucMessageHeader
{
    union
    {
        struct
        {
            IGHostToGucMessageType  MessageType;
            uint8_t                 MessageLength;  // Playload size. Excludes DW0.
            uint16_t                MessagePreload;
        };

        uint32_t DW0;
    };

    IGHostToGucMessageHeader() = delete;

    IGHostToGucMessageHeader(uint32_t dword)
        : DW0{dword}
    {
        // void
    }

    IGHostToGucMessageHeader(IGHostToGucMessageType type, uint8_t length, uint16_t preload)
        : MessageType{type}
        , MessageLength{length}
        , MessagePreload{preload}
    {
        // void
    }
};

static_assert(sizeof(IGHostToGucMessageHeader) == sizeof(uint32_t), "Host-to-GuC message header size mismatch");

/*!
 * @abstract This message is posted by the host to GFXREG_SOFT_SCRATCH_n, where n: [0, 15)
 */
struct IGHostToGucMessage
{
    enum
    {
        kIGMaxNumDwords     = 16,
        kIGMaxPayloadLength = kIGMaxNumDwords - 2 // reserve 1 dword for the header, 1 for async GuC-To-Host message.
    };

    IGHostToGucMessageHeader header;
    uint32_t                 payload[kIGMaxPayloadLength];

    IGHostToGucMessage() = delete;

    IGHostToGucMessage(uint32_t dword)
        : header{dword}
        , payload{0}
    {
        // void
    }

    IGHostToGucMessage(IGHostToGucMessageType type, uint8_t length = 0, uint16_t preload = 0)
        : header{type, length, preload}
        , payload{0}
    {
        // void
    }

    operator unsigned int() const
    {
        return header.DW0;
    }

    IGHostToGucMessage & operator=(uint32_t dword)
    {
        header.DW0 = dword;
        return *this;
    }

    /*!
     * @abstract Gets the message type.
     */
    IGHostToGucMessageType getType() const
    {
       return header.MessageType;
    }

    /*!
     * @abstract Sets the message type.
     */
    void setType(IGHostToGucMessageType type)
    {
        header.MessageType = type;
    }

    /*!
     * @abstract Sets the ACK bit on the message type.
     */
    void translateToActionResponse()
    {
        *reinterpret_cast<uint8_t*>(&header.MessageType) |= kIGHostToGucActionResponseTypeMask;
    }

    /*!
     * @abstract Gets the total message length (including the header) in num dwords.
     */
    uint8_t getLength() const
    {
        return header.MessageLength + 1; // length == dword count == payload + header
    }

    /*!
     * @abstract Gets the message preload.
     *
     * @discussion
     * The message preload may be used to reduce the amount of scratch registers required
     * to send payload information to the GuC.
     */
    uint16_t getPreload() const
    {
        return header.MessagePreload;
    }

    /*!
     * @abstract Gets a mutable pointer to the message preload.
     *
     * @discussion
     * The message preload may be used to reduce the amount of scratch registers required
     * to send payload information to the GuC.
     */
    uint16_t * getMutablePreload()
    {
        return &header.MessagePreload;
    }
};

struct IGHostToGucActionRequestInitDone : public IGHostToGucMessage
{
    IGHostToGucActionRequestInitDone()
        : IGHostToGucMessage{kIGHostToGucActionRequestInitDone, 0, 0}
    {
        // void
    }
};

struct IGHostToGucActionSystemWillSleep : public IGHostToGucMessage
{
    IGHostToGucActionSystemWillSleep()
        : IGHostToGucMessage(kIGHostToGucActionSystemWillSleep, 0, 0)
    {
        // void
    }
};

struct IGHostToGucActionSystemDidWake : public IGHostToGucMessage
{
    IGHostToGucActionSystemDidWake()
        : IGHostToGucMessage(kIGHostToGucActionSystemDidWake, 0, 0)
    {
        // void
    }
};

struct IGHostToGucActionGetPmStatsSnapshot : public IGHostToGucMessage
{
    IGHostToGucActionGetPmStatsSnapshot()
        : IGHostToGucMessage(kIGHostToGucActionGetPmStatsSnapshot, 0, 0)
    {
        // void
    }
};

struct IGHostToGucActionWorkQueueBase : public IGHostToGucMessage
{
    IGHostToGucActionWorkQueueBase(IGHostToGucMessageType msgType, uint16_t workQueueIndex)
        : IGHostToGucMessage(msgType, 0, workQueueIndex)
    {
        // void
    }

    uint16_t getWorkQueueIndex() const
    {
        return getPreload();
    }
};

struct IGHostToGucActionBindWorkQueue : public IGHostToGucActionWorkQueueBase
{
    IGHostToGucActionBindWorkQueue(uint16_t workQueueIndex)
        : IGHostToGucActionWorkQueueBase(kIGHostToGucActionBindWorkQueue, workQueueIndex)
    {
        // void
    }

};

struct IGHostToGucActionUnbindWorkQueue : public IGHostToGucActionWorkQueueBase
{
    IGHostToGucActionUnbindWorkQueue(uint16_t workQueueIndex)
        : IGHostToGucActionWorkQueueBase(kIGHostToGucActionUnbindWorkQueue, workQueueIndex)
    {
        // void
    }
};

struct IGHostToGucActionWaitForIdle : public IGHostToGucMessage
{
    IGHostToGucActionWaitForIdle()
        : IGHostToGucMessage{kIGHostToGucActionWaitForIdle, 0, 0}
    {
        // void
    }
};

struct IGHostToGucActionPing : public IGHostToGucMessage
{
    IGHostToGucActionPing()
        : IGHostToGucMessage{kIGHostToGucActionPing, 0, 0}
    {
        // void
    }
};

struct IGHostToGucActionDumpStatus : public IGHostToGucMessage
{
    IGHostToGucActionDumpStatus()
        : IGHostToGucMessage{kIGHostToGucActionDumpStatus, 0, 0}
    {
        // void
    }
};

struct IGHostToGucActionSetPerfLogEnable : public IGHostToGucMessage
{
    IGHostToGucActionSetPerfLogEnable(uint16_t enable)
        : IGHostToGucMessage{kIGHostToGucActionSetPerfLogEnable, 0, enable}
    {
        // void
    }
};

struct IGHostToGucActionSetPowerManagement : public IGHostToGucMessage
{
    IGHostToGucActionSetPowerManagement()
        : IGHostToGucMessage{kIGHostToGucActionSetPowerManagement, 0, 0}
    {
        // void
    }
};

struct IGHostToGucActionGpuResetPrepare : public IGHostToGucMessage
{
    IGHostToGucActionGpuResetPrepare(uint16_t commandStreamer)
        : IGHostToGucMessage{kIGHostToGucActionGpuResetPrepare, 0, commandStreamer}
    {
        // void
    }

    IGHwCsType getHwCsType() const
    {
        return static_cast<IGHwCsType>(getPreload());
    }
};

struct IGHostToGucActionGpuResetComplete : public IGHostToGucMessage
{
    IGHostToGucActionGpuResetComplete(uint16_t commandStreamer)
        : IGHostToGucMessage{kIGHostToGucActionGpuResetComplete, 0, commandStreamer}
    {
        // void
    }

    IGHwCsType getHwCsType() const
    {
        return static_cast<IGHwCsType>(getPreload());
    }
};

struct IGHostToGucActionPause : public IGHostToGucMessage
{
    IGHostToGucActionPause()
        : IGHostToGucMessage{kIGHostToGucActionPause, 0, 0}
    {
        // void
    }
};

struct IGHostToGucActionResume : public IGHostToGucMessage
{
    IGHostToGucActionResume()
        : IGHostToGucMessage{kIGHostToGucActionResume, 0, 0}
    {
        // void
    }
};

struct IGHostToGucActionValidateHucKernels : public IGHostToGucMessage
{
    IGHostToGucActionValidateHucKernels()
        : IGHostToGucMessage{kIGHostToGucActionValidateHucKernels, 0, 0 }
    {
        // void
    }
};

struct IGHostToGucActionUpdateConfig : public IGHostToGucMessage
{
    IGHostToGucActionUpdateConfig()
        : IGHostToGucMessage{kIGHostToGucActionUpdateConfig, 0, 0}
    {
        // void
    }
};

struct IGHostToGucActionSignalDisplayStampUpdate : public IGHostToGucMessage
{
    IGHostToGucActionSignalDisplayStampUpdate(uint16_t stampIndex)
        : IGHostToGucMessage{kIGHostToGucActionSignalDisplayStampUpdate, 0, stampIndex}
    {
        // void
    }
};

struct IGHostToGucActionGetActiveContexts : public IGHostToGucMessage
{
    IGHostToGucActionGetActiveContexts(uint16_t cs)
        : IGHostToGucMessage{kIGHostToGucActionGetActiveContexts, 0, cs}
    {
        // void
    }
};


#pragma pack(pop)

#endif // IOFB

#pragma pack(push, 1)



struct IGUkPmMessagePayloadHeader
{
    union
    {
        struct
        {
            uint32_t UseGFM                 : IG_BITFIELD_BIT  (      0 );
            uint32_t UpdatePPFreq           : IG_BITFIELD_RANGE(  4,  1 );  // IGSchedPriorityMask
            uint32_t ChangeSlice            : IG_BITFIELD_BIT  (      5 );
            uint32_t ApplyThermalThrottle   : IG_BITFIELD_BIT  (      6 );
            uint32_t UpdateThresholds       : IG_BITFIELD_BIT  (      7 );
            uint32_t UpdateTimer            : IG_BITFIELD_BIT  (      8 );
            uint32_t SetCoarsePowerGating   : IG_BITFIELD_BIT  (      9 );
            uint32_t Reserved               : IG_BITFIELD_RANGE( 31, 10 );
        };

        uint32_t Value;
    };
};

static_assert(sizeof(IGUkPmMessagePayloadHeader) == sizeof(uint32_t), "Power management message header size mismatch");



struct IGUkPmPrioritySliceUnsliceFreqAndPolicy
{
    union
    {
        struct
        {
             uint32_t Policy                 : IG_BITFIELD_RANGE(  3,   0 );
             uint32_t Reserved4_13           : IG_BITFIELD_RANGE( 13,   4 );
             uint32_t SliceFreq              : IG_BITFIELD_RANGE( 22,  14 );
             uint32_t UnsliceFreq            : IG_BITFIELD_RANGE( 31,  23 );
        };

        uint32_t Value;
    };

    IGUkPmPrioritySliceUnsliceFreqAndPolicy(uint32_t dword = 0)
        : Value(dword)
    {
        // void
    }

    IGUkPmPrioritySliceUnsliceFreqAndPolicy& operator=(uint32_t dword)
    {
        Value = dword;
        return *this;
    }

    IGUkPmPrioritySliceUnsliceFreqAndPolicy& operator=(const IGUkPmPrioritySliceUnsliceFreqAndPolicy & rhs)
    {
        return IGUkPmPrioritySliceUnsliceFreqAndPolicy::operator=(rhs.Value);
    }
};

static_assert(sizeof(IGUkPmPrioritySliceUnsliceFreqAndPolicy) == sizeof(uint32_t), "Priority slice/unslice frequency and policy size mismatch");

typedef struct IGUkPmPrioritySliceUnsliceFreqAndPolicy SGfxRpFreqNormal;



struct IGUkPmGfxGtConfig
{
    union
    {
        struct
        {
            uint32_t GfxGtType              : IG_BITFIELD_RANGE(  2,  0 ); // IGGfxGtType
            uint32_t Reserved3_7            : IG_BITFIELD_RANGE(  7,  3 );
            uint32_t NumEUs                 : IG_BITFIELD_RANGE( 15,  8 );
            uint32_t NumSubSlices           : IG_BITFIELD_RANGE( 18, 16 );
            uint32_t Reserved19_31          : IG_BITFIELD_RANGE( 31, 19 );
        };

        uint32_t Value;
    };
};

static_assert(sizeof(IGUkPmGfxGtConfig) == sizeof(uint32_t), "Power management GFX GT config size mismatch");



struct IGUkPmCoarsePowerGatingConfig
{
    union
    {
        struct
        {
            uint32_t RenderPowerGateHysteresis    : IG_BITFIELD_RANGE( 23,  0 );
            uint32_t Reserved0_24_29              : IG_BITFIELD_RANGE( 29, 24 );
            uint32_t MediaSamplerPowerGateEnable  : IG_BITFIELD_BIT  (     30 );
            uint32_t RenderPowerGateEnable        : IG_BITFIELD_BIT  (     31 );
        };

        uint32_t DW0;
    };

    union
    {
        struct
        {
            uint32_t MediaPowerGateHysteresis     : IG_BITFIELD_RANGE( 23,  0 );
            uint32_t Reserved1_24_30              : IG_BITFIELD_RANGE( 30, 24 );
            uint32_t MediaPowerGateEnable         : IG_BITFIELD_BIT  (     31 );
        };

        uint32_t DW1;
    };

    union
    {
        struct
        {
            uint32_t RenderC6_WakeLimit           : IG_BITFIELD_RANGE( 15,  0 );
            uint32_t GlobalRC6_WakeLimit          : IG_BITFIELD_RANGE( 31, 16 );
        };

        uint32_t DW2;
    };

    union
    {
        struct
        {
            uint32_t MediaC6_WakeLimit            : IG_BITFIELD_RANGE( 15,  0 );
            uint32_t Reserved2_16_31              : IG_BITFIELD_RANGE( 31, 16 );
        };

        uint32_t DW3;
    };
};

static_assert(sizeof(IGUkPmCoarsePowerGatingConfig) == sizeof(uint32_t) * 4, "Power management CPG config size mismatch");



struct IGUkPmThermalThrottle
{
    union
    {
        struct
        {
            uint32_t PercentHigh            : IG_BITFIELD_RANGE(  7,  0 );  // [0, 100]
            uint32_t PercentNormalHigh      : IG_BITFIELD_RANGE( 15,  8 );  // [0, 100]
            uint32_t PercentNormal          : IG_BITFIELD_RANGE( 23, 16 );  // [0, 100]
            uint32_t PercentLow             : IG_BITFIELD_RANGE( 31, 24 );  // [0, 100]
        };

        uint32_t DW0;
    };

    union
    {
        struct
        {
            uint32_t PriorityMask           : IG_BITFIELD_RANGE(  3,  0 );  // IGSchedPriorityMask
            uint32_t Reserved4_31           : IG_BITFIELD_RANGE( 31,  4 );
        };

        uint32_t DW1;
    };
};

static_assert(sizeof(IGUkPmThermalThrottle) == 2 * sizeof(uint32_t), "Power management priority percentage size mismatch");



/*!
 * @abstract Power Management Threshold
 *
 * @discussion
 * All priorities share the same UP/DOWN thresholds in GFM, that is, <i>PriorityMask</i> is ignored (in GFM).
 */
struct IGUkPmThreshold
{
    union
    {
        struct
        {
            uint32_t PriorityMask           : IG_BITFIELD_RANGE(  3,  0 );  // IGSchedPriorityMask
            uint32_t ThresholdGlobal        : IG_BITFIELD_RANGE( 11,  4 );
            uint32_t Reserve11_31           : IG_BITFIELD_RANGE( 31, 12 );
        };

        uint32_t DW0;
    };

    union
    {
        struct
        {
            uint32_t ThresholdHigh          : IG_BITFIELD_RANGE(  7,  0 );
            uint32_t ThresholdNormalHigh    : IG_BITFIELD_RANGE( 15,  8 );
            uint32_t ThresholdNormal        : IG_BITFIELD_RANGE( 23, 16 );
            uint32_t ThresholdLow           : IG_BITFIELD_RANGE( 31, 24 );
        };

        uint32_t DW1;
    };
};

static_assert(sizeof(IGUkPmThreshold) == (2 * sizeof(uint32_t)), "Power management priority percentage size mismatch");



struct IGUkPmMessagePayload
{
    // DW0
    IGUkPmMessagePayloadHeader                      Header;

    // DW1-4 (GFM/PPFM)
    union
    {
        struct
        {
            IGUkPmPrioritySliceUnsliceFreqAndPolicy PrioritySliceUnsliceFreqAndPolicy;
            uint32_t                                Reserved1_3[3];
        } GFM;

        struct
        {
            IGUkPmPrioritySliceUnsliceFreqAndPolicy PrioritySliceUnsliceFreqAndPolicy[kIGNumSchedPriorities];
        } PPFM;
    };

    // DW5
    IGUkPmGfxGtConfig                               GfxGtConfig;

    // DW6-7
    IGUkPmThermalThrottle                           ThermalThrottle;

    // DW8-9
    IGUkPmThreshold                                 ThresholdUp;

    // DW10-11
    IGUkPmThreshold                                 ThresholdDown;

    // DW12
    uint32_t                                        PmTimerUS;

    // DW13-16
    IGUkPmCoarsePowerGatingConfig                   CoarsePowerGatingConfig;

    // DW17-31
    uint32_t                                        Reserved[15];

    IGUkPmMessagePayload() {}
};

static_assert(sizeof(IGUkPmMessagePayload) == (32 * sizeof(uint32_t)), "Power management message payload size mismatch");



struct IGUkPmStatistics
{
    union
    {
        struct
        {
            uint32_t Priority                   : IG_BITFIELD_RANGE(  3,  0 );
            uint32_t UnusedPadAfterPriority     : IG_BITFIELD_RANGE(  7,  4 );
            uint32_t AverageFrequency           : IG_BITFIELD_RANGE( 15,  8 );
            uint32_t UnusedPadAfterAvgFreq      : IG_BITFIELD_RANGE( 23, 16 );
            uint32_t GlobalUtilization          : IG_BITFIELD_RANGE( 31, 24 );
        };

        uint32_t DW0;
    };

    union
    {
        struct
        {
            uint32_t HighPriorityUtilization       : IG_BITFIELD_RANGE(  7,  0 );
            uint32_t NormalHighPriorityUtilization : IG_BITFIELD_RANGE( 15,  8 );
            uint32_t NormalPriorityUtilization     : IG_BITFIELD_RANGE( 23, 16 );
            uint32_t LowPriorityUtilization        : IG_BITFIELD_RANGE( 31, 24 );
        };

        uint32_t DW1;
    };

    uint32_t CounterData[14];
};

static_assert(sizeof(IGUkPmStatistics) == IG_IA_CACHE_LINE_SIZE, "Power management statistics size mismatch");



enum IGUkPmUtilStatBufType
{
    kIGUkPmUtilStatBufTypeGlobal     = 0,
    kIGUkPmUtilStatBufTypeHigh       = 1,
    kIGUkPmUtilStatBufTypeNormalHigh = 2,
    kIGUkPmUtilStatBufTypeNormal     = 3,
    kIGUkPmUtilStatBufTypeLow        = 4
};

enum IGUkPmUtilStatBufThreshold
{
    kIGUkPmUtilStatBufThresholdDown = 0,
    kIGUkPmUtilStatBufThresholdUp   = 1
};

struct IGUkPmUtilStatBufElement
{
    union
    {
        struct
        {
            uint32_t Type                       : IG_BITFIELD_RANGE(  1,  0 ); // IGUkPmUtilStatBufType
            uint32_t Threshold                  : IG_BITFIELD_BIT  (      2 ); // IGUkPmUtilStatBufThreshold
            uint32_t Reserved3                  : IG_BITFIELD_BIT  (      3 );
            uint32_t PmStatsBufferIndex         : IG_BITFIELD_RANGE(  7,  4 );
            uint32_t Reserved8_31               : IG_BITFIELD_RANGE( 31,  8 );
        };

        uint32_t DW0;
    };

    uint32_t Reserved[15];
};

static_assert(sizeof(IGUkPmUtilStatBufElement) == IG_IA_CACHE_LINE_SIZE, "PM utilization status buffer element size mismatch");



struct IGUkPmGfxAsyncSliceRegister
{
    union
    {
        struct
        {
            uint32_t AsyncSliceCount    : IG_BITFIELD_RANGE(  3,  0);
            uint32_t AsyncEUMax         : IG_BITFIELD_RANGE(  7,  4);
            uint32_t AsyncSubSliceCount : IG_BITFIELD_RANGE( 10,  8);
            uint32_t                    : IG_BITFIELD_RANGE( 31, 11);
        };

        uint32_t Value;
    };
};

static_assert(sizeof(IGUkPmGfxAsyncSliceRegister) == sizeof(uint32_t), "AsyncSlice config size mismatch");



/*!
 * @abstract Power management status page
 *
 * @discussion
 * <h2>Intialization</h2>
 * The power management status page is allocated by the accelerator and used throughout the
 * lifetime of the system to share power management related data between the host drivers
 * and micro-controller system software.  Once allocated, it is wired down and remains so
 * throughout the lifetime of the system.
 *
 * After creating the memory, the accelerator shares the page address with the framebuffer
 * driver through the RPC <i>accelWillLoadGuC</i> prior to loading the GuC binary.  The
 * framebuffer driver should cache the address and initialize the first cache line (64-bytes)
 * [<b>HostToGucPmMessagePayload</b>] with the initial power management profile.  During GuC
 * initialization, the GuC software will read this data to establish its baseline power
 * management state.  Also during initialization, the GuC software shall initialize the
 * following fields of the power management status page:
 * <ul>
 *   <li>UtilizationStatusBufferReadPointer: <b>kIGUkPmUtilStatBufRWInitialValue</b></li>
 *   <li>UtilizationStatusBufferWritePointer: <b>kIGUkPmUtilStatBufRWInitialValue</b></li>
 *   <li>PmStatsBufferReadPointer: <b>kIGUkNumPmStatBufRWInitialValue</b></li>
 *   <li>PmStatsBufferWritePointer: <b>kIGUkNumPmStatBufRWInitialValue</b></li>
 * </ul>
 *
 * Note, in the event that the framebuffer driver fails to return success from
 * <i>accelWillLoadGuC</i>, the accelerator shall fill in a default power management profile.
 *
 * Once the accelerator has detected a successful load of the GuC binary it will then issue
 * the RPC <i>accelDidLoadGuC</i>.  The purpose of this is to inform the framebuffer driver
 * that the GuC is up and running, ready to accept and respond to action requests.
 *
 * <h2>Message Passing and Data Sharing</h2>
 * During the normal course of operation, the GuC software shall track global and per-priority
 * utilization and issue an interrupt to indicate UP/DOWN threshold crossings at a frequency
 * of the host driver's chosing [see <i>IGUkPmMessagePayload.PmTimerUS</i>].  SWInterrupt6
 * is used for this purpose.  For each threshold interrupt generated, the GuC software shall
 * write the current power management statistics profile to a circular buffer within the
 * power management status page, <i>PmStatsBuffer</i> and update the write pointer,
 * <i>PmStatsBufferWritePointer</i>.  The GuC software shall also, for each threshold crossing
 * detected, write a utlization status buffer element as well as update the utilization status
 * buffer write pointer, <i>UtilizationStatusBufferWritePointer</i>.  After these two status
 * buffers and their respective write-pointers are updated, the GuC software shall then
 * generate an interrupt (targeted for the host framebuffer driver).
 *
 * The host framebuffer driver, upon receipt of the threshold interrupt, shall
 * &quot;process&quot; the power management statistics buffer <b>and</b> the utilization
 * status buffer, <b>and</b> update the respective read pointers.  The GuC software shall make
 * no effort to avoid trampling unprocessed data.  The buffers should be sized such that
 * proper response to any threshold crossing interrupt prevents this.
 *
 * As there is no implicit memory coherency between host and GuC software, the GuC software
 * shall issue an explicit cache-line flush (of 64-bytes) for each piece of data intended for
 * host consumption.  <b>This is why each data field is 64-bytes in size.</b>  The host must
 * assume that the page is mapped WC or WB, and that the page address contains volatile data
 * fields.  As such, host software may benefit from issuing MFENCE or SFENCE instructions
 * prior to reading back page data as volatile with the understanding that the GuC software
 * has issued CLFUSH instructions to invalidate any cache lines it has written.
 *
 * The framebuffer driver may elect to modify the power management profile at any time.  To do
 * this, it must construct a stack-local IGUkPmMessagePayload object, fill it in, and call the
 * accelerator RPC <i>PMSetParamsCallback</i>.  The callback shall load a copy of the object
 * into the first cache-line of the power management status page,
 * [<i>HostToGucPmMessagePayload</i>], and issue a Host-to-GuC interrupt.  The GuC software,
 * upon receipt of the interrupt, shall then parse the message header and update the relevant
 * fields of the SRAM local power management profile, and take any necessary action
 * accordingly.
 *
 * Host software may also elect to query for a power management statistics snapshot in an
 * ad-hoc fashion.  To do this from the framebuffer driver, a call must be made to the
 * (blocking) accelerator callback <i>PMGetStats</i>.  After the callback returns, the ad-hoc
 * snapshot shall be cached in the power management status page field, <i>Stats</i>.
 */
struct IGUkPmStatusPage
{
    enum
    {
        kIGUkNumPmUtilStatBufElements    = 8,
        kIGUkPmUtilStatBufRWInitialValue = 9,

        kIGUkNumPmStatBufElements        = 4,
        kIGUkNumPmStatBufRWInitialValue  = 5,

        //
        // The residency table covers frequencies of [100, 2200] MHz in 50 MHz steps. The data
        // is stored as a 64-bit unsigned integer value, and the total size of the table is
        // aligned (or expanded) to consume a full IA cache line (64-bytes) to make for clean
        // data store flushing out of the MinuteIA cache.
        //
        // The (requested) frequency to slot index formula is: (requested_frequency / 3) - 2
        //   Example:
        //     requested_frequency   = 850 Mhz        ==> GFXREG_RP_FREQ_NORMAL.UnsliceFreq = 0x33
        //     residency_table_index = (0x33 / 3) - 2 ==> 15
        //
        kIGUkMinResidencyFreqEncoded     = 0x6,     // 100 MHz (= 6 * 16.6666...)
        kIGUkMaxResidencyFreq            = 2200,    // MHz
        kIGUkResidencyFrequencyTableStep = 50,      // MHz
        kIGUkNumResidencyTableEntries    = (((((kIGUkMaxResidencyFreq / kIGUkResidencyFrequencyTableStep) - 2) * sizeof(uint64_t)) + (IG_IA_CACHE_LINE_SIZE - 1)) & ~(IG_IA_CACHE_LINE_SIZE - 1)) / sizeof(uint64_t)
    };

    // Cache line 0:1 - Host-to-GuC data
    IGUkPmMessagePayload                    HostToGucPmMessagePayload;

    // Cache line 2 - power management statistics snapshot (for ad-hoc query).
    IGUkPmStatistics                        PmStatsSnapshot;

    // Cache lines 3:12 - Utilization status buffer
    uint32_t                                UtilizationStatusBufferReadPointer;
    uint32_t                                UnusedPadAfterUtilizationStatusBufferReadPointer[15];

    uint32_t                                UtilizationStatusBufferWritePointer;
    uint32_t                                UnusedPadAfterUtilizationStatusBufferWritePointer[15];

    IGUkPmUtilStatBufElement                UtilizationStatusBuffer[kIGUkNumPmUtilStatBufElements];

    // Cache lines 13:18 - PM stats buffer
    uint32_t                                PmStatsBufferReadPointer;
    uint32_t                                UnusedPadAfterPmStatsBufferReadPointer[15];

    uint32_t                                PmStatsBufferWritePointer;
    uint32_t                                UnusedPadAfterPmStatsBufferWritePointer[15];

    IGUkPmStatistics                        PmStatsBuffer[kIGUkNumPmStatBufElements];

    // Cache lines 19:30 - Slice/Unslice residency
    uint64_t                                SliceResidency[kIGUkNumResidencyTableEntries];
    uint64_t                                UnsliceResidency[kIGUkNumResidencyTableEntries];

    // Unused Space - Could be used for whatever.
    uint32_t                                Reserved[528];
};

// Assert that the PM status page is one 4k page in size.
static_assert(sizeof(IGUkPmStatusPage) == IG_PAGE_SIZE, "Power Management page size mismatch");

// Assert that the fields within the PM status page are cache-line aligned.
static_assert(__builtin_offsetof(IGUkPmStatusPage, PmStatsSnapshot)                     == (IG_IA_CACHE_LINE_SIZE * 2), "PM stats data misaligned");
static_assert(__builtin_offsetof(IGUkPmStatusPage, UtilizationStatusBufferReadPointer)  == (IG_IA_CACHE_LINE_SIZE * 3), "PM util stat buf read pointer misaligned");
static_assert(__builtin_offsetof(IGUkPmStatusPage, UtilizationStatusBufferWritePointer) == (IG_IA_CACHE_LINE_SIZE * 4), "PM util stat buf write pointer misaligned");
static_assert(__builtin_offsetof(IGUkPmStatusPage, UtilizationStatusBuffer)             == (IG_IA_CACHE_LINE_SIZE * 5), "PM util status buffer misaligned");
static_assert(__builtin_offsetof(IGUkPmStatusPage, PmStatsBufferReadPointer)            == (IG_IA_CACHE_LINE_SIZE * 13), "PM statistics buf read pointer misaligned");
static_assert(__builtin_offsetof(IGUkPmStatusPage, PmStatsBufferWritePointer)           == (IG_IA_CACHE_LINE_SIZE * 14), "PM statistics buf write pointer misaligned");
static_assert(__builtin_offsetof(IGUkPmStatusPage, PmStatsBuffer)                       == (IG_IA_CACHE_LINE_SIZE * 15), "PM statistics buffer misaligned");
static_assert(__builtin_offsetof(IGUkPmStatusPage, SliceResidency)                      == (IG_IA_CACHE_LINE_SIZE * 19), "PM slice residency table misaligned");
static_assert(__builtin_offsetof(IGUkPmStatusPage, UnsliceResidency)                    == (IG_IA_CACHE_LINE_SIZE * 25), "PM unslice residency table misaligned");

#pragma pack(pop)



#ifndef DEBUGCLASS

#pragma pack(push, 1)



/*!
 * @abstract A hardware command streamer descriptor.
 *
 * @field type                          The @link IGHwCsType @/link corresponding to this
 *                                        descriptor.
 * @field name                          An ASCII character string representing this
 *                                        descriptor.
 * @field mmioExecListSubmitPort        The ELSP MMIO offset corresponding to the HW CS type
 *                                        of this descriptor.
 * @field mmioExecListSubmitQueue       The EL Submit Queue MMIO offset corresponding to the
 *                                        HW CS type of this descriptor.
 * @field mmioExecListControl           The EL Control Register MMIO offset corresponding to the
 *                                        HW CS type of this descriptor.
 * @field mmioContextStatusPointer      The context status pointer MMIO offset corresponding
 *                                        to the HW CS type of this descriptor.
 * @field mmioContextStatusBuffer       The context status buffer MMIO offset corresponding to
 *                                        the HW CS type of this descriptor.
 * @field mmioGlobalStatusPage          The MMIO offset of Global Hardware Status Page.
 * @field mmioGfxMode                   The MMIO offset of Mode Register.
 * @field mmioResetCtrl                 The MMIO offset of Reset Control Register.
 * @field mmioErrorIdentity             The MMIO offset of Error Identity Register.
 * @field mmioErrorMask                 The MMIO offset of Error Mask Register.
 * @field mmioTimeStamp                 The MMIO offset of CS TimeStamp Register.
 * @field mmioGpr0                      The MMIO offset of CS GPR0.
 * @field fuseMask                      Enable instances available via fuse mask.
 * @field contextSizeBytes              The size (in bytes) of a hardware context for the
 *                                        HW CS type of this descriptor.
 * @field stampIndexRangeMin            The min stamp index that can be allocated for the CS.
 * @field stampIndexRangeMax            The max stamp index that can be allocated for the CS.
 * @field stampIndexRangeSize           The number of stamp indices that can be allocated for the CS.
 * @field contextSwitchInterruptType    <b>KEXT_ONLY</b> The IOFB context switch interrupt
 *                                        type corresponsding to context switch interrupts
 *                                        generated by the HW CS type of this descriptor.
 * @field flushNotifyInterruptType      <b>KEXT_ONLY</b> The IOFB interrupt type corresponding
 *                                        to PIPE_CONTROL (with notify) and MI_FLUSH_DW
 *                                        (with notify) interrupts.
 * @field errorInterruptType            <b>KEXT_ONLY</b> The IOFB CS error interrupt type.
 */
struct IGHwCsDesc
{
    IGHwCsType   type;
    uint32_t     csMask;
    const char * name;
    const char * label;
    uint32_t     mmioExecListSubmitPort;
    uint32_t     mmioExecListSubmitQueue;
    uint32_t     mmioExecListControl;
    uint32_t     mmioExecListStatus;
    uint32_t     mmioContextStatusPointer;
    uint32_t     mmioContextStatusBuffer;
    uint32_t     mmioContextStatusPort;
    uint32_t     mmioContextStatusFifoStatus;
    uint32_t     mmioGlobalStatusPage;
    uint32_t     mmioGfxMode;
    uint32_t     mmioResetCtrl;
    uint32_t     mmioErrorIdentity;
    uint32_t     mmioErrorMask;
    uint32_t     mmioTimeStamp;
    uint32_t     mmioGpr0;
    uint32_t     fuseMask;
    uint32_t     contextSizeBytes;
    int32_t      stampIndexRangeMin;
    int32_t      stampIndexRangeMax;
    int32_t      stampIndexRangeSize;
#if !defined(GUC_UKERNEL_BUILD)
    IOSelect     contextSwitchInterruptType;
    IOSelect     flushNotifyInterruptType;
    IOSelect     errorInterruptType;
    uint32_t     mmioForcewakeReq;
    uint32_t     mmioForcewakeAck;
#endif
};



/*!
 * @abstract THE way to reference hardware command streamer descriptors.
 */
typedef const IGHwCsDesc * const IGHwCsDesc_t;



/*!
 * @abstract A common, globally referenced set of hardware command streamer descriptors.
 */
static const IGHwCsDesc kIGHwCsDesc[] =
{
    IG_FOREACH_HW_CS_DESC(IG_DEFINE_HW_CS_DESC_INITIALIZER)
};

static_assert(IG_ARRAY_COUNT(kIGHwCsDesc) == kIGNumHwCsTypes, "HW command streamer descriptors count mismatch");



struct IGDoorbell
{
    union
    {
        struct
        {
            IGDoorbellStatus    Status;
            uint32_t            Cookie;
        };

        uint64_t QW0;
    };

    IGDoorbell(IGDoorbellStatus status, uint32_t cookie)
        : Status{status}
        , Cookie{cookie}
    {
    }

    IGDoorbell(uint64_t val)
        : QW0(val)
    {
    }
};

static_assert(sizeof(IGDoorbell) == sizeof(uint64_t), "Doorbell size mismatch");



// ******* DO ___NOT___ MODIFY THIS STRUCT UNLESS YOU KNOW __EXACTLY__ WHAT YOU ARE DOING! ******* //
struct IGUkContextShared
{
    union
    {
        struct
        {
            uint64_t        ContextId;
            uint64_t        ContextDescriptorQW;
        };
        uint32_t CL0[4];
    };

    union
    {
        struct
        {
            IGVirtualAddress64 ExecListContextGpuVirtualAddress;
            int32_t            StampIndex;
            uint32_t           badWQItems;
        };
        uint32_t CL1[4];
    };

    union
    {
        struct
        {
            uint32_t        DequeueCount;
            int32_t         SubmitCount;
            int32_t         WaitCount;
            IGSchedPriority SchedPriority;
            struct
            {
                uint8_t     DisableCoalescing : 1;
                uint8_t                       : 7;
            }               SchedFlags;
            uint8_t         pad2;
        };
        uint32_t CL2[4];
    };

    union
    {
        struct
        {
            uint32_t        LastReadContextTimestamp;
            uint32_t        PreemptCount;
            uint32_t        CumulativeRuntime;
            uint32_t        pad3;
        };
        uint32_t CL3[4];
    };

    union
    {
        struct
        {
            uint32_t        EnqueueCount;
            uint32_t        pad4[3];
        };
        uint32_t CL4[4];
    };

    uint32_t pad5[1004];
};

static_assert(sizeof(IGUkContextShared) == IG_PAGE_SIZE, "uKernel context shared size mismatch");



struct IGUkWorkItem
{
    IGVirtualAddress64 ukCtxSharedOffset;
    uint32_t           tail;
    uint32_t           rpnswreq;
    uint32_t           stamp;

    union
    {
        struct
        {
            uint32_t valid                  : IG_BITFIELD_BIT  (      0 );
            uint32_t svalid                 : IG_BITFIELD_BIT  (      1 );
            uint32_t has_barrier            : IG_BITFIELD_BIT  (      2 );
            uint32_t itemTimeIndex          : IG_BITFIELD_BIT  (      3 ); // This is at index 3 on purpose to avoid shifting when used
            uint32_t gpu_reset_invalidate   : IG_BITFIELD_BIT  (      4 );
            uint32_t no_preempt             : IG_BITFIELD_BIT  (      5 );
            uint32_t unused6_7              : IG_BITFIELD_RANGE(  7,  6 );
            uint32_t hcs_type               : IG_BITFIELD_RANGE( 15,  8 ); // IGHwCsType
            uint32_t priority               : IG_BITFIELD_RANGE( 20, 16 );
            uint32_t retry_credits          : IG_BITFIELD_RANGE( 22, 21 );
            uint32_t unused21_31            : IG_BITFIELD_RANGE( 31, 23 );
        };
        uint32_t all;
    } flags;

    uint32_t hostPtrRefCount;
    uint64_t hostPtr;
    uint64_t itemTimes[2];
    uint32_t itemRuntime;
    uint32_t headUpdate;
    uint32_t pad;

    IGUkWorkItem()
    {
        Invalidate();
    }

    void Invalidate()
    {
        ukCtxSharedOffset = 0;
        tail              = 0;
        rpnswreq          = 0;
        stamp             = 0;
        flags.all         = 0;
        flags.priority    = 0x1f;
        hostPtr           = 0;
        hostPtrRefCount   = 0;
        itemTimes[0]      = 0;
        itemTimes[1]      = 0;
        itemRuntime       = 0;
        headUpdate        = 0;
    }

    bool isValid() const
    {
        return (IG_TRUE == flags.valid);
    }

    bool Invalidated() const
    {
        return !isValid();
    }

    void copyFrom(volatile IGUkWorkItem * vc) volatile
    {
        ukCtxSharedOffset = vc->ukCtxSharedOffset;
        tail              = vc->tail;
        rpnswreq          = vc->rpnswreq;
        stamp             = vc->stamp;
        flags.all         = vc->flags.all;
        hostPtr           = vc->hostPtr;
        itemTimes[0]      = vc->itemTimes[0];
        itemTimes[1]      = vc->itemTimes[1];
        hostPtrRefCount   = vc->hostPtrRefCount;
        itemRuntime       = vc->itemRuntime;
        headUpdate        = vc->headUpdate;
    }
};

static_assert(sizeof(IGUkWorkItem) == IG_IA_CACHE_LINE_SIZE, "Work item size mismatch");



struct IGUkWaitItem
{
    enum
    {
        kMaxRetryCredits = 2
    };

    union
    {
        IGUkWorkItem           *workItem;
        uint64_t                ptrSize;
    };
    struct
    {
        uint32_t                semIndex :     IG_BITFIELD_RANGE( 15, 0); // Wait for stamp
        uint32_t                unusedOldWa :  IG_BITFIELD_BIT  (    16);
        uint32_t                hitSemaphore : IG_BITFIELD_BIT  (    17);
        uint32_t                preempted :    IG_BITFIELD_BIT  (    18);
        uint32_t                unused :       IG_BITFIELD_RANGE( 31,19);
    };
    uint32_t semValue; // Wait for stamp value

    IGUkWaitItem()
    {
        Invalidate();
    }

    void Invalidate()
    {
        ptrSize      = 0;
        workItem     = nullptr;
        semIndex     = 0xFFFF;
        semValue     = 0xFFFFFFFF;
        hitSemaphore = false;
        preempted    = false;
    }
};

static_assert(sizeof(IGUkWaitItem) == IG_MIA_CACHE_LINE_SIZE, "Wait item size mismatch" );


struct IGUkKdebugEntry
{
    uint64_t timeStamp;
    uint32_t code;
    uint32_t data[3];
    uint32_t pad[10];
};

static_assert(sizeof(IGUkKdebugEntry) == 64, "Kdebug entry should be 64-bytes");



struct IGUkAssertionEntry
{
    uint32_t line;
    char     func[128];
    char     file[124];
};

static_assert(sizeof(IGUkAssertionEntry) == 4*IG_IA_CACHE_LINE_SIZE, "IGUkAssertionEntry entry should be 256-bytes");


struct IGUkWorkQueueSharedParams
{
    union
    {
        struct
        {
            IGVirtualAddress32 fGpuVirtualAddressLDW;
            IGVirtualAddress32 fGpuVirtualAddressUDW;
        };
        IGVirtualAddress64 fGpuVirtualAddress;
    };

    volatile int16_t fRefCount;
    int16_t          padAfterRefCount[3];
};

static_assert(sizeof(IGUkWorkQueueSharedParams) == (sizeof(uint64_t) << 1), "work queue shared params size mismatch");



/*!
 * @abstract Work Queue Descriptors that are shared between Host and GuC.
 *
 * @discussion
 * There is one descriptor per channel (stamp index).  The memory for the descriptors is
 * allocated and mapped into GGTT during startGraphicsEngine.  The Host maps the pages as WT
 * from the CPU side, but issues a CLFLUSH and MFENCE after writing <i>fTail</i> prior to
 * ringing any associated doorbell anyway.  The work queue FIFOs are fixed-sized and lock-
 * free.  Certain parameters are shared (and/or replicated) within the descriptor to make it
 * possible to safely write from either the producer (accelerator) or consumer (GuC/uKernel)
 * and/or reduce cache line contention.  As a further optimization, the Host and/or uKernel
 * may cache uncommonly read values in their respective local memory and only refresh the
 * relevant local copies with a sampling from the real values when necessary (e.g. Host-
 * side reads of <i>fHead</i>).
 */
struct IGUkWorkQueueDesc
{
    // Written by the uKernel, read (occasionally) by the Host.
    volatile uint32_t           fHead;
    IGUkWorkQueueSharedParams   fSharedParamsUk;        // uKernel copy
    uint32_t                    fCachedTail;            // Used for delayed updates from GuC to Host
    uint32_t                    fDequeuedHead;          // Head for items we've dequeued but they haven't been retired, yet
    uint32_t                    padToAlignUkWriteableCacheLine[9];

    // Written by the Host, read by uKernel
    volatile uint32_t           fTail;
    IGUkWorkQueueSharedParams   fSharedParamsAk;        // Accelerator (kext) copy
    uint32_t                    padAfterSharedParamsAk[11];
};

static_assert(sizeof(IGUkWorkQueueDesc) == 128, "work queue descriptor size mismatch");
static_assert(__builtin_offsetof(IGUkWorkQueueDesc, fTail) == IG_IA_CACHE_LINE_SIZE, "work queue descriptor tail misaligned");



struct IGUkDumpStatus
{
    struct
    {
        uint32_t fActiveCommandStreamers;
        uint32_t fBlockedChannels[4];                   // 32-bit groups, up to 128 possible bits (one per channel index).
    } fMasks;

    struct
    {
        uint32_t fContextIds[2];
        int32_t  fStampIndexes[2];
        uint32_t fStamps[2];
    } fActiveContexts[kIGMaxHwEngines];

    struct
    {
        uint64_t fContextDescriptor;
        uint32_t fTail;
        stamp_t  fStamp;
        int32_t  fWaitForStampIndex;
        stamp_t  fWaitForStamp;
        bool     fPreempted;
        uint32_t fBarrierMask;
    } fBlockedChannels[IG_MAX_STAMP_INDEX];

    struct
    {
        uint32_t faultNumber;
        uint32_t errorCode;
        uint32_t stackUsedBytes;
        uint32_t eip;
        uint32_t espLast;
        uint32_t espCurrent;
        uint32_t eflags;
        uint32_t cs;
        uint32_t eax;
        uint32_t ebx;
        uint32_t ecx;
        uint32_t edx;
        uint32_t esi;
        uint32_t edi;
        uint32_t ebp;
        uint32_t esp;
    } fException;

    union
    {
        struct
        {
            uint32_t fCommandStreamerInfoValid : 1;
            uint32_t fActiveContextInfoValid   : 1;
            uint32_t fBlockedChannelInfoValid  : 1;
            uint32_t fExceptionInfoValid       : 1;
            uint32_t fSRAMDumpValid            : 1;
            uint32_t                           : 27;
        };

        uint32_t u32;
    } fFlags;
};



#pragma pack(pop)

#endif // IOFB



#undef IG_DEFINE_HW_CS_DESC_INITIALIZER
#undef IG_DEFINE_HW_CS_DESC_INITIALIZER_KEXT
#undef IG_DEFINE_HW_CS_DESC_INITIALIZER_COMMON
#undef IG_DECL_HW_CS_TYPE
#undef IG_DECL_HW_CS_MASK
#undef IG_DECL_SCHED_PRI_ENUM
#undef IG_DECL_SCHED_PRI_MASK
#undef IG_DECL_SCHED_PRI_NAME



#endif /* __cplusplus */

#endif // IG_HW_IFC_H
