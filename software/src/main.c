#include "main.h"

// temperature sensors
#define BME280_IN_ADDR 	0x77
#define BME280_OUT_ADDR 0x76

BME280 bme280_in = {0};
BME280 bme280_out = {0};

FAN_CTRL fan_one;
FAN_CTRL fan_two;

struct mosquitto *mosq;

int init_fan_CTRL();
void fan_CTRL(FAN_CTRL *ctrl);

int init_temp_sense();
void read_temp_sense();

void init_OLED();
void print_OLED();


int main()
{
	init_fan_CTRL();
	// turn off

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

	init_temp_sense();

	init_OLED();
	read_temp_sense();
	print_OLED();

	mosquitto_loop_forever(mosq, -1, 1);

	mosquitto_lib_cleanup();
	return 0;
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	UNUSED(obj);

	int rc;
	// fan state
	if (strcmp(msg->topic, "gb_ex_fan/on/set") == 0)
	{
		if (!strcmp((char *)msg->payload, "true"))
		{
			printf("fan on: %s\n", (char *)msg->payload);
			fan_one.state = true;
			fan_CTRL(&fan_one);

			char payload[] = "true";
			rc = mosquitto_publish(mosq, NULL, "gb_ex_fan/on/state", strlen(payload), payload, 2, false);
			if(rc != MOSQ_ERR_SUCCESS) 
				fprintf(stderr, "Error publishing: %s\n", mosquitto_strerror(rc));
		}
		else if (!strcmp((char *)msg->payload, "false"))
		{
			printf("fan off: %s\n", (char *)msg->payload);
			fan_one.state = false;
			fan_CTRL(&fan_one);

			char payload[] = "false";
			rc = mosquitto_publish(mosq, NULL, "gb_ex_fan/on/state", strlen(payload), payload, 2, false);
			if(rc != MOSQ_ERR_SUCCESS) 
				fprintf(stderr, "Error publishing: %s\n", mosquitto_strerror(rc));
		}
	}
	// fan speed
	else if (strcmp(msg->topic, "gb_ex_fan/speed/percentage") == 0)
	{
		printf("fan speed: %s\n", (char *)msg->payload);
		fan_one.speed = atoi((char *)msg->payload);
		fan_CTRL(&fan_one);

		char *payload = (char *)msg->payload;
		rc = mosquitto_publish(mosq, NULL, "gb_ex_fan/speed/percentage_state", strlen(payload), payload, 2, false);
		if(rc != MOSQ_ERR_SUCCESS) 
			fprintf(stderr, "Error publishing: %s\n", mosquitto_strerror(rc));
	}
	else if (strcmp(msg->topic, "gb_env_sense/update") == 0)
	{
		printf("gb_ex_temp/update\n");
		read_temp_sense();

		cJSON *root = NULL;
		char t_buf[8] = {0};
		sprintf(t_buf, "%.1f", bme280_in.temperature);
		const char *env_data[4] = {t_buf, t_buf, t_buf, t_buf};
		root = cJSON_CreateStringArray(env_data,4);
		// char *payload = NULL;
		char *payload = cJSON_Print(root);
		cJSON_Delete(root);

		rc = mosquitto_publish(mosq, NULL, "gb_env_sense/data", strlen(payload), payload, 2, false);
		if(rc != MOSQ_ERR_SUCCESS) 
			fprintf(stderr, "Error publishing: %s\n", mosquitto_strerror(rc));
	}
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
	fan_one.speed = 0;

	fan_two.pwm_pin = 23;
	fan_two.state = false;
	fan_two.speed = 0;

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

void init_OLED()
{
	ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
	// ssd1306_display(); //Adafruit logo is visible
	ssd1306_setTextSize(3);

}

void print_OLED()
{
	ssd1306_clearDisplay();

	char print_buf[8] = {0};
	sprintf(print_buf, "%.1f C", bme280_in.temperature);
	ssd1306_drawString(print_buf);

	char print_buf1[8] = {0};
    sprintf(print_buf1, "%.0f    %%\n", bme280_in.humidity);
	ssd1306_drawString(print_buf1);

	ssd1306_display(); // needed
}

int init_temp_sense()
{
    // holds calibration and values after read
    bme280_in.fd = wiringPiI2CSetup(BME280_IN_ADDR);
    if(bme280_in.fd < 0) {
    	printf("Error: Temperature sensor not found");
		return EXIT_FAILURE;
	}
    setupBME280(&bme280_in);

    bme280_out.fd = wiringPiI2CSetup(BME280_OUT_ADDR);
    if(bme280_out.fd < 0) {
    	printf("Error: Temperature sensor not found");
		return EXIT_FAILURE;
    }

    setupBME280(&bme280_out);
	return EXIT_SUCCESS;
}

void read_temp_sense()
{
    readBME280(&bme280_in);
    printf("temperature: %.1f °C\n", bme280_in.temperature);
    printf("humidity: %.0f %%\n", bme280_in.humidity);

    readBME280(&bme280_out);
    printf("temperature: %.1f °C\n", bme280_out.temperature);
    printf("humidity: %.0f %%\n", bme280_out.humidity);
}