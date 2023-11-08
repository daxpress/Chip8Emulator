#pragma once

#include <filesystem>
#include "gamefile.h"

namespace chipotto
{
	class Loader
	{
	public:
		static bool ReadFromFile(std::filesystem::path Path, Gamefile** out_gamefile);
	};
}