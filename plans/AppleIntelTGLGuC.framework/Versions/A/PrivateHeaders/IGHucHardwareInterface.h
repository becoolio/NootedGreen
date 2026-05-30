#ifndef IG_HUC_HW_IFC_H
#define IG_HUC_HW_IFC_H


#ifdef __cplusplus

#pragma pack(push, 1)

// Corresponds to HUC_STATUS (0x0D000) bits [15:8] (uKernel/uOS Status).
// If GeneralException is set, then the Exception vector is in bits 23:16
enum IGHucUkStatus : uint32_t
{
    kIGHucUkStatusDefault                 = 0x00,
    kIGHucUkStatusStart                   = 0x01,
    kIGHucUkStatusIDTInitialized          = 0x02,
    kIGHucUkStatusRegionsInitialized      = 0x03,
    kIGHucUkStatusLapicInitialized        = 0x04,
    kIGHucUkStatusReady                   = 0x05,
    kIGHucUkStatusKernelStart             = 0x06,
    kIGHucUkStatusKernelComplete          = 0x07,
    kIGHucUkStatusGeneralException        = 0xF9,

    kIGHucUkStatusExceptionMask           = 0xF8,
    kIGHucUkStatusShift                   = 0x00,
};

#pragma pack(pop)

#endif /* __cplusplus */

#endif // IG_HUC_HW_IFC_H
