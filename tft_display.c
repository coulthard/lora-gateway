#include <stdlib.h>
#include <time.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>
#include <pthread.h>

#include "ili9163.h"
#include "tft_display.h"

SDL_Surface*	surface = NULL;
SDL_Renderer*	renderer = NULL;

SDL_Color		tft_white	= { 255, 255, 255 };
SDL_Color		tft_red		= { 255, 0, 0 };
SDL_Color		tft_green	= { 0, 255, 0 };
SDL_Color		tft_yellow	= { 255, 255, 0 };
SDL_Color		tft_blue	= { 0, 0, 255};
SDL_Color		tft_pink	= { 255, 0, 255};
SDL_Color		tft_cyan	= { 0, 255, 255};
SDL_Color		tft_black	= { 0, 0, 0};


extern pthread_mutex_t spi_mutex;


void init_tft_display()
{
    init_display(ORIENTATION180);

	if (TTF_Init() < 0)
	{
		fprintf(stderr, "TTF_Init failed\n");
		exit(1);
	}

	surface = SDL_CreateRGBSurface(SDL_SWSURFACE, TFT_WIDTH, TFT_HEIGHT, 16, 0x001f, 0x07e0, 0xf800, 0);
	renderer = SDL_CreateSoftwareRenderer(surface);

	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xff, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

    pthread_mutex_lock( &spi_mutex);
	blt_bitmap(surface->pixels);
    pthread_mutex_unlock( &spi_mutex);
}

void tft_printf(int xc, int yc, SDL_Color col, int sz, char* format, ...)
{
    char str[80];
    va_list args;
    va_start( args, format );
    vsprintf( str, format, args );
    va_end( args );

    TTF_Font* font = TTF_OpenFont( "/usr/share/fonts/truetype/droid/DroidSansMono.ttf", sz);
	if (!font)
	{
		fprintf(stderr, "unable to load font.\n");
		return;
	}

	SDL_Surface* textSurface = TTF_RenderText_Shaded(font, str, col, tft_blue);
	SDL_Rect targetPos = { x: xc, y: yc };
	SDL_BlitSurface(textSurface, NULL, surface, &targetPos);
	SDL_FreeSurface(textSurface);
	TTF_CloseFont(font);


    pthread_mutex_lock( &spi_mutex);
/*
    static time_t last_reset = 0;
	if (time(NULL) > last_reset + 15)
	{
		last_reset = time(NULL);
		init_display(ORIENTATION180);
	}
*/
	blt_bitmap(surface->pixels);
    pthread_mutex_unlock( &spi_mutex);
}

