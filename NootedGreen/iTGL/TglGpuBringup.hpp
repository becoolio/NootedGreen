#pragma once

#include <stdint.h>

struct TglGpuBringupState {
	bool isTgl9A49 {false};
	bool allow3d {false};
	int schedulerType {0};
	bool mmioReady {false};
	bool forceWakeOk {false};
	bool gtPowered {false};
	bool topologyOk {false};
	bool gucRequested {false};
	bool gucWopcmProgrammed {false};
	bool gucFirmwareLoaded {false};
	bool gucBooted {false};
	bool gucInterruptsEnabled {false};
	bool gucSubmissionReady {false};
	bool rcsResetSeen {false};
	bool rcsRingStarted {false};
	bool rcsContextReady {false};
	bool rcsInterruptsEnabled {false};
	bool rcsFirstSubmitSeen {false};
	bool rcsFirstCompletionSeen {false};
	uint32_t lastGucStatus {0};
	uint32_t lastRcsCtl {0};
	uint32_t lastRcsHead {0};
	uint32_t lastRcsTail {0};
	uint32_t lastErrorGen6 {0};
};

TglGpuBringupState &getTglGpuBringupState();
bool isTglTraceEnabled();
void updateTglGpuBringupStateFromRegisters(const char *stage);
void dumpTglGpuBringupState(const char *stage);
void dumpRcsRegisters(const char *stage);
void dumpGucRegisters(const char *stage);
void dumpGtInterruptRegisters(const char *stage);
