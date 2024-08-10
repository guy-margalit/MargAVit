#define SECOND_IN_MILLISECONDS (1000)

#define MILLISECOND_IN_100_NANOSECONDS (10000)

#define SECOND_IN_100_NANOSECONDS (SECOND_IN_MILLISECONDS * MILLISECOND_IN_100_NANOSECONDS)

#define MINIMUM_ALERT_TIME ({MinimumAlertTimeS} * SECOND_IN_100_NANOSECONDS)

#define MAXIMUM_ALERT_TIME ({MaximumAlertTimeS} * SECOND_IN_100_NANOSECONDS)

#define ALERT_TITLE (L"{AlertTitle}")

#define ALERT_DESCRIPTION (L"{AlertDescription}")

#define HOOK_TYPE {HookType}

#define SET_HOOKS(hooker) \
{Hooks}

#define TIMERS {Timers}

#define TIMERS_COUNT ({TimersCount})

#define ALERT_TIMER_INDEX ({AlertTimerIndex})
