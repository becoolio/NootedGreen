#ifndef IG_UK_TRACE_POINT_IDS_H
#define IG_UK_TRACE_POINT_IDS_H

#define GROUP(DECL,base, BaseName, StrName )            \
    DECL(base+0, BaseName ## RCS, StrName "RCS")        \
    DECL(base+4, BaseName ## VCS, StrName "VCS")        \
    DECL(base+8, BaseName ## BCS, StrName "BCS")        \
    DECL(base+12, BaseName ## VECS, StrName "VECS")     \
    DECL(base+16, BaseName ## VCS2, StrName "CCS")

#define FOREACH_IGUK_KTRACE_ID(DECL) \
    DECL(0x85A70000, Init,                                   "Init"                                  ) \
    DECL(0x85A70004, ExitRC6,                                "ExitRC6"                               ) \
    DECL(0x85A70008, HostIntr,                               "HostIntr"                              ) \
    DECL(0x85A7000C, AratInterrupt,                          "AratInterrupt"                         ) \
    DECL(0x85A70010, CheckFrequencyTimerInterrupt,           "CheckFrequencyTimerInterrupt"          ) \
    DECL(0x85A70014, ChangeFrequencyTimerInterrupt,          "ChangeFrequencyTimerInterrupt"         ) \
    DECL(0x85A70018, ThrottleHighTimerInterrupt,             "ThrottleHighTimerInterrupt"            ) \
    DECL(0x85A7001C, ThrottleNormalHighTimerInterrupt,       "ThrottleNormalHighTimerInterrupt"      ) \
    DECL(0x85A70020, ThrottleNormalTimerInterrupt,           "ThrottleNormalTimerInterrupt"          ) \
    DECL(0x85A70024, ThrottleLowTimerInterrupt,              "ThrottleLowTimerInterrupt"             ) \
    DECL(0x85A70028, BindWorkQueue,                          "BindWorkQueue"                         ) \
    DECL(0x85A7002C, UnbindWorkQueue,                        "UnbindWorkQueue"                       ) \
    DECL(0x85A70030, DoorbellMonitor,                        "DoorbellMonitor"                       ) \
    DECL(0x85A70034, DoorbellRing,                           "DoorbellRing"                          ) \
    DECL(0x85A70038, EngineInterrupt,                        "EngineInterrupt"                       ) \
    DECL(0x85A7003C, ContextSwitchInterrupt,                 "ContextSwitchInterrupt"                ) \
    DECL(0x85A70040, SchedNotify,                            "SchedNotify"                           ) \
    DECL(0x85A70044, UnexpectedContextStatusBufferEntry,     "UnexpectedContextStatusBufferEntry"    ) \
    DECL(0x85A70048, HaltEnter,                              "HaltEnter"                             ) \
    DECL(0x85A7004C, HaltExit,                               "HaltExit"                              ) \
    DECL(0x85A70050, ResetPrepare,                           "ResetPrepare"                          ) \
    DECL(0x85A70054, ResetComplete,                          "ResetComplete"                         ) \
    DECL(0x85A70058, PerfLogOverhead,                        "PerfLogOverhead"                       ) \
    DECL(0x85A7005C, PreemptionTimerInterrupt,               "PreemptionTimerInterrupt"              ) \
    GROUP(DECL,0x85A70060, SchedDispatch,                    "SchedDispath"                          ) \
    GROUP(DECL,0x85A70074, CheckUnblock,                     "CheckUnblock"                          ) \
    GROUP(DECL,0x85A70088, SubmitExecList,                   "SubmitExecList"                        ) \
    GROUP(DECL,0x85A7009C, ProcessContextStatusBuffer,       "ProcessContextStatusBuffer"            ) \
    GROUP(DECL,0x85A700B0, ContextSwitchTypeIdleToActive,    "ContextSwitchTypeIdleToActive"         ) \
    GROUP(DECL,0x85A700C4, ContextSwitchTypePreempted,       "ContextSwitchTypePreempted"            ) \
    GROUP(DECL,0x85A700D8, ContextSwitchTypeElementSwitch,   "ContextSwitchTypeElementSwitch"        ) \
    GROUP(DECL,0x85A700EC, ContextSwitchTypeActiveToIdle,    "ContextSwitchTypeActiveToIdle"         ) \
    GROUP(DECL,0x85A70100, SchedDequeueExecList,             "SchedDequeueExecList"                  ) \
    GROUP(DECL,0x85A70114, SchedEvalWaitQueues,              "SchedEvalWaitQueues"                   ) \
    GROUP(DECL,0x85A70128, SchedEvalWorkQueues,              "SchedEvalWorkQueues"                   ) \
    GROUP(DECL,0x85A7013C, WriteELSP_,                       "WriteELSP_"                            ) \
    GROUP(DECL,0x85A70150, MoveItemToWaitQueueSemaphore,     "MoveItemToWaitQueueSemaphore"          ) \
    GROUP(DECL,0x85A70164, MoveItemToWaitQueuePreempted,     "MoveItemToWaitQueuePreempted"          ) \
    DECL(0x85A70178, RCS_Active,                             "RCS_Active"                            ) \
    DECL(0x85A7017C, VCS0_Active,                            "VCS0_Active"                           ) \
    DECL(0x85A70180, BCS_Active,                             "BCS_Active"                            ) \
    DECL(0x85A70184, VECS_Active,                            "VECS_Active"                           ) \
    DECL(0x85A70188, VCS2_Active,                            "VCS2_Active"                           ) \
    DECL(0x85A7018C, ContextRunTime,                         "ContextRunTime"                        ) \
    DECL(0x85A70190, Debug0,                                 "DEBUG0"                                ) \
    DECL(0x85A70194, Debug1,                                 "DEBUG1"                                ) \
    DECL(0x85A70198, Debug2,                                 "DEBUG2"                                ) \
    DECL(0x85A7019C, Debug3,                                 "DEBUG3"                                ) \
    DECL(0x85A701A0, Debug4,                                 "DEBUG4"                                ) \
    DECL(0x85A701A4, Debug5,                                 "DEBUG5"                                ) \
    DECL(0x85A701A8, Debug6,                                 "DEBUG6"                                ) \
    DECL(0x85A701AC, DisplayInterrupt,                       "DisplayInterrupt"                      ) \
    DECL(0x85A701B0, ResetCredits,                           "ResetCredits"                          ) \
    DECL(0x85A701B4, CreditsStatus,                          "CreditsStatus"                         ) \
    DECL(0x85A701B8, TimeToPreempt,                          "TimeToPreempt"                         ) \
    DECL(0x85A701BC, ContextStampInfo,                       "ContextStampInfo"                      ) \
    DECL(0x85A701C0, GPUUtilization,                         "GPUUtilization"                        ) \
    DECL(0x85A701C4, EventsDropped,                          "EventsDropped"                         ) \
    DECL(0x85A701C8, GPUPPFMUtilization,                     "GPUPPFMUtilization"                    ) \
    DECL(0x85A701CC, GPUFreqSWReq,                           "GPUFreqSWReq"                          ) \
    DECL(0x85A701D0, ThermalStallInterrupt,                  "ThermalStallInterrupt"                 ) \
    DECL(0x85A701D4, ThermalStall,                           "ThermalStall"                          ) \
    DECL(0x85A701D8, ThermalStallRC6,                        "ThermalStallRC6"                       ) \
    DECL(0x85A701DC, ThermalStallPlimitIA,                   "ThermalStallPlimitIA"                  ) \

#define DECL_ENUM_COUNT(ID,A,B) kIGUkTraceIdCount_  ## A,
enum IGUkTraceIdCount : uint32_t
{
    FOREACH_IGUK_KTRACE_ID(DECL_ENUM_COUNT)

    kIGUkTraceId_CountItems
};
#define DECL_ENUM(ID, A, B) kIGUkTraceId_ ## A = ID,
enum IGUkTraceId : uint32_t
{
    FOREACH_IGUK_KTRACE_ID(DECL_ENUM)

    kIGUkTraceId_Max
};
static_assert(((kIGUkTraceId_Max+3) == kIGUkTraceId_Init + kIGUkTraceId_CountItems*4), "Gaps in trace ID numbers!");
#undef DECL_ENUM

#define DECL_NAME(ID, A, B) B,
static const char * kIGUkTraceIdName[] = {
    FOREACH_IGUK_KTRACE_ID(DECL_NAME)
};
#undef DECL_NAME

#endif /* IG_UK_TRACE_POINT_IDS_H */
