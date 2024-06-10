#ifndef H_MQTT_UTILS
#define H_MQTT_UTILS

#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int init_mqtt(struct mosquitto *mosq);

#endif // H_MQTT_UTILS