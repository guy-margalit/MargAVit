#pragma once

#include <cstdio>

#include "config.auto.h"
#include "Common/MargAVitException.hpp"

#define NTSTATUS_SUCCESS (0)

#define PAGE_EXECUTE_ALL (PAGE_EXECUTE | \
						  PAGE_EXECUTE_READ | \
						  PAGE_EXECUTE_READWRITE | \
						  PAGE_EXECUTE_WRITECOPY)

#define LOG(message) (wprintf_s(message"\n"))
