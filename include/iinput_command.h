#pragma once

#include <cstdint>
#include "keys.h"
#include "input_type.h"

namespace chipotto
{
	class IInputCommand
	{
	public:
		virtual const uint8_t* GetKeyboardState() = 0;
		virtual bool IsInputPending() = 0;
		virtual EmuKey GetKey() = 0;
		virtual bool IsKeyPressed(const EmuKey key) = 0;
		virtual InputType GetInputEventType() = 0;
		virtual ~IInputCommand() {};
	};
}
