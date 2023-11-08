#include "sdl/loader.h"
#include <fstream>

namespace chipotto
{
	bool Loader::ReadFromFile(std::filesystem::path Path, Gamefile** out_gamefile)
	{
        std::ifstream file;
        file.open(Path, std::ios::binary);
        if (!file.is_open()) return false;

        auto size = std::filesystem::file_size(Path);

        *out_gamefile = new Gamefile(size);

        file.read((*out_gamefile)->bytecode, (*out_gamefile)->size);
        file.close();
        return true;
	}
}
