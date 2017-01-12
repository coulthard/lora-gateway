#include "ili9163.h"

#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>


void init_tft_display()
{
    init_display(ORIENTATION180);

	if (TTF_Init() < 0)
	{
		fprintf(stderr, "TTF_Init failed\n");
		exit(1);
	}
	TTF_Font* font = TTF_OpenFont( "/usr/share/fonts/truetype/droid/DroidSansMono.ttf", 14);
	if (!font)
	{
		fprintf(stderr, "unable to load font.\n");
		exit(1);
	}

	SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE, TFT_WIDTH, TFT_HEIGHT, 16, 0x001f, 0x07e0, 0xf800, 0);
	SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(surface);

	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xff, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	SDL_Color textColour = { 255, 255, 255 };
	//SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(font, "Ready", textColour, 100);
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Ready", textColour);
	SDL_Rect targetPos = { x: 40, y: 20 };
	SDL_BlitSurface(textSurface, NULL, surface, &targetPos);
	SDL_FreeSurface(textSurface);


	blt_bitmap(surface->pixels);
}


