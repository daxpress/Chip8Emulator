#pragma once

#include "renderer.h"

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;

namespace chipotto
{
	class SDLEmuRenderer : public EmuRenderer
	{
	public:
		SDLEmuRenderer(const int width, const int height);

		virtual void ClearScreen() override;
		/// <summary>
		/// SDL implementation of the DRW command.
		/// Draws a sprite from memory starting at location I, at (Vx, Vy) coords.
		/// </summary>
		/// <param name="x_coord">the x coordinate where the sprite is drawn</param>
		/// <param name="y_coord">the y coordinate where the sprite is drawn</param>
		/// <param name="raw_sprite_mono"> the sprite data using a single channel configuration</param>
		/// <param name="sprite_height"> the sprite height</param>
		/// <param name="do_wrap">if true, the sprite wraps around the screen</param>
		/// <param name="out_collision">true on pixel collision(s), false otherwise</param>
		/// <returns>0 if no error occurred</returns>
		virtual int Draw(uint8_t const x_coord, const uint8_t y_coord,
			const uint8_t* raw_sprite_mono, const uint8_t sprite_height, bool do_wrap,
			bool& out_collision) override;

		inline virtual bool IsValid() override
		{
			if (!window || !renderer || !texture)
				return false;
			return true;
		}

		virtual ~SDLEmuRenderer() override;

	protected:
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;
		SDL_Texture* texture = nullptr;
	};
}