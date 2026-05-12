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

#include <stdio.h>
#include "motor_test.h"
#include "main.h"
#include "cy_pdl.h"
#include "cyhal.h"

/* Motor pin mapping from schematic */
#define M0_IN1_PORT GPIO_PRT9
#define M0_IN1_PIN  6
#define M0_IN1_MASK (1u << M0_IN1_PIN)

#define M0_IN2_PORT GPIO_PRT7
#define M0_IN2_PIN  2
#define M0_IN2_MASK (1u << M0_IN2_PIN)

#define M1_IN1_PORT GPIO_PRT5
#define M1_IN1_PIN  6
#define M1_IN1_MASK (1u << M1_IN1_PIN)

#define M1_IN2_PORT GPIO_PRT10
#define M1_IN2_PIN  6
#define M1_IN2_MASK (1u << M1_IN2_PIN)


void app_init_hw(void)
{
    motor_gpio_init();
}

void app_main(void)
{
    motor_test_loop();
}

void motor_gpio_init(void)
{
    Cy_GPIO_Pin_FastInit(M0_IN1_PORT, M0_IN1_PIN,
                         CY_GPIO_DM_STRONG_IN_OFF, 0, HSIOM_SEL_GPIO);

    Cy_GPIO_Pin_FastInit(M0_IN2_PORT, M0_IN2_PIN,
                         CY_GPIO_DM_STRONG_IN_OFF, 0, HSIOM_SEL_GPIO);

    Cy_GPIO_Pin_FastInit(M1_IN1_PORT, M1_IN1_PIN,
                         CY_GPIO_DM_STRONG_IN_OFF, 0, HSIOM_SEL_GPIO);

    Cy_GPIO_Pin_FastInit(M1_IN2_PORT, M1_IN2_PIN,
                         CY_GPIO_DM_STRONG_IN_OFF, 0, HSIOM_SEL_GPIO);
}

void motor_test_loop(void)
{
    // Run both motors forward continuously
    M0_IN1_PORT->OUT_SET = M0_IN1_MASK;//EN //* gives address and assigns
    M0_IN2_PORT->OUT_CLR = M0_IN2_MASK;

    M1_IN1_PORT->OUT_SET = M1_IN1_MASK;
    M1_IN2_PORT->OUT_CLR = M1_IN2_MASK;

    // Wait 1000 ms, then print message
    cyhal_system_delay_ms(1000);
    printf("Motors running forward...\n");
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