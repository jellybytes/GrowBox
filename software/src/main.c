#include <stdio.h>
#include <stdlib.h>

#include <wiringPi.h>
#include "ssd1306_i2c.h"
#include "bme280.h"

#define BME280_IN_ADDR 	0x76
#define BME280_OUT_ADDR 0x77

void printBME280();
void testOLED();

BME280 bme280_in = {0};
BME280 bme280_out = {0};

int main()
{
	printBME280();
	testOLED();

    return 0;
}

void testOLED()
{
	ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
	// ssd1306_display(); //Adafruit logo is visible
	ssd1306_clearDisplay();
	ssd1306_setTextSize(2);

	char print_buf[32] = {0};
    sprintf(print_buf, "%.2f %cC\n", bme280_in.temperature, 0xF8);
	ssd1306_drawString(print_buf);

	char print_buf1[32] = {0};
    sprintf(print_buf1, "%.0f    %%\n", bme280_in.humidity);
	ssd1306_drawString(print_buf1);

	char print_buf2[32] = {0};
    sprintf(print_buf2, "%.0f  hPa\n", bme280_in.pressure);
	ssd1306_drawString(print_buf2);

	// ssd1306_display();
}


void printBME280()
{
    // holds calibration and values after read

    bme280_in.fd = wiringPiI2CSetup(BME280_IN_ADDR);
    if(bme280_in.fd < 0) {
    	printf("Device not found");
      	// return -1;
	}
    setupBME280(&bme280_in);
    readBME280(&bme280_in);

    printf("temperature: %.2f °C\n", bme280_in.temperature);
    printf("humidity: %.0f %%\n", bme280_in.humidity);
    printf("pressure: %.0f hPa\n", bme280_in.pressure);

    bme280_out.fd = wiringPiI2CSetup(BME280_OUT_ADDR);
    if(bme280_out.fd < 0) {
    	printf("Device not found");
    	// return -1;
    }

    setupBME280(&bme280_out);
    readBME280(&bme280_out);

    printf("temperature: %f\n", bme280_out.temperature);
    printf("humidity: %f\n", bme280_out.humidity);
    printf("pressure: %f\n", bme280_out.pressure);
}