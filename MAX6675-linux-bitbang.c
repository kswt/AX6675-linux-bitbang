#include <gpiod.h> // libgpiod is required
#include <stdio.h>
#include <unistd.h>

/*
GPIO chip and pin labels
Find it at /sys/kernel/debug/gpio
or /sys/class/gpio/<gpiochip_id>/label
*/

#define GPIOCHIP_LABEL "pinctrl-bcm2835"
#define CE0_N_LABEL "SPI_CE0_N"
#define SCLK_LABEL "SPI_SCLK"
#define MISO_LABEL "SPI_MISO"

int main(){
	struct gpiod_chip *gpiodchip0 = gpiod_chip_open_by_label(GPIOCHIP_LABEL);
	if(!gpiodchip0) {
		fprintf(stderr,"Cannot open %s\nAre you root?\n", GPIOCHIP_LABEL);
		return -1;
	}

	struct gpiod_line *CE0_N = gpiod_chip_find_line(gpiodchip0, CE0_N_LABEL);
	struct gpiod_line *SCLK = gpiod_chip_find_line(gpiodchip0, SCLK_LABEL);
	struct gpiod_line *MISO = gpiod_chip_find_line(gpiodchip0, MISO_LABEL);

	const char consumer[] = "MAX6675-bitbang";

	gpiod_line_request_output(CE0_N, consumer,1);
	gpiod_line_request_output(SCLK, consumer,0);
	gpiod_line_request_input(MISO, consumer);
	
// Delays (in microseconds):
	int tCON = 0.23*1000000; // Conversion time
	int tCSS = 200; // Delay after CE signal
	int tCL = 200; // SCL low duration
	int tCH = tCL; // SCL high duration

	int temperature; // * 0.01℃C
	while(1){
		int chip_output=0;
		usleep(tCON); 

		gpiod_line_set_value(CE0_N,0);
		usleep(tCSS);
		chip_output = gpiod_line_get_value(MISO);

		for(int i=0;i<15;++i){
			chip_output <<=1;
			gpiod_line_set_value(SCLK,1);
			usleep(tCH);
			gpiod_line_set_value(SCLK,0);
			usleep(tCL);
			chip_output |= gpiod_line_get_value(MISO);
		}
		gpiod_line_set_value(CE0_N,1);
		if(chip_output & 0b100)
			printf("\rThermocouple is not connected");
		else
		{
			temperature = (chip_output>>3)*100/4;
			printf("\r                             ");// Clear previous data
			printf("\r%.2f", (float)temperature/100);
		}
		fflush(stdout);
	}
	gpiod_chip_close(gpiodchip0);
	return 0;
}
