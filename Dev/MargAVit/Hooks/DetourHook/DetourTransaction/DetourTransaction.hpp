#pragma once

#include <string>
#include <Windows.h>

#include "detours.h"

#include "Common/MargAVitException.hpp"

class DetourTransaction
{
public:
	DetourTransaction(const HANDLE thread = nullptr);

	DetourTransaction(const DetourTransaction&) = delete;
	DetourTransaction& operator=(const DetourTransaction&) = delete;

	virtual ~DetourTransaction();

	template<typename FuncType>
	void add_detour(FuncType* source, const FuncType destination);

	template<typename FuncType>
	void remove_detour(FuncType* source, const FuncType destination);

	void commit();

private:
	bool m_commited;
};

using DetourTransactionPtr = std::shared_ptr<DetourTransaction>;
using DetourTransactionUPtr = std::unique_ptr<DetourTransaction>;

template<typename FuncType>
void DetourTransaction::add_detour(FuncType* source, const FuncType destination)
{
	const ULONG status = DetourAttach(source, reinterpret_cast<PVOID>(destination));
	if (NO_ERROR != status)
	{
		throw MargAVitException(MargAVitStatus::MARGAVITSTATUS_DETOURTRANSACTION_ADD_DETOUR_DETOURATTACH_FAILED, static_cast<uint32_t>(status));
	}
}

template<typename FuncType>
void DetourTransaction::remove_detour(FuncType* source, const FuncType destination)
{
	const ULONG status = DetourDetach(source, reinterpret_cast<PVOID>(destination));
	if (NO_ERROR != status)
	{
		throw MargAVitException(MargAVitStatus::MARGAVITSTATUS_DETOURTRANSACTION_REMOVE_DETOUR_DETOURDETACH_FAILED, static_cast<uint32_t>(status));
	}
}
