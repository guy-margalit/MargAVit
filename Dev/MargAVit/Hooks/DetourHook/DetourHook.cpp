#include "DetourHook.hpp"
#include "DetourTransaction/DetourTransaction.hpp"

DetourHook::DetourHook(const PVOID source, const PVOID destination) :
	IHook(source, destination)
{}

void DetourHook::_hook()
{	
	auto detour_transaction = std::make_unique<DetourTransaction>();
	detour_transaction->add_detour(&m_source, m_destination);
}

void DetourHook::_unhook()
{
	auto detour_transaction = std::make_unique<DetourTransaction>();
	detour_transaction->remove_detour(&m_source, m_destination);
}
