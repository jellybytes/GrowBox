#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "mqtt_utils.h"
#include "cJSON.h"
#include "bme280.h"
#include "ssd1306_i2c.h"


// temperature sensors
#define BME280_IN_ADDR 	0x76
#define BME280_EX_ADDR 0x77

BME280 bme280_in = {0};
BME280 bme280_ex = {0};

int init_env_sense();
void update_env_sense();

// fan utility
FAN_CTRL fan_one;
FAN_CTRL fan_two;

int init_fan_CTRL();
void update_fan_state(char *state);
void update_fan_speed(char *speed);
void fan_CTRL(FAN_CTRL *ctrl);

// oled display
void init_OLED();
void update_OLED();
bool disp_on = false;
uint8_t disp_on_cycles = 0;

#define BUTTON_PIN 22
int init_button();
void button_callback();

struct mosquitto *mosq;

int main()
{
	mosq = mosquitto_new(NULL, true, NULL);
	if(mosq == NULL){
		fprintf(stderr, "Error: Out of memory.\n");
		return EXIT_FAILURE;
	}

	if (init_mqtt(mosq) != EXIT_SUCCESS)
	{
		fprintf(stderr, "Error: Init mqtt connection failed.\n");
		return EXIT_FAILURE;
	}

	if (init_fan_CTRL() != EXIT_SUCCESS)
	{
		fprintf(stderr, "Error: Init PWM for fan control.\n");
		return EXIT_FAILURE;
	}
	update_fan_state("false");
	update_fan_speed("50");

	if (init_env_sense() != EXIT_SUCCESS)
	{
		fprintf(stderr, "Error: Init environment sensors\n");
		return EXIT_FAILURE;
	}
	update_env_sense();

	init_OLED();
	update_OLED();

	init_button();

	// blocking while loop, handles mqtt reconnect and
	// calls on_message if subscribed topic inc
	mosquitto_loop_forever(mosq, -1, 1);

	mosquitto_lib_cleanup();
	return 0;
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	UNUSED(obj);
	UNUSED(mosq);
	// fan state
	if (strcmp(msg->topic, "gb_ex_fan/on/set") == 0) update_fan_state((char *)msg->payload);
	// fan speed
	else if (strcmp(msg->topic, "gb_ex_fan/speed/percentage") == 0) update_fan_speed((char *)msg->payload);
	// bme280 environment sensor
	else if (strcmp(msg->topic, "gb_env_sense/update") == 0) update_env_sense();
}

int init_fan_CTRL()
{
	// initialise wiringPi with wPi numbering
	if (wiringPiSetup() == -1) 
	{
		printf("Error: Setting up wiringPi.\n");
		return EXIT_FAILURE;
	}

	fan_one.pwm_pin = 26;
	fan_one.state = false;
	fan_one.speed = 10; // range 10 - 180

	fan_two.pwm_pin = 23;
	fan_two.state = false;
	fan_two.speed = 10;

	// motor control PWM setup
	// fixed period for all Duty cycles
	pinMode(fan_one.pwm_pin, PWM_OUTPUT);
	pinMode(fan_two.pwm_pin, PWM_OUTPUT);

	pwmSetMode(PWM_MODE_MS);
	// clock divider 19.2 MHz / x
	// pwmFreq = 19.2e6 / clkDiv / range
	pwmSetClock(5);
	pwmSetRange(192);

	// set fans off
	fan_CTRL(&fan_one);
	fan_CTRL(&fan_two);
	return EXIT_SUCCESS;
}

int init_env_sense()
{
    // holds calibration and values after read
    bme280_in.fd = wiringPiI2CSetup(BME280_IN_ADDR);
    if(bme280_in.fd < 0) {
    	printf("Error: Temperature sensor not found");
		return EXIT_FAILURE;
	}
    setupBME280(&bme280_in);

    bme280_ex.fd = wiringPiI2CSetup(BME280_EX_ADDR);
    if(bme280_ex.fd < 0) {
    	printf("Error: Temperature sensor not found");
		return EXIT_FAILURE;
    }
    setupBME280(&bme280_ex);

	return EXIT_SUCCESS;
}

void init_OLED()
{
	ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
	ssd1306_setTextSize(3);
}

int init_button()
{
	if (wiringPiISR(BUTTON_PIN, INT_EDGE_FALLING, &button_callback) < 0)
	{
        printf("Error: Unable to setup ISR.\n");
		return EXIT_FAILURE; // 27343
	}
	return EXIT_SUCCESS;
}

void button_callback()
{
	uint8_t edge = 0b10000000;
	uint8_t stream = 0b11111111;
	for(int i = 0; i < 16; i++)
	{
	    stream <<= 1;
	    stream |= digitalRead(BUTTON_PIN);

	    if (stream == edge)
	    {
			// printf("button pressed!\n");
			disp_on = true;
			update_OLED();
	        break;
	    }
	   delay(3);
	}
}

void update_fan_state(char *state)
{
	if (!strcmp(state, "true"))
	{
		// printf("fan on: %s\n", state);
		fan_one.state = true;
		fan_CTRL(&fan_one);

		mqtt_publish(mosq, "gb_ex_fan/on/state", state);
	}
	else if (!strcmp(state, "false"))
	{
		// printf("fan off: %s\n", state);
		fan_one.state = false;
		fan_CTRL(&fan_one);

		mqtt_publish(mosq, "gb_ex_fan/on/state", state);
	}
}

void update_fan_speed(char *speed)
{
	// printf("fan speed: %s\n", speed);
	fan_one.speed = atoi(speed);
	fan_CTRL(&fan_one);

	mqtt_publish(mosq, "gb_ex_fan/speed/percentage_state", speed);
}

void fan_CTRL(FAN_CTRL *ctrl)
{
	if (ctrl->state == false) {
		pwmWrite(ctrl->pwm_pin, 0);
	}
	else if (ctrl->state == true)
	{
		pwmWrite(ctrl->pwm_pin, ctrl->speed);
	}
}

void update_env_sense()
{
    readBME280(&bme280_in);
    // printf("temperature: %s °C\n", bme280_in.temperature);
    // printf("humidity: %s %%\n", bme280_in.humidity);
    readBME280(&bme280_ex);
    // printf("temperature: %s °C\n", bme280_ex.temperature);
    // printf("humidity: %s %%\n", bme280_ex.humidity);

	cJSON *root = NULL;
	const char *env_data[4] = {	bme280_ex.temperature, bme280_ex.humidity, 
								bme280_in.temperature, bme280_in.humidity };
	root = cJSON_CreateStringArray(env_data,4);
	char *payload = cJSON_Print(root);
	cJSON_Delete(root);

	mqtt_publish(mosq, "gb_env_sense/data", payload);

	if (disp_on == true && disp_on_cycles < 2) disp_on_cycles++;
	if (disp_on == true && disp_on_cycles >= 2)
	{
		disp_on = false;
		disp_on_cycles = 0;
	}
	update_OLED();
	update_OLED();
}

void update_OLED()
{
	if (disp_on == false)
	{
		ssd1306_clearDisplay();
		ssd1306_display(); // needed
	}
	else
	{
		ssd1306_clearDisplay();
		char print_buf[16] = {0};
		sprintf(print_buf, "%s C ", bme280_in.temperature);
		ssd1306_drawString(print_buf);

		char print_buf1[16] = {0};
 		sprintf(print_buf1, "%s   %%\n", bme280_in.humidity);
		ssd1306_drawString(print_buf1);

		ssd1306_display();
	}
}