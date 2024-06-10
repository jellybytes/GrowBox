#ifndef H_MAIN
#define H_MAIN

#include <stdio.h>
#include <stdlib.h>

#include <mosquitto.h>
#include <wiringPi.h>

#include "ssd1306_i2c.h"
#include "bme280.h"
#include "mqtt_utils.h"
#include "cJSON.h"

#define UNUSED(x) (void)(x)

struct _FAN_CTRL
{
	bool 	state;
	uint8_t	speed;	
	uint8_t	pwm_pin;	
};
typedef struct _FAN_CTRL FAN_CTRL;

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);

#endif // H_MAIN