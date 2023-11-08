#pragma once

#include <filesystem>
#include "gamefile.h"

namespace chipotto
{
	class Loader
	{
	public:
		/// <summary>
		/// Loads a Gamefile from the provided path. Allocates a new Gamefile on success, remember to delete it when done!
		/// </summary>
		/// <param name="Path:">the path to load from</param>
		/// <param name="out_gamefile:">the Gamefile pointer address to which allocate the instance</param>
		/// <returns>false on error, true otherwise</returns>
		static bool ReadFromFile(std::filesystem::path Path, Gamefile** out_gamefile);
	};
}