#include <SDL.h>

extern SDL_Color tft_white	;
extern SDL_Color tft_red	;
extern SDL_Color tft_green	;
extern SDL_Color tft_yellow	;
extern SDL_Color tft_blue	;
extern SDL_Color tft_pink	;
extern SDL_Color tft_cyan	;
extern SDL_Color tft_black	;



void init_tft_display();
void tft_printf(int x, int y, SDL_Color col, int sz, char* format, ...);
