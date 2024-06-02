#include <stdio.h>
#include <stdlib.h>

#include <wiringPi.h>
#include "ssd1306_i2c.h"
#include "bme280.h"

#define BME280_IN_ADDR 	0x76
#define BME280_OUT_ADDR 0x77

void printBME280();
void testOLED();

int main()
{
	printBME280();
	testOLED();

    return 0;
}

void testOLED()
{
	ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);

	ssd1306_display(); //Adafruit logo is visible
	ssd1306_clearDisplay();
	delay(5000);

	char* text = "This is demo for SSD1306 i2c driver for Raspberry Pi";
	ssd1306_drawString(text);
	ssd1306_display();
	delay(5000);

	ssd1306_dim(1);
	ssd1306_startscrollright(00,0xFF);
	delay(5000);

	ssd1306_clearDisplay();
	ssd1306_fillRect(10,10, 50, 20, WHITE);
	ssd1306_fillRect(80, 10, 130, 50, WHITE);
	ssd1306_display();
}


void printBME280()
{
    // holds calibration and values after read
    BME280 bme280_in = {0};
    BME280 bme280_out = {0};

    bme280_in.fd = wiringPiI2CSetup(BME280_IN_ADDR);
    if(bme280_in.fd < 0) {
    	printf("Device not found");
      	// return -1;
	}
    setupBME280(&bme280_in);
    readBME280(&bme280_in);

    printf("temperature: %f\n", bme280_in.temperature);
    printf("humidity: %f\n", bme280_in.humidity);
    printf("pressure: %f\n", bme280_in.pressure);

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