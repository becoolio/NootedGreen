#pragma once

#include <cstdarg>
#include <libkern/libkern.h>
#include <sys/socket.h>
#include <sys/types.h>

class IOService;

#define NETLOG(mod, fmt, ...) NETDBG::printf(mod ": " fmt "\n", ##__VA_ARGS__)

class NETDBG {
private:
	static bool enabled;
	static bool ready;
	static bool checkpointsEnabled;
	static bool haltOnFail;
	static bool initialWaitDone;
	static in_addr_t ip_addr;
	static uint32_t port;
	static uint32_t debugLevel;
	static uint32_t initialDelaySeconds;
	static uint32_t timeoutSeconds;
	static uint32_t retryIntervalSeconds;
	static uint32_t checkpointDelaySeconds;

	static bool parseIPv4(const char *host, in_addr_t &outAddr);
	static bool sendBuffer(const char *data, size_t len);

public:
	static void initFromPlistAndBootArgs(IOService *service = nullptr);
	static bool isEnabled() { return enabled; }
	static bool isReady() { return ready; }
	static bool shouldLogLevel(uint32_t level) { return enabled && debugLevel >= level; }

	static void checkpoint(const char *tag);
	static void checkpointf(const char *tag, const char *fmt, ...);
	static void waitBeforeNetInitIfRequested(const char *where);
	static bool tryInitUdp();
	static void send(const char *msg);
	static void sendf(const char *fmt, ...);
	static void delayCheckpointIfRequested(const char *tag);

	static size_t nprint(const char *data, size_t len);
	[[gnu::format(__printf__, 1, 2)]] static size_t printf(const char *fmt, ...);
	[[gnu::format(__printf__, 1, 0)]] static size_t vprintf(const char *fmt, va_list args);
};
