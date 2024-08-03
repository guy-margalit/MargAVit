#pragma once

#include "Common/Common.hpp"
#include "Hooks/DetourHook/DetourHook.hpp"

#define DETACHED_HOOK(dll, function, hook_callback) create_detached_hook<HOOK_TYPE>(static_cast<decltype(&(function))>(nullptr), dll, #function, (hook_callback));

#define DETACHED_EMPTY_HOOK(dll, function) DETACHED_HOOK(dll, function, nullptr)

template<template<typename> typename HookType, typename DestinationPtrType, typename RetType, typename... ArgsType>
void create_detached_hook(RetType(__stdcall*)(ArgsType...), const std::string& library_name, const std::string& function_name, const DestinationPtrType destination)
{
	auto hook = new HookType<RetType, ArgsType...>(library_name, function_name, destination);
	hook->hook();
	hook->detach();
}
