#pragma once

#include "Common/Common.hpp"
#include "Hooks/DetourHook/DetourHook.hpp"

#define HOOK_TYPE DetourHook

#define DETACHED_HOOK(function_name, hook_callback) create_detached_hook<HOOK_TYPE>(static_cast<decltype(&(function_name))>(nullptr), #function_name, (hook_callback));

#define DETACHED_EMPTY_HOOK(function_name) create_detached_hook<HOOK_TYPE>(static_cast<decltype(&(function_name))>(nullptr), #function_name);

template<template<typename> typename HookType, typename DestinationPtrType, typename RetType, typename... ArgsType>
void create_detached_hook(RetType(__stdcall*)(ArgsType...), const std::string& function_name, const DestinationPtrType destination = bn, const DynamicLibraryUPtr& library = g_ntdll)
{
	auto hook = new HookType<RetType, ArgsType...>(library, function_name, destination);
	hook->hook();
	hook->detach();
}

template<template<typename> typename HookType, typename RetType, typename... ArgsType>
void create_detached_hook(RetType(__stdcall*)(ArgsType...), const std::string& function_name, const DynamicLibraryUPtr& library = g_ntdll)
{
	auto hook = new HookType<RetType, ArgsType...>(library, function_name);
	hook->hook();
	hook->detach();
}
