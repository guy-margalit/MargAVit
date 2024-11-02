#pragma once

#include <new>
#include <memory>
#include <utility>
#include <Windows.h>
#include <functional>
#include <type_traits>

#include "DynamicLibrary/DynamicLibrary.hpp"

template<typename CallableType, typename RetType, typename... ArgsType>
auto _callable_to_function_pointer(CallableType&& callable, RetType(__stdcall*)(ArgsType...))
{
	static std::decay_t<CallableType> storage = std::forward<CallableType>(callable);
	static bool used = false;
	
	if (used)
	{
		using type = decltype(storage);
		storage.~type();
		new (&storage) type(std::forward<CallableType>(callable));
	}

	used = true;

	return [](ArgsType... args) -> RetType {
		auto& c = *std::launder(&storage);
		return RetType(c(std::forward<ArgsType>(args)...));
	};
}

template<typename FuncPtrType, typename CallableType>
FuncPtrType callable_to_function_pointer(CallableType&& callable)
{
	return _callable_to_function_pointer(std::forward<CallableType>(callable), reinterpret_cast<FuncPtrType>(nullptr));
}

template<typename RetType, typename... ArgsType>
class IHook
{
protected:
	using FuncPtrType = RetType(__stdcall*)(ArgsType...);
	using DestinationPtrType = RetType(__stdcall*)(FuncPtrType, ArgsType...);
	using LambdaCallbackType = std::function<RetType(__stdcall)(ArgsType...)>;

public:
	IHook(const FuncPtrType source, const DestinationPtrType destination = nullptr) :
		m_source(source),
		m_destination(destination),
		m_hook_callback_obj(_create_hook_func()),
		m_hook_callback(callable_to_function_pointer<FuncPtrType>(m_hook_callback_obj)),
		m_should_unhook(false)
	{}

	IHook(const DynamicLibraryUPtr& library, const std::string& function_name, const DestinationPtrType destination = nullptr) :
		IHook(reinterpret_cast<FuncPtrType>(library->get_function(function_name)), destination)
	{}

	IHook(const std::string& library_name, const std::string& function_name, const DestinationPtrType destination = nullptr) :
		IHook(std::make_unique<DynamicLibrary>(library_name), function_name, destination)
	{}

	virtual ~IHook()
	{
		try
		{
			unhook();
		}
		catch (...) {}
	}

	IHook(const IHook&) = delete;
	IHook& operator=(const IHook&) = delete;

	virtual void hook()
	{
		_hook();
		m_should_unhook = true;
	}

	virtual void unhook()
	{
		if (m_should_unhook)
		{
			_unhook();
			m_should_unhook = false;
		}
	}

	virtual void detach() { m_should_unhook = false; }

protected:
	LambdaCallbackType _create_hook_func()
	{
		return [=](ArgsType... args) -> RetType {
			return nullptr == m_destination ? m_source(args...) : reinterpret_cast<DestinationPtrType>(m_destination)(m_source, args...);
		};
	}

	virtual void _hook() = 0;
	virtual void _unhook() = 0;

protected:
	FuncPtrType m_source;
	const LambdaCallbackType m_hook_callback_obj;
	const FuncPtrType m_hook_callback;
	const PVOID m_destination;
	bool m_should_unhook;
};

template<typename RetType, typename... ArgsType>
using IHookPtr = std::shared_ptr<IHook<RetType, ArgsType...>>;

template<typename RetType, typename... ArgsType>
using IHookUPtr = std::unique_ptr<IHook<RetType, ArgsType...>>;
