#pragma once

#include <stdint.h>

struct IGHwCsDesc;

bool isRcsEngineTraceEnabled();
void dumpRcsEngineActivationState(const char *stage);
void markRcsRegisterTransition(const char *stage);
bool validateRcsDescriptorOrEngineObject(const char *stage, void *object);
void dumpNearbyKnownEngineGlobals(const char *stage);
void dumpContextImageKnownFields(const char *stage, void *context);
void dumpRingMemoryKnownFields(const char *stage, void *ringBuffer);
void decodeErrorGen6(uint32_t value);
