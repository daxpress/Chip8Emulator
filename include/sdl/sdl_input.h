#pragma once

#include <unordered_map>
#include <array>

#include <SDL.h>
#include "iinput_command.h"


namespace chipotto
{
	class SDLInput : public IInputCommand
	{
	public:
		SDLInput();

		virtual const uint8_t* GetKeyboardState() override;
		// returns true if there is an event in the event queue, false otherwise
		virtual bool IsInputPending() override;
		// returns the key that triggered the SDL_KEYDOWN event
		virtual EmuKey GetKey() override;
		virtual bool IsKeyPressed(const EmuKey key) override;
		virtual InputType GetInputEventType() override;

	protected:
		// used to link the SDL_Keycode to the emulator hex key
		std::unordered_map<SDL_Keycode, EmuKey> KeyboardMap;
		// used to get an SDL_Scancode from the emulator hex key
		std::array<SDL_Scancode, 0x10> KeyboardValuesMap;

		SDL_Keycode KeyFromEvent;
		SDL_Event Event;
	};
}