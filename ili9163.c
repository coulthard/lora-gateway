#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "ili9163.h"

// CE  1
// RST bcm:5  (wiring 21)
// DC  bcm:6  (wiring 22)
// LED bcm:13 (wiring 23)


#define SPI_CHANNEL			1
#define RST_WIRING_PIN		21
#define DC_WIRING_PIN		22
#define LED_WIRING_PIN		23

int g_spi_fd = -1;

static void write_command(uint8_t cmd)
{
    digitalWrite( DC_WIRING_PIN, 0); // Command mode
    wiringPiSPIDataRW( SPI_CHANNEL, &cmd, 1 );
}

static void write_data_buf(uint8_t* data, int len)
{
	const int max_chunk = 0x1000;
    digitalWrite( DC_WIRING_PIN, 1); // Data mode
    //wiringPiSPIDataRW( SPI_CHANNEL, data, len );
    while (len > max_chunk)
    {
    	write(g_spi_fd, data, max_chunk);
    	data += max_chunk;
    	len -= max_chunk;
    }
    write(g_spi_fd, data, len);
}

static void write_data(uint8_t data)
{
	write_data_buf(&data, 1);
}


void reset_LCD()
{
    digitalWrite( RST_WIRING_PIN, 0);
    usleep(20000);
    digitalWrite( RST_WIRING_PIN, 1);
    usleep(20000);
}


void set_frame(int x1, int y1, int x2, int y2)
{
	uint8_t data_xrange[4] = { x1 >> 8, x1 & 0xff, x2 >> 8, x2 & 0xff };
	uint8_t data_yrange[4] = { y1 >> 8, y1 & 0xff, y2 >> 8, y2 & 0xff };
	write_command(SET_COLUMN_ADDRESS);
	write_data_buf(data_xrange, 4);
	write_command(SET_PAGE_ADDRESS);
	write_data_buf(data_yrange, 4);
}

void clear_display(uint16_t col)
{
	uint8_t buf[TFT_WIDTH * 2];
	uint8_t *ptr = buf;
	int i;
	for(i = 0; i < TFT_WIDTH; i++)
	{
		*(ptr++) = (col >> 8);
		*(ptr++) = (col & 0xff);

	}

	set_frame(0, 0, TFT_WIDTH - 1, TFT_HEIGHT - 1);
	write_command(WRITE_MEMORY_START);
	for(i = 0; i < TFT_HEIGHT; i++)
	{
		write_data_buf(buf, sizeof(buf));
	}
}

void set_pixel(uint16_t col, int x, int y, int sz)
{
	set_frame(x, y, x + sz - 1, y + sz - 1);
	write_command(WRITE_MEMORY_START);
	int i;
	for (i =0; i < (sz * sz); i++)
	{
		write_data(col >> 8);
		write_data(col & 0xff);
	}
}

void init_display(uint8_t orientation)
{
	if (g_spi_fd == -1)
	{
		if ( wiringPiSetup() < 0 )
		{
			fprintf(stderr, "Failed to open wiringPi\n");
			exit(1);
		}

		if ( (g_spi_fd = wiringPiSPISetup(SPI_CHANNEL, 32000000)) < 0 )
		{
			fprintf(stderr, "Failed to open SPI port\n");
			exit(1);
		}
	}

	// Set all IO pins as outputs
	pinMode( LED_WIRING_PIN, OUTPUT );
    digitalWrite( LED_WIRING_PIN, 1);
    pinMode( DC_WIRING_PIN, OUTPUT );
    digitalWrite( DC_WIRING_PIN, 1);
    pinMode( RST_WIRING_PIN, OUTPUT );
    digitalWrite( RST_WIRING_PIN, 1);

    reset_LCD();

    uint8_t data_pos_gamma[]	= {0x3f, 0x25, 0x1c, 0x1e, 0x20, 0x12, 0x2a, 0x90, 0x24, 0x11, 0, 0, 0, 0, 0};
    uint8_t data_neg_gamma[]	= {0x20, 0x20, 0x20, 0x20, 0x05, 0, 0x15, 0xa7, 0x3d, 0x18, 0x25, 0x2a, 0x2b, 0x2b, 0x3a};
    uint8_t data_frame_ctl[]	= {8, 8};
    uint8_t data_power_ctl1[]	= {0x0a, 0x02};
    uint8_t data_vcom_ctl[]		= {0x50, 0x5b};

    write_command(EXIT_SLEEP_MODE);
    usleep(50000);	// 50ms
    write_command(SET_PIXEL_FORMAT);
    write_data(0x05);
    write_command(SET_GAMMA_CURVE);
    write_data(0x04);
    write_command(GAM_R_SEL);
    write_data(0x01);

    write_command(POSITIVE_GAMMA_CORRECT);
    write_data_buf(data_pos_gamma, sizeof(data_pos_gamma));

    write_command(NEGATIVE_GAMMA_CORRECT);
    write_data_buf(data_neg_gamma, sizeof(data_neg_gamma));
    write_command(FRAME_RATE_CONTROL1);
    write_data_buf(data_frame_ctl, sizeof(data_frame_ctl));

    write_command(DISPLAY_INVERSION);
    write_data(0x01);

    write_command(POWER_CONTROL1);
    write_data_buf(data_power_ctl1, sizeof(data_power_ctl1));

    write_command(POWER_CONTROL2);
    write_data(0x02);

    write_command(VCOM_CONTROL1);
    write_data_buf(data_vcom_ctl, sizeof(data_vcom_ctl));

    write_command(VCOM_OFFSET_CONTROL);
    write_data(0x40);
	set_frame(0, TFT_WIDTH - 1, 0, TFT_HEIGHT - 1);

    write_command(SET_ADDRESS_MODE);
    write_data(orientation);

    write_command(SET_DISPLAY_ON);
}

/**
 * Copy 128x128 bitmap to ILI9163 display
 * Code swaps order of every 2 bytes to cater for SDL bitmap being
 * little endian, and ILI9163 needing data in big endian order.
 */
void blt_bitmap(uint8_t* bitmap)
{
	int i;
	set_frame(0, 0, TFT_WIDTH - 1, TFT_HEIGHT - 1);
	write_command(WRITE_MEMORY_START);

	uint16_t *ptr = (uint16_t *) bitmap;
	uint16_t buf2[TFT_WIDTH * TFT_HEIGHT];
	for(i = 0; i < TFT_WIDTH * TFT_HEIGHT; i++)
		buf2[i] = ((ptr[i] << 8) | (ptr[i] >> 8));
	write_data_buf((uint8_t*) buf2, TFT_WIDTH * TFT_HEIGHT * 2);
}
