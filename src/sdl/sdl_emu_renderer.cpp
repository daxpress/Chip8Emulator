#include "sdl/sdl_emu_renderer.h"

#include <cstring>
#include <SDL2/SDL.h>

namespace chipotto
{
	SDLEmuRenderer::SDLEmuRenderer(const int width, const int height) 
		: EmuRenderer(width, height)
	{

		window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width * 10, height * 10, 0);
		if (!window)
		{
			SDL_Log("Unable to create window: %s", SDL_GetError());
			return;
		}
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (!renderer)
		{
			SDL_Log("Unable to create renderer: %s", SDL_GetError());
			SDL_DestroyWindow(window);
			return;
		}
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, width, height);
		if (!texture)
		{
			SDL_Log("Unable to create texture: %s", SDL_GetError());
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			return;
		}
	}
	void SDLEmuRenderer::ClearScreen()
	{
		uint8_t* pixels = nullptr;
		int pitch;
		int result = SDL_LockTexture(texture, nullptr, reinterpret_cast<void**>(&pixels), &pitch);
		memset(pixels, 0, size_t(pitch) * height);
		SDL_UnlockTexture(texture);
		SDL_RenderCopy(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);
	}

	int SDLEmuRenderer::Draw(uint8_t const x_coord, const uint8_t y_coord,
		const uint8_t* raw_sprite_mono, const uint8_t sprite_height, bool do_wrap,
		bool& out_collision)
	{
		uint8_t* pixels = nullptr;
		int pitch;
		int result = SDL_LockTexture(texture, nullptr, reinterpret_cast<void**>(&pixels), &pitch);
		if (result != 0)
		{
			SDL_Log("Failed to lock texture");
			return -1;
		}

		for (int y = 0; y < sprite_height; ++y)
		{
			if (!do_wrap && y + y_coord >= height)
				break;
			uint8_t row_byte = raw_sprite_mono[y];
			for (int x = 0; x < 8; x++)
			{
				uint8_t pixel_color = (row_byte >> (7 - x)) & 0x1;
				uint8_t color = 0;
				if (pixel_color)
				{
					color = 0xFF;
				}
				if (!do_wrap && x + x_coord >= width)
					break;
				int pixel_index = ((x + x_coord) % width) * 4 + pitch * ((y + y_coord) % height);
				uint8_t existing_pixel = pixels[pixel_index];
				color ^= existing_pixel;

				if (existing_pixel != 0 && color != 0)
				{
					out_collision = true;
				}

				pixels[pixel_index + 0] = color;
				pixels[pixel_index + 1] = color;
				pixels[pixel_index + 2] = color;
				pixels[pixel_index + 3] = color;
			}
		}

		SDL_UnlockTexture(texture);

		SDL_RenderCopy(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);
		return 0;
	}

	SDLEmuRenderer::~SDLEmuRenderer()
	{
		if (texture)
		{
			SDL_DestroyTexture(texture);
		}
		if (renderer)
		{
			SDL_DestroyRenderer(renderer);
		}
		if (window)
		{
			SDL_DestroyWindow(window);
		}
	}
}