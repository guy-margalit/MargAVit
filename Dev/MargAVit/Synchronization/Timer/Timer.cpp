#include <Windows.h>

#include "Timer.hpp"
#include "Common/MargAVitException.hpp"

Timer::Timer(const std::wstring& timer_name, const bool manual_reset, int64_t due_time, uint32_t period) :
	ISignalWaitable(_s_create_timer(timer_name, manual_reset), manual_reset),
	m_due_time(due_time),
	m_period(period)
{}

Timer::Timer(const bool manual_reset, int64_t due_time, uint32_t period) :
	ISignalWaitable(_s_create_timer(nullptr, manual_reset), manual_reset),
	m_due_time(due_time),
	m_period(period)
{}

Timer::Timer(Timer&& other) :
	ISignalWaitable(std::move(other)),
	m_due_time(other.m_due_time),
	m_period(other.m_period)
{}

void Timer::set()
{
	LARGE_INTEGER large_due_time = { 0 };
	large_due_time.QuadPart = m_due_time;

	if (!SetWaitableTimer(
		m_object,
		&large_due_time,
		static_cast<LONG>(m_period),
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

int64_t Timer::get_due_time() const
{
	return m_due_time;
}

uint32_t Timer::get_period() const
{
	return m_period;
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
