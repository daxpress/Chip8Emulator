#include "sdl/keyboardstate_input_command.h"
#include "SDL.h"

namespace chipotto
{
    const uint8_t* KeyboardStateInputCommand::GetKeyboardState()
    {
        return SDL_GetKeyboardState(nullptr);   //returns the actual keyboard state
    }
}