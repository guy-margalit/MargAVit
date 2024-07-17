#include <Windows.h>

#include "Event.hpp"
#include "Common/MargAVitException.hpp"

Event::Event(const std::wstring& event_name, const bool manual_reset, const bool initial_state) :
	AutoCloseHandle(_s_create_event(event_name, manual_reset, initial_state), CloseHandle),
	m_manual_reset(manual_reset)
{}

Event::Event(const bool manual_reset, const bool initial_state) :
	AutoCloseHandle(_s_create_event(nullptr, manual_reset, initial_state), CloseHandle),
	m_manual_reset(manual_reset)
{}

Event::Event(Event&& other) :
	AutoCloseHandle(std::move(other)),
	m_manual_reset(other.m_manual_reset)
{}

void Event::set()
{
	if (!SetEvent(m_object))
	{
		throw MargAVitException(MargAVitStatus::MARGAVITSTATUS_EVENT_SET_SETEVENT_FAILED, GetLastError());
	}
}

void Event::reset()
{
	if (!ResetEvent(m_object))
	{
		throw MargAVitException(MargAVitStatus::MARGAVITSTATUS_EVENT_RESET_RESETEVENT_FAILED, GetLastError());
	}
}

bool Event::is_manual_reset() const
{
	return m_manual_reset;
}

HANDLE Event::_s_create_event(const std::optional<std::wstring>& event_name, const bool manual_reset, const bool initial_state)
{
	const HANDLE event_handle = CreateEventW(nullptr, manual_reset, initial_state, event_name.has_value() ? event_name->c_str() : nullptr);
	if (nullptr == event_handle)
	{
		throw MargAVitException(MargAVitStatus::MARGAVITSTATUS_EVENT_S_CREATE_EVENT_CREATEEVENTW_FAILED, GetLastError());
	}

	return event_handle;
}
