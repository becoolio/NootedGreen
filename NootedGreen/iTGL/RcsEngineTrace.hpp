#pragma once

#include <stdint.h>
#include <stddef.h>

struct IGHwCsDesc;

bool isRcsEngineTraceEnabled();
void dumpRcsEngineActivationState(const char *stage);
void markRcsRegisterTransition(const char *stage);
bool validateRcsDescriptorOrEngineObject(const char *stage, void *object);
void dumpNearbyKnownEngineGlobals(const char *stage);
void dumpContextImageKnownFields(const char *stage, void *context);
void dumpRingMemoryKnownFields(const char *stage, void *ringBuffer);
void decodeErrorGen6(uint32_t value);

void dumpIGHardwareContextObject(const char *functionName, void *context);
void dumpIGHardwareRingBufferObject(const char *functionName, void *ringBuffer);
void scanMappedCandidates(const char *owner, void *object, uint32_t objectSize);
void scanContextImageFields(const char *candidateOwnerOffset, void *cpuPtr, uint64_t gpuAddr);
void scanRingMemoryFields(const char *ringInfo, void *cpuPtr, uint64_t gpuAddr, uint32_t ringSize, uint32_t oldTail, uint32_t newTail);
void traceVirtualCallInSubmitToRing(void *ringBuffer);
void scanHwsCandidates(const char *candidateOwnerOffset, void *cpuPtr, uint64_t gpuAddr);
void logBuildMarker();