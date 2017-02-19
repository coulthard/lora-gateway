#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
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
struct ui_state g_ui_state;


int hline(SDL_Renderer * renderer, Sint16 x1, Sint16 x2, Sint16 y);
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


void ui_redraw()
{
	static time_t last_redraw = 0;
	time_t now = time(NULL);

	// limit redraws to one every 10 seconds.	
	if (now <= last_redraw + 10)
		return;
	last_redraw = now;

	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xff, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	tft_printf(0, 0, tft_white, 12, "Lora %.3f MHz ", g_ui_state.freq);

	tft_printf(0, 16, tft_green, 12, "Telem:%d (%us) ",
			g_ui_state.telem_pkts,
			g_ui_state.telem_time ? (unsigned int) (time(NULL) - g_ui_state.telem_time) : 0);
    tft_printf(0, 30, tft_green, 12, "Image:%d (%us) ",
                   g_ui_state.ssdv_pkts,
				   g_ui_state.ssdv_time ? (unsigned int) (time(NULL) - g_ui_state.ssdv_time) : 0);

	tft_printf(0, 50, tft_white, 18, "%8.5lf ", g_ui_state.latitude);
	tft_printf(0, 70, tft_white, 18, "%8.5lf ", g_ui_state.longitude);
	tft_printf(0, 90, tft_white, 15, "%um ", g_ui_state.altitude);

    tft_printf(0, 110, tft_yellow, 12, "SNR:%d rssi:%d ", g_ui_state.snr_ratio, g_ui_state.rssi );


    //uint32_t activiy_col = g_ui_state.activity_flag ?
    //		SDL_MapRGB(surface->format, 0xff, 0x00, 0x20) : SDL_MapRGB(surface->format, 0x00, 0x00, 0x00);
    //uint32_t network_col = g_ui_state.internet_flag ? SDL_MapRGB(surface->format, 0x00, 0xff, 0x00) :
	//		g_ui_state.network_flag ? SDL_MapRGB(surface->format, 0xff, 0xff, 0x00) :
	//		SDL_MapRGB(surface->format, 0x00, 0x00, 0x00);

    //SDL_Rect rect1 = { x: 105, y:40, w:18, h:18};
    //SDL_Rect rect2 = { x: 105, y:65, w:18, h:18};
    //SDL_FillRect(surface, &rect1, activiy_col);
    //SDL_FillRect(surface, &rect2, network_col);


    uint32_t activity_col = g_ui_state.activity_flag ? 0xff0020ffUL : 0xff000000UL;
    uint32_t network_col = g_ui_state.internet_flag ? 0xff00ff00UL :
			g_ui_state.network_flag ? 0xff00ffffUL : 0xff000000UL;

    filledCircleColor(renderer, 112, 49, 10, activity_col);
    filledCircleColor(renderer, 112, 74, 10, network_col);


    pthread_mutex_lock( &spi_mutex);
	blt_bitmap(surface->pixels);
    pthread_mutex_unlock( &spi_mutex);
}



void ui_set_freq(double f)
{
	g_ui_state.freq = f;
	ui_redraw();
}

void ui_set_pkt_counts(
	int 	telem_pkts,
	time_t	telem_time,
	int		ssdv_pkts,
	time_t	ssdv_time)
{
	g_ui_state.telem_pkts = telem_pkts;
	g_ui_state.telem_time = telem_time;
	g_ui_state.ssdv_pkts  = ssdv_pkts;
	g_ui_state.ssdv_time  = ssdv_time;
	ui_redraw();
}

void ui_set_gps_loc(
	double	longitude,
	double	latitude,
	int		altitude)
{
	g_ui_state.longitude = longitude;
	g_ui_state.latitude  = latitude;
	g_ui_state.altitude  = altitude;

	ui_redraw();
}

void ui_set_activity_flag(int activity_flag)
{
	g_ui_state.activity_flag = activity_flag;
	ui_redraw();
}

void ui_set_network_flag(int network_flag)
{
	g_ui_state.network_flag = network_flag;
	ui_redraw();
}

void ui_set_internet_flag(int internet_flag)
{
	g_ui_state.internet_flag = internet_flag;
	ui_redraw();
}



void ui_set_rssi(int snr, int rssi)
{
	g_ui_state.snr_ratio = snr;
	g_ui_state.rssi	     = rssi;

	ui_redraw();
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

/*
    pthread_mutex_lock( &spi_mutex);
	blt_bitmap(surface->pixels);
    pthread_mutex_unlock( &spi_mutex);
*/
}

