#include <stdio.h>
#include <stdlib.h>

#include "bme280.h"
#include <wiringPi.h>

#define BME280_IN_ADDR 	0x76
#define BME280_OUT_ADDR 0x77

void printBME280();

int main()
{
	printBME280();
    return 0;
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