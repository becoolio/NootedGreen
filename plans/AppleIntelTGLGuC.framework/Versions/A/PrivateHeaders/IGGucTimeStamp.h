#ifndef IG_STAMPS_H
#define IG_STAMPS_H

// Arbitrarily chosen index values into unused parts of stamps page.  EventMachineFast
// supports up to 128 unique stamp indexes, anything >= 128 may be used for general purpose.
enum
{
    kIGTSSemaphoreWaitBaseIndexGuC = IG_MAX_STAMP_INDEX,
    kIGTSPriorityFreqBaseIndexGuC  = (kIGTSSemaphoreWaitBaseIndexGuC + IG_MAX_STAMP_INDEX),
};

// Commands used for writing timestamps (MI_FLUSH, PIPE_CONTROL) can only write to
// QWORD aligned offsets. Use a type with the necessary padding so that if we define
// an array of stamps, they meet the necessary alignment restrictions.
struct IGTimeStamp
{
    stamp_t  stamp;          // last stamp returned from hardware
    uint32_t pad1;
    stamp_t  submitStamp;    // last stamp submitted to the scheduler
    uint32_t pad2[13];
};

static_assert(sizeof(IGTimeStamp) == IG_IA_CACHE_LINE_SIZE, "IGTimeStamp size mismatch");

// BDW WA: Semaphore data (semVal and waitForChannel) must be streamed out to
// memory in order for the scheduler to make good decisions.  The PPHWS is
// supposed to contain a copy of the SEMAPHORE_WAIT event that triggers a
// context switch due to wait-on-semaphore, but we've found that the data
// doesn't land in time for us to read it.  As a result, we end up making poor
// scheduling decisions (and often hanging the GPU because we're waiting on
// an incorrect channel index) as a result.  Intel's suggested workaround is
// to issue MI_FLUSH_DW or PIPE_CONTROL prior to SEMAPHORE_WAIT for this
// information.
struct IGSemaphoreData
{
    union
    {
        struct
        {
            stamp_t  stamp;
            int16_t  stamp_idx;
            int16_t  stamp_sem2_wait;     // WaSemaphoreWaitLiteRestore
        };
        uint64_t qw0;
    };
    uint32_t pad[14];
};

static_assert(sizeof(IGSemaphoreData) == IG_IA_CACHE_LINE_SIZE, "IGSemaphoreData size mismatch");

template <typename T>
inline int32_t stampDiff(const T &a, const T &b)
{
    return (int32_t)(a - b);
}

template <typename T>
inline bool stampLessThan(const T &a, const T &b)
{
    return stampDiff(b, a) > 0;
}

template <typename T>
inline bool stampGreaterThan(const T &a, const T &b)
{
    return stampDiff(a, b) > 0;
}

template <typename T>
inline bool stampLessEqual(const T &a, const T &b)
{
    return stampDiff(b, a) >= 0;
}

template <typename T>
inline bool stampGreaterEqual(const T &a, const T &b)
{
    return stampDiff(a, b) >= 0;
}

#endif /* IG_STAMPS_H */
