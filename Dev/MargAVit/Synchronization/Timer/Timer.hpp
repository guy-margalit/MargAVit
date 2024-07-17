#pragma once

#include <optional>

#include "AutoCloseObject/AutoCloseObject.hpp"

class Timer : public AutoCloseHandle
{
	Timer(const std::wstring& timer_name, const bool manual_reset);

	Timer(const bool manual_reset);

	Timer(Timer&& other);

	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;

	virtual ~Timer() = default;

	void set(int64_t due_time, uint32_t period = 0);
	void cancel();

protected:
	static HANDLE _s_create_timer(const std::optional<std::wstring>& timer_name, const bool manual_reset);
};

using TimerPtr = std::shared_ptr<Timer>;
using TimerUPtr = std::unique_ptr<Timer>;
