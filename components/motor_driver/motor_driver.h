// motor_driver.h
#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "driver/gpio.h"
#include "driver/ledc.h"

#define LEDC_TIMER_RESOLUTION LEDC_TIMER_12_BIT
#define LEDC_BASE_FREQ        5000

// Motor structure
typedef struct {
    ledc_channel_t channel;
    int mina_pin;
    int minb_pin;
    int pwm_pin;
    ledc_timer_t timer;
} Motor;

// Oruga structure
typedef struct {
    Motor left_motor;
    Motor right_motor;
} Oruga;

// Function declarations
void motor_init(Motor* motor, ledc_channel_t channel, int mina_pin, int minb_pin, int pwm_pin, ledc_timer_t timer);
void motor_start(Motor* motor, int speed);
void motor_stop(Motor* motor);
void oruga_init(Oruga* oruga, int left_mina, int left_minb, int left_pwm, int right_mina, int right_minb, int right_pwm);
void move_forward(Oruga* oruga, int speed);
void move_backward(Oruga* oruga, int speed);
void turn_right(Oruga* oruga, int speed);
void turn_left(Oruga* oruga, int speed);

#endif // MOTOR_DRIVER_H