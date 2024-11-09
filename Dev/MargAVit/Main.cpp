#include <map>
#include <intrin.h>
#include <Windows.h>

#include "Common/NtDll.hpp"
#include "Thread/Thread.hpp"
#include "Common/Common.hpp"
#include "Hooks/Hooker/Hooker.hpp"
#include "Synchronization/Timer/Timer.hpp"
#include "DynamicLibrary/DynamicLibrary.hpp"

#define BREAKPOINT_OPCODE (0xCC)

static HANDLE g_alert_waitable = nullptr;
static int64_t g_alert_due_time = 0;
static ThreadUPtr g_alert_thread = nullptr;
static std::wstring g_timer_names[] = TIMERS;
static ThreadUPtr g_fake_timers_thread = nullptr;
static TimerUPtr g_signal_waitables[TIMERS_COUNT];

static
BOOL
WINAPI
read_directory_changes_w_hook(
	decltype(&ReadDirectoryChangesW) pReadDirectoryChangesW,
	HANDLE hDirectory,
	LPVOID lpBuffer,
	DWORD nBufferLength,
	BOOL bWatchSubtree,
	DWORD dwNotifyFilter,
	LPDWORD lpBytesReturned,
	LPOVERLAPPED lpOverlapped,
	LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
)
{
	LARGE_INTEGER large_due_time = { 0 };
	large_due_time.QuadPart = g_alert_due_time;

#ifdef USE_SINGLE_STEPPING_MITIGATION
	// The idea is that if someone were to step over this function,
	// the debugger would set a breakpoint on the return, meaning the return address
	// would point to a breakpoint opcode
	if (*(PBYTE)_ReturnAddress() == BREAKPOINT_OPCODE)
	{
		large_due_time.QuadPart *= 2;
	}
#endif

	(void)SetWaitableTimer(
		g_alert_waitable,
		&large_due_time,
		0,
		nullptr,
		nullptr,
		false);
	
	return pReadDirectoryChangesW(
		hDirectory,
		lpBuffer,
		nBufferLength,
		bWatchSubtree,
		dwNotifyFilter,
		lpBytesReturned,
		lpOverlapped,
		lpCompletionRoutine);
}

static unsigned __stdcall fake_timers_thread_enty_point(void*)
{
	for (size_t i = 0; i < TIMERS_COUNT; ++i)
	{
		if (ALERT_TIMER_INDEX != i)
		{
			g_signal_waitables[i]->set();
		}
	}

	return 0;
}

static unsigned __stdcall alert_thread_entry_point(void*)
{
	const DWORD wait_result = WaitForSingleObject(g_alert_waitable, INFINITE);
	if (WAIT_OBJECT_0 != wait_result)
	{
		return MARGAVITSTATUS_MAIN_ALERT_THREAD_ENTRY_POINT_WAITFORSINGLEOBJECT_FAILED;
	}

	MessageBoxW(nullptr, ALERT_DESCRIPTION, ALERT_TITLE, MB_OK);

	return 0;
}

static int64_t generate_random_number(int64_t min, int64_t max)
{
	UCHAR number_buffer[sizeof(int64_t)] = { 0 };

	if (NTSTATUS_SUCCESS != BCryptGenRandom(nullptr, number_buffer, sizeof(number_buffer), BCRYPT_USE_SYSTEM_PREFERRED_RNG))
	{
		throw MargAVitException(MARGAVITSTATUS_MAIN_GENERATE_RANDOM_NUMBER_BCRYPTGENRANDOM_FAILED);
	}

	return (*(int64_t*)&number_buffer) % (max - min) + min + 1;
}

static int64_t generate_random_due_time()
{
	return -1 * generate_random_number(MINIMUM_ALERT_TIME, MAXIMUM_ALERT_TIME);
}

BOOL
WINAPI
DllMain(
	HINSTANCE,
	DWORD dwReason,
	LPVOID 
)
{
	switch (dwReason)
	{
	case DLL_THREAD_ATTACH:
		break;
	
	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_ATTACH:
		for (size_t i = 0; i < TIMERS_COUNT; ++i)
		{
			g_signal_waitables[i] = std::make_unique<Timer>(g_timer_names[i], false, generate_random_due_time());
		}

		// Put the alert waitable handle in a separate global because it'll be easier to see and track in WinDBG
		g_alert_waitable = g_signal_waitables[ALERT_TIMER_INDEX]->get();
		g_alert_due_time = g_signal_waitables[ALERT_TIMER_INDEX]->get_due_time();
		
		g_alert_thread = std::make_unique<Thread>(alert_thread_entry_point);
		g_fake_timers_thread = std::make_unique<Thread>(fake_timers_thread_enty_point);

		SET_HOOKS(DETACHED_EMPTY_HOOK);
		DETACHED_HOOK("kernel32.dll", ReadDirectoryChangesW, &read_directory_changes_w_hook);

		break;

	case DLL_PROCESS_DETACH:
		break;
	}

	return true;
}
