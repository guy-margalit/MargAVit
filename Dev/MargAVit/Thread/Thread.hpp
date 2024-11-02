#pragma once

#include <memory>
#include <Windows.h>
#include <functional>

#include "AutoCloseObject/AutoCloseObject.hpp"

using ThreadProc = std::function<unsigned(__stdcall)(void*)>;
using ThreadArgs = std::pair<ThreadProc, void*>;
using ThreadArgsUPtr = std::unique_ptr<ThreadArgs>;

class Thread
{
public:
	Thread(const ThreadProc callback, void* arg = nullptr);

	Thread(Thread&& other);

	virtual ~Thread();

	Thread(const Thread&) = delete;
	Thread& operator=(const Thread&) = delete;

	void stop();
	bool is_running();
	void join(uint32_t timeout = 0);
	uint32_t get_exit_code();

protected:
	static HANDLE _create_thread(const ThreadProc callback, void* arg);
	static unsigned __stdcall _safe_thread_ep(void* arg);

protected:
	AutoCloseHandleUPtr m_handle;
};

using ThreadPtr = std::shared_ptr<Thread>;
using ThreadUPtr = std::unique_ptr<Thread>;
