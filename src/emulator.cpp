#include "emulator.h"
#include "emulator_impl.h"

chipotto::Emulator::Emulator(EmuRenderer* renderer, IInputCommand* input, IRandomGenerator* random_generator)
{
	impl = new EmulatorImpl(renderer, input, random_generator);
}

chipotto::Emulator::~Emulator()
{
	if (impl)
	{
		delete impl;
	}
}

bool chipotto::Emulator::Load(const Gamefile* gamefile)
{
	return impl->Load(gamefile);
}

bool chipotto::Emulator::Tick(const float deltatime)
{
	return impl->Tick(deltatime);
}

void chipotto::Emulator::HardResetEmulator()
{
	impl->HardResetEmulator();
}

void chipotto::Emulator::SetDoWrap(const bool do_wrap)
{
	impl->SetDoWrap(do_wrap);
}
