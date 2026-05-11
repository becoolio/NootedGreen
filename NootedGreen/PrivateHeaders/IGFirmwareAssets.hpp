#pragma once

#include <libkern/libkern.h>

#include "Version.h"
#include "IGGucBinary.h"
#include "IGHucBinary.h"

namespace IGFirmwareAssets {
static constexpr size_t kFirmwareSignatureSize = 0x100;

static_assert(sizeof(__KmGuCBinary) > kFirmwareSignatureSize, "GuC blob must contain CSS signature and firmware");
static_assert(sizeof(__KmHuCBinary) > kFirmwareSignatureSize, "HuC blob must contain CSS signature and firmware");

static const uint8_t *kGuCFull = reinterpret_cast<const uint8_t *>(__KmGuCBinary);
static constexpr size_t kGuCFullSize = sizeof(__KmGuCBinary);
static const uint8_t *kGuCSignature = kGuCFull;
static constexpr size_t kGuCSignatureSize = kFirmwareSignatureSize;
static const uint8_t *kGuCFirmware = kGuCFull + kFirmwareSignatureSize;
static constexpr size_t kGuCFirmwareSize = kGuCFullSize - kFirmwareSignatureSize;

static const uint8_t *kHuCFull = reinterpret_cast<const uint8_t *>(__KmHuCBinary);
static constexpr size_t kHuCFullSize = sizeof(__KmHuCBinary);
static const uint8_t *kHuCSignature = kHuCFull;
static constexpr size_t kHuCSignatureSize = kFirmwareSignatureSize;
static const uint8_t *kHuCFirmware = kHuCFull + kFirmwareSignatureSize;
static constexpr size_t kHuCFirmwareSize = kHuCFullSize - kFirmwareSignatureSize;
}
