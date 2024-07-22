#include <Windows.h>

#include "Thread/Thread.hpp"
#include "Common/MargAVitException.hpp"
#include "Hooks/DetourHook/DetourHook.hpp"
#include "Synchronization/Timer/Timer.hpp"
#include "Hooks/HookFactory/HookFactory.hpp"
#include "DynamicLibrary/DynamicLibrary.hpp"

#define SECOND_IN_MILLISECONDS (1000)

#define MILLISECOND_IN_100_NANOSECONDS (10000)

#define SECOND_IN_100_NANOSECONDS (SECOND_IN_MILLISECONDS * MILLISECOND_IN_100_NANOSECONDS)

#define MINIMUM_ALERT_TIME (6 * SECOND_IN_100_NANOSECONDS)

#define MAXIMUM_ALERT_TIME (9 * SECOND_IN_100_NANOSECONDS)

#define ALERT_TIMER_NAME (L"GuyGuyTimer")

#define NTSTATUS_SUCCESS (0)

#define PAGE_EXECUTE_ALL (PAGE_EXECUTE | \
						  PAGE_EXECUTE_READ | \
						  PAGE_EXECUTE_READWRITE | \
						  PAGE_EXECUTE_WRITECOPY)

typedef
NTSTATUS
(NTAPI* PFN_NT_ALLOCATE_VIRTUAL_MEMORY)(
	HANDLE ProcessHandle,
	PVOID* BaseAddress,
	ULONG ZeroBits,
	PULONG RegionSize,
	ULONG AllocationType,
	ULONG Protect);

static IHookUPtr g_hook = nullptr;
static ThreadUPtr g_alert_thread = nullptr;
static ISignalWaitableUPtr g_signal_waitable = nullptr;

static
NTSTATUS
NTAPI
nt_allocate_virtual_memory_hook(
	HANDLE ProcessHandle,
	PVOID* BaseAddress,
	ULONG ZeroBits,
	PULONG RegionSize,
	ULONG AllocationType,
	ULONG Protect
)
{
	if (PAGE_EXECUTE_ALL & Protect)
	{
		wprintf_s(L"Tried to allocate non-image executable memory\n");
		g_signal_waitable->set();
	}
	
	return reinterpret_cast<PFN_NT_ALLOCATE_VIRTUAL_MEMORY>(g_hook->get_source())(
		ProcessHandle,
		BaseAddress,
		ZeroBits,
		RegionSize,
		AllocationType,
		Protect);
}

static
inline
void
alert(void)
{
	(void)MessageBoxW(nullptr, L"ALERT", L"Allocated executable virtual memory", MB_OK);
}

static
unsigned
__stdcall
alert_thread_entry_point(
	void*
)
{
	const DWORD wait_result = WaitForSingleObject(g_signal_waitable->get(), INFINITE);
	if (WAIT_OBJECT_0 != wait_result)
	{
		return MARGAVITSTATUS_MAIN_ALERT_THREAD_ENTRY_POINT_WAITFORSINGLEOBJECT_FAILED;
	}

	alert();

	return 0;
}

int64_t
generate_random_number(
	int64_t min,
	int64_t max
)
{
	UCHAR number_buffer[sizeof(int64_t)] = { 0 };

	if (NTSTATUS_SUCCESS != BCryptGenRandom(nullptr, number_buffer, sizeof(number_buffer), BCRYPT_USE_SYSTEM_PREFERRED_RNG))
	{
		throw MargAVitException(MARGAVITSTATUS_MAIN_GENERATE_RANDOM_NUMBER_BCRYPTGENRANDOM_FAILED);
	}

	return (*(int64_t*)&number_buffer) % (max - min) + min + 1;
}

BOOL
WINAPI
DllMain(
	HINSTANCE,
	DWORD dwReason,
	LPVOID 
)
{
	const auto hook_factory = std::make_unique<HookFactory<DetourHook>>();
	const DynamicLibraryUPtr ntdll = std::make_unique<DynamicLibrary>("ntdll.dll");
	const int64_t alert_due_time = -1 * generate_random_number(MINIMUM_ALERT_TIME, MAXIMUM_ALERT_TIME);
	 
	switch (dwReason)
	{
	case DLL_THREAD_ATTACH:
		break;
	
	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_ATTACH:
		g_signal_waitable = std::make_unique<Timer>(std::wstring(ALERT_TIMER_NAME), false, alert_due_time);

		g_alert_thread = std::make_unique<Thread>(alert_thread_entry_point);

		g_hook = hook_factory->create_hook(ntdll, "NtAllocateVirtualMemory", &nt_allocate_virtual_memory_hook);
		g_hook->hook();
		g_hook->detach();

		break;

	case DLL_PROCESS_DETACH:
		break;
	}

	return true;
}
