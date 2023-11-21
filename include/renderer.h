#pragma once
#include "export.h"

#include <cstdint>

namespace chipotto
{
	class CHIP8_API EmuRenderer
	{
	public:
		EmuRenderer(const int in_width, const int in_height) 
			: width(in_width), height(in_height)
		{};

		virtual void ClearScreen() = 0;
		/// <summary>
		/// Implementation of the DRW command for your render pipeline.
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
			bool& out_collision) = 0;

		virtual bool IsValid() = 0;

		virtual ~EmuRenderer() {};

	protected:
		int width;
		int height;
	};
}
