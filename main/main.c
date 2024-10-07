
#include <stdio.h>
#include <inttypes.h>
#include "esp_wifi.h" //Esp wifi library
#include "esp_http_server.h" //http server library for esp32
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "driver/gpio.h"


#define relay GPIO_NUM_12
#define esp_wifi_ssid "Testi"
#define esp_wifi_pass "Testisalasana"


void init_wifi();

void app_main(void)
{   
    init_wifi();
    printf("Hello world!\n");

}



static httpd_handle_t start_http_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the HTTP server
    ESP_LOGI(TAG, "Starting server on port %d", config.server_port);
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK) {
        // Register URI handlers
        httpd_register_uri_handler(server, &root_uri);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}


void init_wifi(){
    wifi_init_config_t wifi_init_default_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_default_config));

    wifi_mode_t station_mode = WIFI_MODE_STA; 
    ESP_ERROR_CHECK(esp_wifi_set_mode(station_mode));
    wifi_config_t sta_config = {
        .sta = {
            .ssid = "Testi",
            .password = "testisalasana"
        
        },
        

    };
    sta_config.sta.bssid_set = false;

    //esp_wifi_connect();
}


static void stop_http_server(httpd_handle_t server){
    if (server) {
        httpd_stop(server);
    }
}
