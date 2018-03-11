#include <SDL.h>

struct ui_state
{
	double 	freq;
	int 	telem_pkts;
	time_t	telem_time;
	int		ssdv_pkts;
	time_t	ssdv_time;
	int 	bad_pkts;
	double	longitude;
	double	latitude;
	int		altitude;
	int		snr_ratio;
	int		rssi;
	int		activity_flag;
	int		internet_flag;
	int		network_flag;
};

//extern struct ui_state g_ui_state;

void ui_set_freq(double f);
void ui_set_pkt_counts(int telem_pkts, time_t telem_time, int ssdv_pkts, time_t ssdv_time, int bad_pkts);
void ui_set_gps_loc(double longitude, double latitude, int altitude);
void ui_set_rssi(int snr, int rssi);
void ui_set_activity_flag(int activity_flag);
void ui_set_network_flag(int activity_flag);
void ui_set_internet_flag(int activity_flag);
void ui_redraw();


extern SDL_Color tft_white	;
extern SDL_Color tft_red	;
extern SDL_Color tft_green	;
extern SDL_Color tft_yellow	;
extern SDL_Color tft_blue	;
extern SDL_Color tft_pink	;
extern SDL_Color tft_cyan	;
extern SDL_Color tft_black	;



void init_tft_display(char* display_type, int display_orientation);
void tft_printf(int x, int y, SDL_Color col, int sz, char* format, ...);
