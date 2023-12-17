#pragma once
#include "export.h"

namespace chipotto
{
	class IInputCommand;
	class EmuRenderer;
	class IRandomGenerator;
	class EmulatorImpl;
	class Gamefile;

	class CHIP8_API Emulator
	{
	public:
		Emulator(EmuRenderer* renderer, IInputCommand* input, IRandomGenerator* random_generator);
		~Emulator();

		bool Load(const Gamefile* gamefile);

		bool Tick(const float deltatime);

		void HardResetEmulator();

		void SetDoWrap(const bool do_wrap);

	private:
		EmulatorImpl* impl;
	};
}