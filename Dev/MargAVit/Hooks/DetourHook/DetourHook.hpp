#pragma once

#include <Windows.h>

#include "Hooks/IHook/IHook.hpp"
#include "Hooks/DetourHook/DetourTransaction/DetourTransaction.hpp"

template<typename RetType, typename... ArgsType>
class DetourHook : public IHook<RetType, ArgsType...>
{
protected:
	using FuncPtrType = IHook<RetType, ArgsType...>::FuncPtrType;
	using DestinationPtrType = IHook<RetType, ArgsType...>::DestinationPtrType;

public:
	DetourHook(const FuncPtrType source, const DestinationPtrType destination = nullptr) :
		IHook<RetType, ArgsType...>(source, destination)
	{}

	DetourHook(const DynamicLibraryUPtr& library, const std::string& function_name, const DestinationPtrType destination = nullptr) :
		IHook<RetType, ArgsType...>(library, function_name, destination)
	{}

	DetourHook(DetourHook&&) = default;
	DetourHook(const DetourHook&) = delete;
	DetourHook& operator=(const DetourHook&) = delete;

	virtual ~DetourHook() = default;

protected:
	virtual void _hook() override
	{
		auto detour_transaction = std::make_unique<DetourTransaction>();
		detour_transaction->add_detour(&this->m_source, this->m_hook_callback);
	}

	virtual void _unhook() override
	{
		auto detour_transaction = std::make_unique<DetourTransaction>();
		detour_transaction->remove_detour(&this->m_source, this->m_hook_callback);
	}
};

template<typename RetType, typename... ArgsType>
using DetourHookPtr = std::shared_ptr<DetourHook<RetType, ArgsType...>>;

template<typename RetType, typename... ArgsType>
using DetourHookUPtr = std::unique_ptr<DetourHook<RetType, ArgsType...>>;
