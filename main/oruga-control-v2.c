// main.c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "motor_driver.h"

void app_main() {
    // GPIOS
    int left_mina = 26;
    int left_minb = 27;
    int left_pwm = 13;
    int right_mina = 17;
    int right_minb = 16;
    int right_pwm = 4;

    // Create an instance of Oruga
    Oruga my_oruga;

    // Corrected calls to init, start, and stop functions
    oruga_init(&my_oruga, left_mina, left_minb, left_pwm, right_mina, right_minb, right_pwm);

    // Testing part
    int vel = 2048; // 0 - 4096
    move_forward(&my_oruga, vel);
    move_backward(&my_oruga, vel);
    turn_right(&my_oruga, vel);
    turn_left(&my_oruga, vel);

    // Stop the motors
    motor_stop(&my_oruga.left_motor);
    motor_stop(&my_oruga.right_motor);
}