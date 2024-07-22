#include "DynamicLibrary.hpp"
#include "Common/MargAVitException.hpp"

DynamicLibrary::DynamicLibrary(const std::string& module_name) :
	AutoCloseModuleHandle(_s_get_module(module_name), FreeLibrary)
{}

PVOID DynamicLibrary::get_function(const std::string& function_name) const
{
	const PVOID function_address = GetProcAddress(m_object, function_name.c_str());
	if (nullptr == function_address)
	{
		throw MargAVitException(MARGAVITSTATUS_DYNAMICLIBRARY_GET_FUNCTION_GETPROCADDRESS_FAILED, GetLastError());
	}

	return function_address;
}

HMODULE DynamicLibrary::_s_get_module(const std::string& module_name)
{
	HMODULE module_handle = LoadLibraryA(module_name.c_str());
	if (nullptr == module_handle)
	{
		throw MargAVitException(MARGAVITSTATUS_DYNAMICLIBRARY_S_GET_MODULE_LOADLIBRARYA_FAILED, GetLastError());
	}

	return module_handle;
}
