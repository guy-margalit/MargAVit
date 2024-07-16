#pragma once

#include <memory>
#include <string>
#include <Windows.h>

#include "AutoClose/AutoCloseObject/AutoCloseObject.hpp"

using AutoCloseModuleHandle = AutoCloseObject<HMODULE, decltype(FreeLibrary)>;

class DynamicLibrary : public AutoCloseModuleHandle
{
public:
	DynamicLibrary(const std::string& module_name);

	DynamicLibrary(const DynamicLibrary&) = delete;
	DynamicLibrary& operator=(const DynamicLibrary&) = delete;

	virtual ~DynamicLibrary() = default;

	PVOID get_function(const std::string& function_name) const;

protected:
	static HMODULE _s_get_module(const std::string& module_name);
};

using DynamicLibraryPtr = std::shared_ptr<DynamicLibrary>;
using DynamicLibraryUPtr = std::unique_ptr<DynamicLibrary>;
