/*
============================================================
 UART Rover Control Instructions
============================================================
Open a serial terminal (e.g., Tera Term, PuTTY, minicom) at the correct COM port and baud rate.

Use these keys to control the rover in real time:
    w = move forward
    s = move backward
    a = turn left
    d = turn right
    x = stop

Type a command and press Enter (if required by your terminal).
============================================================
*/

#include "cybsp.h"
#include <stdio.h>
#include "motor_test.h"
#include "main.h"
#include "cyhal.h"

/* Motor pin mapping from schematic (keep these assignments fixed) */
#define M0_IN1 P9_6
#define M0_IN2 P7_2
#define M1_IN1 P5_6
#define M1_IN2 P10_6

static void motor_command_forward(void)
{
    cyhal_gpio_write(M0_IN1, true);
    cyhal_gpio_write(M0_IN2, false);
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

    motor_gpio_init();
    printf("[INIT] app_init_hw complete. LED and motor pins initialized.\r\n");
}

void app_main(void)
{
    printf("[RUN] app_main entered. Starting continuous forward drive loop.\r\n");

    while (1)
    {
        motor_command_forward();
        cyhal_gpio_toggle(CYBSP_USER_LED);
        printf("[RUN] Forward command active. LED toggled.\r\n");
        cyhal_system_delay_ms(500);
    }
}

void motor_gpio_init(void)
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


void motor_test_loop(void)
{
    app_main();
}

void motor_set_left(int speed)
{
    if (speed >= 0)
    {
        cyhal_gpio_write(M0_IN1, true);
        cyhal_gpio_write(M0_IN2, false);
    }
    else
    {
        cyhal_gpio_write(M0_IN1, false);
        cyhal_gpio_write(M0_IN2, true);
    }
}

void motor_set_right(int speed)
{
    if (speed >= 0)
    {
        cyhal_gpio_write(M1_IN1, true);
        cyhal_gpio_write(M1_IN2, false);
    }
    else
    {
        cyhal_gpio_write(M1_IN1, false);
        cyhal_gpio_write(M1_IN2, true);
    }
}

void tank_drive(int left_speed, int right_speed)
{
    motor_set_left(left_speed);
    motor_set_right(right_speed);
}

void motor_stop_all(void)
{
    cyhal_gpio_write(M0_IN1, false);
    cyhal_gpio_write(M0_IN2, false);
    cyhal_gpio_write(M1_IN1, false);
    cyhal_gpio_write(M1_IN2, false);
}

void motor_forward(uint8_t speed)
{
    (void)speed;
    tank_drive(100, 100);
}

void motor_backward(uint8_t speed)
{
    (void)speed;
    tank_drive(-100, -100);
}

void motor_turn_left(uint8_t speed)
{
    (void)speed;
    tank_drive(-100, 100);
}

void motor_turn_right(uint8_t speed)
{
    (void)speed;
    tank_drive(100, -100);
}

// Interactive UART rover control (independent function)
// Call this from main to control rover via terminal
void motor_uart_control(void)
{
    char cmd;
    printf("Rover control ready. Use keys:\n");
    printf("  w = forward\n");
    printf("  s = backward\n");
    printf("  a = turn left\n");
    printf("  d = turn right\n");
    printf("  x = stop\n");

    motor_stop_all();

    while (1)
    {
        cmd = getchar(); // Blocking read from UART terminal

        switch (cmd)
        {
            case 'w':
                printf("Forward\n");
                motor_forward(70);
                break;
            case 's':
                printf("Backward\n");
                motor_backward(70);
                break;
            case 'a':
                printf("Turn left\n");
                motor_turn_left(60);
                break;
            case 'd':
                printf("Turn right\n");
                motor_turn_right(60);
                break;
            case 'x':
                printf("Stop\n");
                motor_stop_all();
                break;
            default:
                printf("Unknown command: %c\n", cmd);
                break;
        }
    }
}