#pragma once

#include <optional>

#include "AutoCloseObject/AutoCloseObject.hpp"

class ISignalWaitable : public AutoCloseHandle
{
public:
	ISignalWaitable(const HANDLE handle, const bool manual_reset);

	ISignalWaitable(ISignalWaitable&& other);

	ISignalWaitable(const ISignalWaitable&) = delete;
	ISignalWaitable& operator=(const ISignalWaitable&) = delete;

	virtual ~ISignalWaitable() = default;

	virtual void set() = 0;
	virtual void cancel() = 0;
	bool is_manual_reset() const;

protected:
	bool m_manual_reset;
};

using ISignalWaitablePtr = std::shared_ptr<ISignalWaitable>;
using ISignalWaitableUPtr = std::unique_ptr<ISignalWaitable>;
