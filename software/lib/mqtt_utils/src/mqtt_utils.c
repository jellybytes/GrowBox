#include "mqtt_utils.h"
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include <mosquitto.h>

void on_connect(struct mosquitto *mosq, void *obj, int reason_code);
void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos);

int init_mqtt(struct mosquitto *mosq)
{
	// get mqtt broker credentials	
	const char * mqtt_user = getenv("MQTT_USER");
	const char * mqtt_pw = getenv("MQTT_PW");
	const char * mqtt_ip = getenv("MQTT_IP");

	mosquitto_lib_init();

	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_subscribe_callback_set(mosq, on_subscribe);
	mosquitto_message_callback_set(mosq, on_message);

	mosquitto_username_pw_set(mosq, mqtt_user, mqtt_pw);

	int rc = mosquitto_connect(mosq, mqtt_ip, 1883, 60);
	if(rc != MOSQ_ERR_SUCCESS){
		mosquitto_destroy(mosq);
		fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
	// tell compiled obj is not used
	UNUSED(obj);

	printf("on_connect: %s\n", mosquitto_connack_string(reason_code));
	if(reason_code != 0){
		mosquitto_disconnect(mosq);
	}

	int rc = mosquitto_subscribe(mosq, NULL, "gb_ex_fan/speed/percentage", 1);
	if(rc != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "Error subscribing: %s\n", mosquitto_strerror(rc));
		mosquitto_disconnect(mosq);
	}

	rc = mosquitto_subscribe(mosq, NULL, "gb_ex_fan/on/set", 1);
	if(rc != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "Error subscribing: %s\n", mosquitto_strerror(rc));
		mosquitto_disconnect(mosq);
	}

	rc = mosquitto_subscribe(mosq, NULL, "gb_env_sense/update", 1);
	if(rc != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "Error subscribing: %s\n", mosquitto_strerror(rc));
		mosquitto_disconnect(mosq);
	}
}

void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos)
{
	UNUSED(obj);
	UNUSED(mid);

	bool have_subscription = false;
	for(int i=0; i<qos_count; i++){
		// printf("on_subscribe: %d, granted qos = %d\n", i, granted_qos[i]);
		if(granted_qos[i] <= 2){
			have_subscription = true;
		}
	}

	if(have_subscription == false){
		fprintf(stderr, "Error: All subscriptions rejected.\n");
		mosquitto_disconnect(mosq);
	}
}