#ifndef MOTOR_PRIM_H
#define MOTOR_PRIM_H

#include "main.h"

void prim_motor_gpio_init(void);
void prim_motor_stop_all(void);
void prim_motor_forward(void);
void prim_motor_backward(void);
void prim_motor_turn_rightF(void);
void prim_motor_turn_leftF(void);
void prim_motor_test_loop(void);
void prim_motor_uart_control(void);

#endif // MOTOR_PRIM_H