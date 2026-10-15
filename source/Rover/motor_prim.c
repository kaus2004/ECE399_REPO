/*
 * Copy of the motor test implementation with a prim_ prefix so it can
 * coexist with motor_test.c without duplicate symbol conflicts.
 */
#include "motor_prim.h"
#include "dist_sens.h"
#include "cyhal_gpio.h"
#include "cybsp.h"
#include <FreeRTOS.h>
#include <task.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>

#define M0_IN1 P9_6
#define M0_IN2 P7_2
#define M1_IN1 P5_6
#define M1_IN2 P10_6
#define ROVER_STOP_DISTANCE_MM 100U

void prim_motor_forward(void)
{
	cyhal_gpio_write(M0_IN1, true);
	cyhal_gpio_write(M0_IN2, true);
	cyhal_gpio_write(M1_IN1, true);
	cyhal_gpio_write(M1_IN2, true);
}

void prim_motor_backward(void)
{
	cyhal_gpio_write(M0_IN1, true);
	cyhal_gpio_write(M0_IN2, false);
	cyhal_gpio_write(M1_IN1, true);
	cyhal_gpio_write(M1_IN2, false);
}

void prim_motor_turn_right(void)
{
	cyhal_gpio_write(M0_IN1, false);
	cyhal_gpio_write(M0_IN2, true);
	cyhal_gpio_write(M1_IN1, true);
	cyhal_gpio_write(M1_IN2, true);
}

void prim_motor_turn_left(void)
{
	cyhal_gpio_write(M0_IN1, true);
	cyhal_gpio_write(M0_IN2, true);
	cyhal_gpio_write(M1_IN1, true);
	cyhal_gpio_write(M1_IN2, false);
}
void prim_motor_turn_rightF(void)
{
	cyhal_gpio_write(M0_IN1, true);
	cyhal_gpio_write(M0_IN2, false);
	cyhal_gpio_write(M1_IN1, true);
	cyhal_gpio_write(M1_IN2, true);
}

void prim_motor_turn_leftF(void)
{
	cyhal_gpio_write(M0_IN1, true);
	cyhal_gpio_write(M0_IN2, true);
	cyhal_gpio_write(M1_IN1, true);
	cyhal_gpio_write(M1_IN2, false);
}

void rover_task(void *arg)
{
	(void) arg;
	printf("[RUN] rover_task entered.\r\n");

    while (1)
    {
        cyhal_gpio_toggle(CYBSP_USER_LED);

        printf("[RUN] Waiting for BLE commands...\r\n");

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void rover_safety_task(void *arg)
{
    uint16_t distance_mm = 0U;
    cyhal_i2c_t *sensor_i2c = NULL;

    (void)arg;

    printf("[SENSOR] Task started\r\n");

    sensor_i2c = proto_i2c_init(SDA_1, SCL_1);
    if (sensor_i2c == NULL)
    {
        printf("[SENSOR] I2C initialization failed\r\n");
        prim_motor_stop_all();
        vTaskDelete(NULL);
        return;
    }
    printf("[SENSOR] I2C initialized\r\n");

    if (!dist_sens_st_init())
    {
        printf("[SENSOR] VL53 initialization failed\r\n");
        prim_motor_stop_all();
        vTaskDelete(NULL);
        return;
    }
    printf("[SENSOR] VL53 initialized\r\n");

    while (1)
    {
        if (!dist_sens_st_poll_distance_mm(&distance_mm))
        {
            printf("[DIST] invalid / failed read\r\n");
        }
        else
        {
            printf("[DIST] %u mm\r\n", distance_mm);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void prim_motor_gpio_init(void)
{
	cy_rslt_t result;

	result = cyhal_gpio_init(M0_IN1, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	result = cyhal_gpio_init(M0_IN2, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	result = cyhal_gpio_init(M1_IN1, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	result = cyhal_gpio_init(M1_IN2, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	printf("[INIT] Motor GPIO outputs configured: P9_6, P7_2, P5_6, P10_6\r\n");
}


void prim_motor_stop_all(void)
{
	cyhal_gpio_write(M0_IN1, false);
	cyhal_gpio_write(M0_IN2, false);
	cyhal_gpio_write(M1_IN1, false);
	cyhal_gpio_write(M1_IN2, false);
}

void prim_motor_execute_command(char cmd, bool fast)
{
	char base_cmd = (char)tolower((unsigned char)cmd);

	switch (base_cmd)
	{
		case 'w':
			printf(fast ? "Forward (fast)\n" : "Forward\n");
			prim_motor_forward();
			break;
		case 's':
			printf(fast ? "Backward (fast)\n" : "Backward\n");
			prim_motor_backward();
			break;
		case 'a':
			printf(fast ? "Turn left (fast)\n" : "Turn left\n");
			if (fast) {
				prim_motor_turn_rightF();
			} else {
				prim_motor_turn_right();
			}
			break;
		case 'd':
			printf(fast ? "Turn right (fast)\n" : "Turn right\n");
			if (fast) {
				prim_motor_turn_leftF();
			} else {
				prim_motor_turn_left();
			}
			break;
		case 'x':
			printf("Stop\n");
			prim_motor_stop_all();
			break;
		default:
			if ((unsigned char)cmd > 32u) {
				printf("Unknown command: %c\n", cmd);
			}
			break;
	}
}

void prim_motor_uart_control(void)
{
	char cmd;
	bool fast_mode_enabled = false;
	printf("Rover control ready. Use keys:\n");
	printf("  w = forward\n");
	printf("  s = backward\n");
	printf("  a = turn left\n");
	printf("  d = turn right\n");
	printf("  x = stop\n");
	printf("  f = toggle fast mode on/off\n");
	printf("  Shift+W/A/S/D = one-shot fast command\n");

	prim_motor_stop_all();

	while (1)
	{
		cmd = getchar();

		if (cmd == 'f' || cmd == 'F') {
			fast_mode_enabled = !fast_mode_enabled;
			printf("Fast mode %s\n", fast_mode_enabled ? "ON" : "OFF");
			continue;
		}

		if (isupper((unsigned char)cmd) && (tolower((unsigned char)cmd) == 'w' ||
												   tolower((unsigned char)cmd) == 'a' ||
												   tolower((unsigned char)cmd) == 's' ||
												   tolower((unsigned char)cmd) == 'd')) {
			prim_motor_execute_command(cmd, true);
		} else {
			prim_motor_execute_command(cmd, fast_mode_enabled);
		}
	}
}
