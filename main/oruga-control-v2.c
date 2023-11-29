// main.c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "motor_driver.h"

#include "string.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_http_server.h"

// Define commands
#define CMD_MOVE_FORWARD "MoveForward"
#define CMD_MOVE_BACKWARD "MoveBackward"
#define CMD_TURN_LEFT "TurnLeft"
#define CMD_TURN_RIGHT "TurnRight"
// Testing parameters
const int vel = 2048; // 0 - 4096
// Create an instance of Oruga
Oruga my_oruga;

extern const char index_start[] asm("_binary_index_html_start");
extern const char index_end[] asm("_binary_index_html_end");

static esp_err_t api_get_handler(httpd_req_t *req) {
    char* buf;
    size_t buf_len;

    buf_len = httpd_req_get_url_query_len(req) + 1;

    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            char param[32];

            if (httpd_query_key_value(buf, "cmd", param, sizeof(param)) == ESP_OK) {
                if (strcmp(param, CMD_MOVE_FORWARD) == 0) {
                    move_forward(&my_oruga, vel);
                } else if (strcmp(param, CMD_MOVE_BACKWARD) == 0) {
                    move_backward(&my_oruga, vel);
                } else if (strcmp(param, CMD_TURN_LEFT) == 0) {
                    turn_left(&my_oruga, vel);
                } else if (strcmp(param, CMD_TURN_RIGHT) == 0) {
                    turn_right(&my_oruga, vel);
                } else if (strcmp(param, "Stop") == 0) {
                    stop_oruga(&my_oruga);
                }
            }
        }
        free(buf);
    }

    // Send a simple response (optional)
    httpd_resp_send(req, "Command received", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t home_get_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    const uint32_t index_len = index_end - index_start;
    httpd_resp_send(req, index_start, index_len);
    return ESP_OK;
}

static const httpd_uri_t api = {
    .uri = "/api",
    .method = HTTP_GET,
    .handler = api_get_handler
};

static const httpd_uri_t home = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = home_get_handler
};

void web_server_init() {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &api);
        httpd_register_uri_handler(server, &home);
        return;
    }
    printf("Error al iniciar servidor\n");
}


void app_main() {

    // ------------------------------ START ORUGA -----------------------------------

    // GPIOS
    int left_mina = 26;
    int left_minb = 27;
    int left_pwm = 13;
    int right_mina = 17;
    int right_minb = 16;
    int right_pwm = 4;

    // Initialize oruga
    oruga_init(&my_oruga, left_mina, left_minb, left_pwm, right_mina, right_minb, right_pwm);


    // Handle memory and connections
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    example_connect();

    esp_netif_ip_info_t ip_info;
    esp_netif_t* netif = NULL;
    netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");

    if (netif == NULL) {
        printf("No hay interfaz\n");
    } else {
        esp_netif_get_ip_info(netif, &ip_info);
        printf("IP: %d.%d.%d.%d\n", IP2STR(&ip_info.ip));

        // ------------------------------ START SERVER -----------------------------------
        web_server_init();
    }
}