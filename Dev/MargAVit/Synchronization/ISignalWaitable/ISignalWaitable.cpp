#include "ISignalWaitable.hpp"

ISignalWaitable::ISignalWaitable(const HANDLE handle, const bool manual_reset) :
	AutoCloseHandle(handle, CloseHandle),
	m_manual_reset(manual_reset)
{}

ISignalWaitable::ISignalWaitable(ISignalWaitable&& other) :
	AutoCloseHandle(std::move(other)),
	m_manual_reset(other.m_manual_reset)
{}

bool ISignalWaitable::is_manual_reset() const
{
	return m_manual_reset;
}
