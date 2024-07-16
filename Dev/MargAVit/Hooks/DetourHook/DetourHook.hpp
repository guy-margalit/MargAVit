#pragma once

#include <Windows.h>

#include "Hooks/IHook/IHook.hpp"

class DetourHook : public IHook
{
public:
	DetourHook(const PVOID source, const PVOID destination);

	DetourHook(const DetourHook&) = delete;
	DetourHook& operator=(const DetourHook&) = delete;

	virtual ~DetourHook() = default;

protected:
	virtual void _hook() override;
	virtual void _unhook() override;
};

using DetourHookPtr = std::shared_ptr<DetourHook>;
using DetourHookUPtr = std::unique_ptr<DetourHook>;
