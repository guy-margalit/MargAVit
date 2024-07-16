#pragma once

#include <memory>
#include <Windows.h>

class IHook
{
public:
	IHook(const PVOID source, const PVOID destination);

	IHook(const IHook&) = delete;
	IHook& operator=(const IHook&) = delete;

	virtual ~IHook();

	virtual PVOID get_source();
	virtual void hook();
	virtual void unhook();
	virtual void detach();

protected:
	virtual void _hook() = 0;
	virtual void _unhook() = 0;

protected:
	PVOID m_source;
	const PVOID m_original;
	const PVOID m_destination;
	bool m_should_unhook;
};

using IHookPtr = std::shared_ptr<IHook>;
using IHookUPtr = std::unique_ptr<IHook>;
