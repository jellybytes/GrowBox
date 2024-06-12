#ifndef H_MQTT_UTILS
#define H_MQTT_UTILS

#include <mosquitto.h>

int init_mqtt(struct mosquitto *mosq);
void mqtt_publish(struct mosquitto *mosq, char *topic, char *payload);

#endif // H_MQTT_UTILS