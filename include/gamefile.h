#pragma once
#include <cstdlib>
#include "export.h"

namespace chipotto
{
	class CHIP8_API Gamefile
	{
	public:

		char* bytecode;
		size_t size;

		inline bool isValid() const { return bytecode != nullptr && size != 0; }

		Gamefile(size_t in_size): size(in_size)
		{
			bytecode = static_cast<char*>(malloc(in_size));
		}

		~Gamefile()
		{
			if (bytecode)
			{
				delete bytecode;
			}
		}
	};
}