#pragma once

#include "irandom_generator.h"

namespace chipotto
{
	class EmulatorRandomGenerator : public IRandomGenerator
	{
	public:
		virtual uint8_t GetRandomByte() override;
	};
}