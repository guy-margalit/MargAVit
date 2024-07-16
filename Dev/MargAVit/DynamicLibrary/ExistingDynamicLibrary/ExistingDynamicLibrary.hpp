#pragma once

#include "DynamicLIbrary/DynamicLibrary.hpp"

class ExistingDynamicLibrary : public DynamicLibrary
{
public:
	ExistingDynamicLibrary(const std::string& module_name);

	ExistingDynamicLibrary(const ExistingDynamicLibrary&) = delete;
	ExistingDynamicLibrary& operator=(const ExistingDynamicLibrary&) = delete;

	virtual ~ExistingDynamicLibrary() = default;

protected:
	static HMODULE _s_get_module(const std::string& module_name);
};

using ExistingDynamicLibraryPtr = std::shared_ptr<ExistingDynamicLibrary>;
using ExistingDynamicLibraryUPtr = std::unique_ptr<ExistingDynamicLibrary>;
