#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "emulator.h"
#include "sdl/loader.h"
#include "sdl/sdl_emu_renderer.h"
#include "sdl/sdl_input.h"
#include "sdl/emulator_random_generator.h"

#include <iostream>

int main(int argc, char** argv)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return -1;
	}
	float last_tick = SDL_GetTicks64();

	chipotto::SDLEmuRenderer renderer = chipotto::SDLEmuRenderer(64, 32);

	if (!renderer.IsValid())
	{
		SDL_Quit();
		return -1;
	}

	chipotto::SDLInput input_class = chipotto::SDLInput();
	chipotto::EmulatorRandomGenerator random_generator = chipotto::EmulatorRandomGenerator();

	chipotto::Emulator emulator(&renderer, &input_class, &random_generator);

	chipotto::Gamefile* gamefile;
	if (!chipotto::Loader::ReadFromFile("resources\\TICTAC", &gamefile))
	{
		goto quit_on_error;	// panicking
	}

	emulator.Load(gamefile);


	while (true)
	{
		float deltatime = SDL_GetTicks64() - last_tick;
		deltatime *= 0.001f;
		last_tick = SDL_GetTicks64();

		if (!emulator.Tick(deltatime))
		{
			break;
		}
	}

cleanup:				// jump here if quitting with cleanup is needed
		delete gamefile;

quit:
	SDL_Quit();
	return 0;

quit_on_error:
	SDL_Quit();
	return -1;
}