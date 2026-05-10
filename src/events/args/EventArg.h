#pragma once

#include"src/utils/Math.h"
#include"src/global/Register.h"

struct EventArg {};

#define REG_EVENT_ARG(_CLASS)\
inline static const size_t event_id = static_cast<int>(Register<EventArg>::inst().getId<_CLASS>());