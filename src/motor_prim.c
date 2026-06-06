/*
 * Copy of the motor test implementation with a prim_ prefix so it can
 * coexist with motor_test.c without duplicate symbol conflicts.
 */
#include "motor_prim.h"

#if defined(MOTOR_PRIM)

#define M0_IN1 P9_6
#define M0_IN2 P7_2
#define M1_IN1 P5_6
#define M1_IN2 P10_6

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

void app_init_hw(void)
{
	cy_rslt_t result;

	result = cybsp_init();
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	__enable_irq();

	result = cy_retarget_io_init_fc(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
			CYBSP_DEBUG_UART_CTS, CYBSP_DEBUG_UART_RTS, CY_RETARGET_IO_BAUDRATE);
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	result = cyhal_gpio_init(CYBSP_USER_LED,
							 CYHAL_GPIO_DIR_OUTPUT,
							 CYHAL_GPIO_DRIVE_STRONG,
							 CYBSP_LED_STATE_OFF);
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	prim_motor_gpio_init();
	printf("[INIT] app_init_hw complete. LED and motor pins initialized.\r\n");
}

void app_main(void)
{
	printf("[RUN] app_main entered. Starting continuous forward drive loop.\r\n");

	while (1)
	{
		prim_motor_turn_rightF();
		cyhal_gpio_toggle(CYBSP_USER_LED);
		printf("[RUN] Forward command active. LED toggled.\r\n");
		cyhal_system_delay_ms(500);
		prim_motor_stop_all();

		cyhal_system_delay_ms(1000);

		cyhal_gpio_toggle(CYBSP_USER_LED);
		printf("[RUN] Backward command active. LED toggled.\r\n");
		cyhal_system_delay_ms(500);
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

void prim_motor_uart_control(void)
{
	char cmd;
	printf("Rover control ready. Use keys:\n");
	printf("  w = forward\n");
	printf("  s = backward\n");
	printf("  a = turn left\n");
	printf("  d = turn right\n");
	printf("  x = stop\n");

	prim_motor_stop_all();

	while (1)
	{
		cmd = getchar();

		switch (cmd)
		{
			case 'w':
				printf("Forward\n");
				prim_motor_forward();
				break;
			case 's':
				printf("Backward\n");
				prim_motor_backward();
				break;
			case 'a':
				printf("Turn left\n");
				prim_motor_turn_rightF();
				break;
			case 'd':
				printf("Turn right\n");
				prim_motor_turn_leftF();
				break;
			case 'x':
				printf("Stop\n");
				prim_motor_stop_all();
				break;
			default:
				printf("Unknown command: %c\n", cmd);
				break;
		}
	}
}
#endif