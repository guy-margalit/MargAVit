#include <Windows.h>

#include "Timer.hpp"
#include "Common/MargAVitException.hpp"

Timer::Timer(const std::wstring& timer_name, const bool manual_reset) :
	AutoCloseHandle(_s_create_timer(timer_name, manual_reset), CloseHandle)
{}

Timer::Timer(const bool manual_reset) :
	AutoCloseHandle(_s_create_timer(nullptr, manual_reset), CloseHandle)
{}

Timer::Timer(Timer&& other) :
	AutoCloseHandle(std::move(other))
{}

void Timer::set(int64_t due_time, uint32_t period)
{
	LARGE_INTEGER large_due_time = { 0 };
	large_due_time.QuadPart = due_time;

	if (!SetWaitableTimer(
		m_object,
		&large_due_time,
		static_cast<LONG>(period),
		nullptr,
		nullptr,
		false))
	{
		throw MargAVitException(MargAVitStatus::MARGAVITSTATUS_TIMER_SET_SETWAITABLETIMER_FAILED, GetLastError());
	}
}

void Timer::cancel()
{
	if (!CancelWaitableTimer(m_object))
	{
		throw MargAVitException(MargAVitStatus::MARGAVITSTATUS_TIMER_CANCEL_CANCELWAITABLETIMER_FAILED, GetLastError());
	}
}

HANDLE Timer::_s_create_timer(const std::optional<std::wstring>& timer_name, const bool manual_reset)
{
	const HANDLE timer_handle = CreateWaitableTimerW(nullptr, manual_reset, timer_name.has_value() ? timer_name->c_str() : nullptr);
	if (nullptr == timer_handle)
	{
		throw MargAVitException(MargAVitStatus::MARGAVITSTATUS_TIMER_S_CREATE_TIMER_CREATEWATABLETIMERW_FAILED, GetLastError());
	}

	return timer_handle;
}
