#ifndef MOTOR_PRIM_H
#define MOTOR_PRIM_H

#include <stdbool.h>

void prim_motor_gpio_init(void);
void prim_motor_stop_all(void);
void prim_motor_forward(void);
void prim_motor_backward(void);
void prim_motor_turn_right(void);
void prim_motor_turn_left(void);
void prim_motor_turn_rightF(void);
void prim_motor_turn_leftF(void);
void rover_safety_latch(void);
void rover_safety_clear(void);
bool rover_safety_is_blocked(void);
void rover_recovery_reverse(void);
void prim_motor_test_loop(void);
void prim_motor_uart_control(void);
void rover_task(void *arg);
void rover_safety_task(void *arg);

#endif // MOTOR_PRIM_H