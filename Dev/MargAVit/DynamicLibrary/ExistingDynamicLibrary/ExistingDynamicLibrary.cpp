#include "ExistingDynamicLibrary.hpp"
#include "Common/MargAVitException.hpp"

ExistingDynamicLibrary::ExistingDynamicLibrary(const std::string& module_name) :
	DynamicLibrary(module_name)
{}

HMODULE ExistingDynamicLibrary::_s_get_module(const std::string& module_name)
{
	HMODULE module_handle = nullptr;

	if (!GetModuleHandleExA(0, module_name.c_str(), &module_handle))
	{
		throw MargAVitException(MargAVitStatus::MARGAVITSTATUS_EXISTINGDYNAMICLIBRARY_S_GET_MODULE_GETMODULEHANDLEA_FAILED, GetLastError());
	}

	return module_handle;
}