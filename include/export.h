#pragma once

#ifdef _WIN32
#ifdef CHIP8_IMPORT
#define CHIP8_API __declspec(dllimport)
#else
#define CHIP8_API __declspec(dllexport)
#endif // CHIP8_IMPORT
#else
#define CHIP8_API
#endif // _WIN32