#include "sdl/sdl_input.h"
#include "SDL.h"

namespace chipotto
{
    SDLInput::SDLInput()
    {
#pragma region KEYBINDINGS 
		KeyboardMap[SDLK_1] = K_1;
		KeyboardMap[SDLK_2] = K_2;
		KeyboardMap[SDLK_3] = K_3;
		KeyboardMap[SDLK_4] = K_C;
		KeyboardMap[SDLK_q] = K_4;
		KeyboardMap[SDLK_w] = K_5;
		KeyboardMap[SDLK_e] = K_6;
		KeyboardMap[SDLK_r] = K_D;
		KeyboardMap[SDLK_a] = K_7;
		KeyboardMap[SDLK_s] = K_8;
		KeyboardMap[SDLK_d] = K_9;
		KeyboardMap[SDLK_f] = K_E;
		KeyboardMap[SDLK_z] = K_A;
		KeyboardMap[SDLK_x] = K_0;
		KeyboardMap[SDLK_c] = K_B;
		KeyboardMap[SDLK_v] = K_F;

#pragma endregion //KEYBINDINGS

		for (const auto& pair : KeyboardMap)
		{
			KeyboardValuesMap[pair.second] = static_cast<SDL_Scancode>(pair.first);
		}
    }

    const uint8_t* SDLInput::GetKeyboardState()
    {
        return SDL_GetKeyboardState(nullptr);   //returns the actual keyboard state
    }

    bool SDLInput::IsInputPending()
    {
		int result = SDL_PollEvent(&Event);
		if (!result)
		{
			SDL_PumpEvents();
		}
		KeyFromEvent = Event.key.keysym.sym;
		return result;
    }

    EmuKey SDLInput::GetKey()
    {
		return KeyboardMap[KeyFromEvent];
    }

    bool SDLInput::IsKeyPressed(const EmuKey key)
    {
        return GetKeyboardState()[KeyboardValuesMap[key]];
    }

	InputType SDLInput::GetInputEventType()
	{
		switch (Event.type)
		{
		case SDL_KEYDOWN:
			if (KeyboardMap.contains(KeyFromEvent))
			{
				return InputType::KEYDOWN;
			}
			return InputType::NONE;
		case SDL_QUIT:
			return InputType::QUIT;
		default:
			return InputType::NONE;
		}
	}
}