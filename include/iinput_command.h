#pragma once

#include <cstdint>

namespace chipotto
{
	class IInputCommand
	{
	public:
		virtual const uint8_t* GetKeyboardState() = 0;
	};
}
