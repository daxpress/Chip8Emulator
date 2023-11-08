#pragma once

#include "iinput_command.h"

namespace chipotto
{
	class KeyboardStateInputCommand : public IInputCommand
	{
	public:
		virtual const uint8_t* GetKeyboardState() override;
	};
}