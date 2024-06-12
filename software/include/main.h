#ifndef H_MAIN
#define H_MAIN

#include <stdbool.h>
#include <stdint.h>
#include <mosquitto.h>

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