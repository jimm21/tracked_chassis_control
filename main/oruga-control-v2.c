#include <stdio.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LEDC_TIMER_RESOLUTION LEDC_TIMER_12_BIT // 12-bit resolution
#define LEDC_BASE_FREQ        5000              // Frequency in Hertz

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

// Oruga initialization function
void oruga_init(Oruga* oruga, int left_mina, int left_minb, int left_pwm, int right_mina, int right_minb, int right_pwm) {
    
    motor_init(&oruga->left_motor, LEDC_CHANNEL_0, left_mina, left_minb, left_pwm, LEDC_TIMER_0);
    motor_init(&oruga->right_motor, LEDC_CHANNEL_1, right_mina, right_minb, right_pwm, LEDC_TIMER_1);
}

void app_main() {
    // GPIOS
    int left_mina = 27;
    int left_minb = 12;
    int left_pwm = 13;
    int right_mina = 17;
    int right_minb = 16;
    int right_pwm = 4;

    // Create an instance of Oruga
    Oruga my_oruga;

    // Corrected calls to init, start, and stop functions
    oruga_init(&my_oruga, left_mina, left_minb, left_pwm, right_mina, right_minb, right_pwm);

    // Start the motors
    motor_start(&my_oruga.left_motor, 2048);
    motor_start(&my_oruga.right_motor, -2048);

    vTaskDelay(5000 / portTICK_PERIOD_MS);

    // Stop the motors
    motor_stop(&my_oruga.left_motor);
    motor_stop(&my_oruga.right_motor);
}