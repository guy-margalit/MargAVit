#include <process.h>

#include "Thread.hpp"
#include "Common/MargAVitException.hpp"

Thread::Thread(const ThreadProc callback, void* arg) :
	m_handle(std::make_unique<AutoCloseHandle>(_create_thread(callback, arg), CloseHandle))
{}

Thread::Thread(Thread&& other) :
	m_handle(std::move(other.m_handle))
{}

Thread::~Thread()
{
	try
	{
		stop();
	}
	catch (...) {}
}

void Thread::stop()
{
	if (is_running())
	{
		if (!TerminateThread(m_handle->get(), 0))
		{
			throw MargAVitException(MARGAVITSTATUS_THREAD_CLOSE_TERMINATETHREAD_FAILED, GetLastError());
		}
	}
}

bool Thread::is_running()
{
	return STILL_ACTIVE == get_exit_code();
}

void Thread::join(uint32_t timeout)
{
	switch (WaitForSingleObject(m_handle->get(), timeout ? timeout : INFINITE))
	{
	case WAIT_OBJECT_0:
		// Thread is dead
		break;

	case WAIT_TIMEOUT:
		// Wait timed out
		throw MargAVitException(MARGAVITSTATUS_THREAD_JOIN_WAITFORSINGLEOBJECT_TIMEOUT);

	case WAIT_ABANDONED:
		// Received abandoned status some how (not a mutex)
		throw MargAVitException(MARGAVITSTATUS_THREAD_JOIN_WAITFORSINGLEOBJECT_ABANDONED);

	case WAIT_FAILED:
		// Wait failed
		throw MargAVitException(MARGAVITSTATUS_THREAD_JOIN_WAITFORSINGLEOBJECT_FAILED, GetLastError());

	default:
		// TF happened
		throw MargAVitException(MARGAVITSTATUS_THREAD_JOIN_WAITFORSINGLEOBJECT_UNEXPECTED_STATUS);
	}
}

uint32_t Thread::get_exit_code()
{
	DWORD thread_exit_code = 0;

	if (!GetExitCodeThread(m_handle->get(), &thread_exit_code))
	{
		throw MargAVitException(MARGAVITSTATUS_THREAD_GET_EXIT_CODE_GETEXITCODETHREAD_FAILED, GetLastError());
	}

	return thread_exit_code;
}

HANDLE Thread::_create_thread(const ThreadProc callback, void* arg)
{
	auto thread_arguments = std::make_unique<ThreadArgs>(callback, arg);
	const auto thread_handle = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, _safe_thread_ep, thread_arguments.release(), 0, nullptr));
	if (nullptr == thread_handle)
	{
		throw MargAVitException(MARGAVITSTATUS_THREAD_START_BEGINTHREADEX_FAILED, errno);
	}

	return thread_handle;
}

unsigned __stdcall Thread::_safe_thread_ep(void* arg)
{
	try
	{
		const ThreadArgsUPtr thread_args(reinterpret_cast<ThreadArgs*>(arg));
		return thread_args->first(thread_args->second);
	}
	catch (...)
	{
		return MARGAVITSTATUS_THREAD_SAFE_THREAD_EP_EXCEPTION_OCCURED;
	}
}
