#include <stdint.h>

//ILI9163 commands

#define NOP 0x00
#define SOFT_RESET 0x01
#define ENTER_SLEEP_MODE 0x10
#define EXIT_SLEEP_MODE 0x11
#define ENTER_PARTIAL_MODE 0x12
#define ENTER_NORMAL_MODE 0x13
#define EXIT_INVERT_MODE 0x20
#define ENTER_INVERT_MODE 0x21
#define SET_GAMMA_CURVE 0x26
#define SET_DISPLAY_OFF 0x28
#define SET_DISPLAY_ON 0x29
#define SET_COLUMN_ADDRESS 0x2A
#define SET_PAGE_ADDRESS 0x2B
#define WRITE_MEMORY_START 0x2C
#define SET_PARTIAL_AREA 0x30
#define SET_SCROLL_AREA 0x33
#define SET_ADDRESS_MODE 0x36
#define SET_SCROLL_START 0X37
#define EXIT_IDLE_MODE 0x38
#define ENTER_IDLE_MODE 0x39
#define SET_PIXEL_FORMAT 0x3A
#define WRITE_MEMORY_CONTINUE 0x3C
#define READ_MEMORY_CONTINUE 0x3E
#define FRAME_RATE_CONTROL1 0xB1
#define FRAME_RATE_CONTROL2 0xB2
#define FRAME_RATE_CONTROL3 0xB3
#define DISPLAY_INVERSION 0xB4
#define POWER_CONTROL1 0xC0
#define POWER_CONTROL2 0xC1
#define POWER_CONTROL3 0xC2
#define POWER_CONTROL4 0xC3
#define POWER_CONTROL5 0xC4
#define VCOM_CONTROL1 0xC5
#define VCOM_CONTROL2 0xC6
#define VCOM_OFFSET_CONTROL 0xC7
#define POSITIVE_GAMMA_CORRECT 0xE0
#define NEGATIVE_GAMMA_CORRECT 0xE1
#define GAM_R_SEL 0xF2

#define ORIENTATION0	0x00
#define ORIENTATION90	0x60
#define ORIENTATION270	0xa0
#define ORIENTATION180	0xc0

#define TFT_WIDTH			128
#define TFT_HEIGHT			128

// rgb565 encoding
#define RGB(r, g, b)	(((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3))

void init_display(uint8_t orientation, int draw_x_offset, int draw_y_offset);
void clear_display(uint16_t col);
void set_pixel(uint16_t col, int x, int y, int sz);
void blt_bitmap(uint8_t* bitmap);

