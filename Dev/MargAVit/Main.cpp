#include <stdio.h>
#include <Windows.h>

#include "Hooks/DetourHook/DetourHook.hpp"
#include "Hooks/HookFactory/HookFactory.hpp"
#include "DynamicLibrary/ExistingDynamicLibrary/ExistingDynamicLibrary.hpp"

#define PAGE_EXECUTE_ALL (PAGE_EXECUTE | \
						  PAGE_EXECUTE_READ | \
						  PAGE_EXECUTE_READWRITE | \
						  PAGE_EXECUTE_WRITECOPY)

typedef
NTSTATUS
(NTAPI* PFN_NT_ALLOCATE_VIRTUAL_MEMORY)
(HANDLE ProcessHandle,
	PVOID* BaseAddress,
	ULONG ZeroBits,
	PULONG RegionSize,
	ULONG AllocationType,
	ULONG Protect);

static IHookUPtr g_hook;

static
NTSTATUS
NTAPI
HOOK_NtAllocateVirtualMemory(
	HANDLE hProcess,
	PVOID* pvBaseAddress,
	ULONG ulZeroBits,
	PULONG pluRegionSize,
	ULONG ulAllocationType,
	ULONG ulProtect
)
{
	wprintf_s(L"ulProtect: %lu\n", ulProtect);

	if (PAGE_EXECUTE_ALL & ulProtect)
	{
		wprintf_s(L"SUCCESS\n");
	}

	return reinterpret_cast<PFN_NT_ALLOCATE_VIRTUAL_MEMORY>(g_hook->get_source())(
		hProcess,
		pvBaseAddress,
		ulZeroBits,
		pluRegionSize,
		ulAllocationType,
		ulProtect);
}

BOOL
WINAPI
DllMain(
	HINSTANCE hInstance,
	DWORD dwReason,
	LPVOID lpvReserved
)
{
	const DynamicLibraryUPtr ntdll = std::make_unique<ExistingDynamicLibrary>("ntdll.dll");
	const auto hook_factory = std::make_unique<HookFactory<DetourHook>>();

	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(lpvReserved);
	 
	switch (dwReason)
	{
	case DLL_THREAD_ATTACH:
		break;
	
	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_ATTACH:
		g_hook = hook_factory->create_hook(ntdll, "NtAllocateVirtualMemory", &HOOK_NtAllocateVirtualMemory);
		g_hook->hook();
		g_hook->detach();

		break;

	case DLL_PROCESS_DETACH:
		break;
	}

	return true;
}
