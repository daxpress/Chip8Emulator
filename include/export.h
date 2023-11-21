#pragma once

#ifdef _WIN32
#define CHIP8_API __declspec(dllexport)
#else
#define CHIP8_API
#endif // _WIN32