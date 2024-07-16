#pragma once

#include <memory>
#include <string>
#include <Windows.h>

#include "Hooks/IHook/IHook.hpp"
#include "DynamicLibrary/DynamicLibrary.hpp"

template<class HookType>
class HookFactory
{
	static_assert(std::is_base_of<IHook, HookType>::value, "HookType must be derived from IHook");

public:
	HookFactory() = default;

	HookFactory(const HookFactory&) = delete;
	HookFactory& operator=(const HookFactory&) = delete;

	virtual ~HookFactory() = default;

	IHookUPtr create_hook(const PVOID source, const PVOID destination);
	IHookUPtr create_hook(const DynamicLibraryUPtr& library, const std::string& function_name, const PVOID destination);
};

template<class HookType>
using HookFactoryPtr = std::shared_ptr<HookFactory<HookType>>;

template<class HookType>
using HookFactoryUPtr = std::unique_ptr<HookFactory<HookType>>;

template<class HookType>
IHookUPtr HookFactory<HookType>::create_hook(const PVOID source, const PVOID destination)
{
	return std::make_unique<HookType>(source, destination);
}

template<class HookType>
IHookUPtr HookFactory<HookType>::create_hook(const DynamicLibraryUPtr& library, const std::string& function_name, const PVOID destination)
{
	return create_hook(library->get_function(function_name), destination);
}
