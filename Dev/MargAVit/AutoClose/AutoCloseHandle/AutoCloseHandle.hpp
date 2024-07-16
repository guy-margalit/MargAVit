#pragma once

#include <memory>
#include <Windows.h>

#include "AutoClose/AutoCloseObject/AutoCloseObject.hpp"

template<class HandleType>
class AutoCloseHandle final : public AutoCloseObject<HandleType, decltype(CloseHandle)>
{
public:
	explicit AutoCloseHandle(const HandleType& handle) :
		AutoCloseObject<HandleType, decltype(CloseHandle)>(handle, CloseHandle)
	{}

	AutoCloseHandle(const AutoCloseHandle&) = delete;
	AutoCloseHandle& operator=(const AutoCloseHandle&) = delete;

	virtual ~AutoCloseHandle() = default;
};

using AutoCloseHandlePtr = std::shared_ptr<AutoCloseHandle<HANDLE>>;
using AutoCloseHandleUPtr = std::unique_ptr<AutoCloseHandle<HANDLE>>;

using AutoCloseModuleHandlePtr = std::shared_ptr<AutoCloseHandle<HMODULE>>;
using AutoCloseModuleHandleUPtr = std::unique_ptr<AutoCloseHandle<HMODULE>>;
