#ifndef MOTOR_TEST_H
#define MOTOR_TEST_H

#include "main.h"

// =========================
// Motor Control API for DRV8874 Tank Drive
// =========================

// Initialize all motor-related GPIOs (PH/EN)
void motor_gpio_init(void);

// Stop both motors (brake)
void motor_stop_all(void);

// Helper functions for common maneuvers
void motor_forward(void);    // Both forward
void motor_backward(void);   // Both backward
void motor_turn_leftF(void);   // Left forward, right backward
void motor_turn_rightF(void);  // Left backward, right forward

// Main test sequence (runs a sequence of maneuvers)
void motor_test_loop(void);
void motor_uart_control(void);
void rover_execute_command(char cmd);
void ble_comm_start(void);
#endif // MOTOR_TEST_H