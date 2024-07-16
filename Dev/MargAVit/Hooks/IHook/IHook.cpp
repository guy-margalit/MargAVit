#include "IHook.hpp"

IHook::IHook(const PVOID source, const PVOID destination) :
	m_original(source),
	m_source(source),
	m_destination(destination),
	m_should_unhook(false)
{}

IHook::~IHook()
{
	try
	{
		unhook();
	}
	catch (...) {}
}

PVOID IHook::get_source()
{
	return m_source;
}

void IHook::hook()
{
	_hook();
	m_should_unhook = true;
}

void IHook::unhook()
{
	if (m_should_unhook)
	{
		_unhook();
		m_should_unhook = false;
	}
}

void IHook::detach()
{
	m_should_unhook = false;
}
