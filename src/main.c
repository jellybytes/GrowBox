#include <stdio.h>
#include <stdlib.h>

#include "bme280.h"

#include <wiringPi.h>

int main()
{
    // holds calibration and values after read
    BME280 bme280 = {0};
    bme280.fd = wiringPiI2CSetup(BME280_ADDRESS);
    if(bme280.fd < 0) {
      printf("Device not found");
      return -1;
    }

    setupBME280(&bme280);
    readBME280(&bme280);

    printf("temperature: %f\n", bme280.temperature);
    printf("humidity: %f\n", bme280.humidity);
    printf("pressure: %f\n", bme280.pressure);

    return 0;
}