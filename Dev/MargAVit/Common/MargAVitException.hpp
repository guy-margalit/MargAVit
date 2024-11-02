#pragma once

#include <Windows.h>

#include "MargAVitStatus.hpp"

class MargAVitException
{
public:
	MargAVitException(const MargAVitStatus status, const uint32_t additional_info = 0) :
		m_status(status),
		m_additional_info(additional_info)
	{}

	virtual ~MargAVitException() = default;

	MargAVitStatus get_status() const { return m_status; }
	uint32_t get_additional_info() const { return m_additional_info; }

private:
	const MargAVitStatus m_status;
	const uint32_t m_additional_info;
};
