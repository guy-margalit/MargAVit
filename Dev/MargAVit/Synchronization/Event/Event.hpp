#pragma once

#include <string>
#include <optional>

#include "Synchronization/ISignalWaitable/ISignalWaitable.hpp"

class Event : public ISignalWaitable
{
public:
	Event(const std::wstring& event_name, const bool manual_reset, const bool initial_state = false);

	Event(const bool manual_reset, const bool initial_state = false);

	Event(Event&& other);

	Event(const Event&) = delete;
	Event& operator=(const Event&) = delete;

	virtual ~Event() = default;

	void set() override;
	void cancel() override;

protected:
	static HANDLE _s_create_event(const std::optional<std::wstring>& event_name, const bool manual_reset, const bool initial_state);
};

using EventPtr = std::shared_ptr<Event>;
using EventUPtr = std::unique_ptr<Event>;
