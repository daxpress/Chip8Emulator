#pragma once

#include <cstdint>

namespace chipotto
{
	class IRandomGenerator
	{
	public:
		virtual uint8_t GetRandomByte() = 0;
		virtual ~IRandomGenerator() {};
	};
}