#ifndef MOTOR_TEST_H
#define MOTOR_TEST_H

#include <stdint.h>

// =========================
// Motor Control API for DRV8874 Tank Drive
// =========================

// Initialize all motor-related GPIOs (PH/EN)
void motor_gpio_init(void);

// Set left motor speed (-100 to +100)
void motor_set_left(int speed);

// Set right motor speed (-100 to +100)
void motor_set_right(int speed);

// Tank drive: set both motors (-100 to +100)
void tank_drive(int left_speed, int right_speed);

// Stop both motors (brake)
void motor_stop_all(void);

// Helper functions for common maneuvers
void motor_forward(uint8_t speed);    // Both forward
void motor_backward(uint8_t speed);   // Both backward
void motor_turn_left(uint8_t speed);  // Left backward, right forward
void motor_turn_right(uint8_t speed); // Left forward, right backward

// Main test sequence (runs a sequence of maneuvers)
void motor_test_loop(void);
void motor_uart_control(void);
#endif // MOTOR_TEST_H