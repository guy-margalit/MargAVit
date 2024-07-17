#include "DetourTransaction.hpp"

DetourTransaction::DetourTransaction(const HANDLE thread) :
	m_commited(false)
{
	ULONG status = DetourTransactionBegin();
	if (NO_ERROR != status)
	{
		throw MargAVitException(MargAVitStatus::MARGAVITSTATUS_DETOURTRANSACTION_CTOR_DETOURTRANSACTIONBEGIN_FAILED, static_cast<uint32_t>(status));
	}

	status = DetourUpdateThread(nullptr == thread ? GetCurrentThread() : thread);
	if (NO_ERROR != status)
	{
		throw MargAVitException(MargAVitStatus::MARGAVITSTATUS_DETOURTRANSACTION_CTOR_DETOURUPDATETHREAD_FAILED, static_cast<uint32_t>(status));
	}
}

DetourTransaction::~DetourTransaction()
{
	try
	{
		commit();
	}
	catch (...) {}
}

void DetourTransaction::commit()
{
	if (!m_commited)
	{
		const ULONG status = DetourTransactionCommit();
		if (NO_ERROR != status)
		{
			throw MargAVitException(MargAVitStatus::MARGAVITSTATUS_DETOURTRANSACTION_COMMIT_DETOURTRANSACTIONCOMMIT_FAILED, static_cast<DWORD>(status));
		}

		m_commited = true;
	}
}
