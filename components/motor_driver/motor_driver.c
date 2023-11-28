// motor_driver.c
#include "motor_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Function to initialize LEDC timer for PWM
void ledc_motor_timer_init(ledc_timer_t timer) {
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_RESOLUTION,
        .freq_hz = LEDC_BASE_FREQ,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = timer
    };
    ledc_timer_config(&ledc_timer);
}

// ------------------------- START MOTOR PART -------------------------------

// Function to initialize a motor
void motor_init(Motor* motor, ledc_channel_t channel, int mina_pin, int minb_pin, int pwm_pin, ledc_timer_t timer) {
    motor->channel = channel;
    motor->mina_pin = mina_pin;
    motor->minb_pin = minb_pin;

    // GPIO for MINA and MINB pins
    gpio_set_direction(mina_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction(minb_pin, GPIO_MODE_OUTPUT);

    // Initialize LEDC timer for the motor
    ledc_motor_timer_init(timer);

    // LEDC channel configuration for PWM
    ledc_channel_config_t ledc_channel = {
        .channel    = channel,
        .duty       = 0,
        .gpio_num   = pwm_pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint     = 0,
        .timer_sel  = timer
    };
    ledc_channel_config(&ledc_channel);
}

// Function to start a motor
void motor_start(Motor* motor, int speed) {
    // Set motor direction
    gpio_set_level(motor->mina_pin, speed >= 0 ? 1 : 0);
    gpio_set_level(motor->minb_pin, speed >= 0 ? 0 : 1);

    // Set motor speed
    speed = (speed > 4095) ? 4095 : (speed < -4095) ? -4095 : speed;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, motor->channel, abs(speed));
    ledc_update_duty(LEDC_LOW_SPEED_MODE, motor->channel);
}

// Function to stop a motor
void motor_stop(Motor* motor) {
    // Set motor speed
    ledc_set_duty(LEDC_LOW_SPEED_MODE, motor->channel, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, motor->channel);
    // Stop the motor by setting both MINA and MINB to low
    gpio_set_level(motor->mina_pin, 0);
    gpio_set_level(motor->minb_pin, 0);
}

// ------------------------- START ORUGA PART -------------------------------

// Oruga initialization function
void oruga_init(Oruga* oruga, int left_mina, int left_minb, int left_pwm, int right_mina, int right_minb, int right_pwm) {
    
    motor_init(&oruga->left_motor, LEDC_CHANNEL_0, left_mina, left_minb, left_pwm, LEDC_TIMER_0);
    motor_init(&oruga->right_motor, LEDC_CHANNEL_1, right_mina, right_minb, right_pwm, LEDC_TIMER_1);
}

// Move forward function
void move_forward(Oruga* oruga, int speed) {
    motor_start(&oruga->left_motor, speed);
    motor_start(&oruga->right_motor, speed);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    motor_stop(&oruga->left_motor);
    motor_stop(&oruga->right_motor);
}

// Move backward function
void move_backward(Oruga* oruga, int speed) {
    motor_start(&oruga->left_motor, -speed);
    motor_start(&oruga->right_motor, -speed);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    motor_stop(&oruga->left_motor);
    motor_stop(&oruga->right_motor);
}

// Turn right function
void turn_right(Oruga* oruga, int speed) {
    motor_start(&oruga->left_motor, speed);
    motor_start(&oruga->right_motor, -speed);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    motor_stop(&oruga->left_motor);
    motor_stop(&oruga->right_motor);
}

// Turn left function
void turn_left(Oruga* oruga, int speed) {
    motor_start(&oruga->left_motor, -speed);
    motor_start(&oruga->right_motor, speed);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    motor_stop(&oruga->left_motor);
    motor_stop(&oruga->right_motor);
}