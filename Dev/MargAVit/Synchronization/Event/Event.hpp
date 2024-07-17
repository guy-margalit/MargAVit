#pragma once

#include <string>
#include <optional>

#include "AutoCloseObject/AutoCloseObject.hpp"

class Event : public AutoCloseHandle
{
public:
	Event(const std::wstring& event_name, const bool manual_reset, const bool initial_state = false);

	Event(const bool manual_reset, const bool initial_state = false);

	Event(Event&& other);

	Event(const Event&) = delete;
	Event& operator=(const Event&) = delete;

	virtual ~Event() = default;

	void set();
	void reset();
	bool is_manual_reset() const;

protected:
	static HANDLE _s_create_event(const std::optional<std::wstring>& event_name, const bool manual_reset, const bool initial_state);

protected:
	bool m_manual_reset;
};

using EventPtr = std::shared_ptr<Event>;
using EventUPtr = std::unique_ptr<Event>;
