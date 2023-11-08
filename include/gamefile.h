#pragma once
#include <cstdlib>

namespace chipotto
{
	class Gamefile
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