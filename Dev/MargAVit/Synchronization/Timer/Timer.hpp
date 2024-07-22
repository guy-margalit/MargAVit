#pragma once

#include <optional>

#include "Synchronization/ISignalWaitable/ISignalWaitable.hpp"

class Timer : public ISignalWaitable
{
public:
	Timer(const std::wstring& timer_name, const bool manual_reset, int64_t due_time, uint32_t period = 0);

	Timer(const bool manual_reset, int64_t due_time, uint32_t period = 0);

	Timer(Timer&& other);

	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;

	virtual ~Timer() = default;

	void set() override;
	void cancel() override;

	int64_t get_due_time() const;
	uint32_t get_period() const;

protected:
	static HANDLE _s_create_timer(const std::optional<std::wstring>& timer_name, const bool manual_reset);

protected:
	int64_t m_due_time;  // 100 nanoseconds
	uint32_t m_period;
};

using TimerPtr = std::shared_ptr<Timer>;
using TimerUPtr = std::unique_ptr<Timer>;
