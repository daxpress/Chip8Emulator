#pragma once

#include "export.h"

#define KEY_AS_INT(Key) static_cast<int>(Key)
#define INT_AS_KEY(Int) static_cast<EmuKey>(Int)

namespace chipotto
{
	enum EmuKey
	{
		K_0 = 0x0,
		K_1 = 0x1,
		K_2 = 0x2,
		K_3 = 0x3,
		K_4 = 0x4,
		K_5 = 0x5,
		K_6 = 0x6,
		K_7 = 0x7,
		K_8 = 0x8,
		K_9 = 0x9,
		K_A = 0xA,
		K_B = 0xB,
		K_C = 0xC,
		K_D = 0xD,
		K_E = 0xE,
		K_F = 0xF,
		K_NONE
	};
}