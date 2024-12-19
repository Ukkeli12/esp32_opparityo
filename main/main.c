
#include <stdio.h>
#include <inttypes.h>
#include "esp_wifi.h" //Esp wifi library
#include "esp_https_server.h" //https server library for esp32
#include "esp_http_server.h" //http server library for esp32
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "esp_check.h"
#include "esp_mac.h"
#include <esp_wifi_types_generic.h>
#include "esp_http_server.h"
#include "esp_task_wdt.h"


#define relay_port GPIO_NUM_12
#define wifi_ssid "Testi" //ssid for esp wifi
#define wifi_password "Testiverkko" //Password for esp wifi


//Login html starts
static const char *TAG = "LoginServer";

// Define credentials
const char *valid_username = "admin";
const char *valid_password = "password";
//Login html stops 



//HTTP STARTS
esp_err_t root_get_handler(httpd_req_t *req) {
    const char *response = "<!DOCTYPE html><html><body><h1>ESP32 Web Server</h1><p>Petterin verkkosivusto</p><button type=\"button\">ON</button><button type=\"button\">OFF</button></body></html>";

    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void register_uri_handlers(httpd_handle_t server) {
    httpd_uri_t root_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = root_get_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &root_uri);
}


static httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK) {
        return server;
    }


    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK) {
        // Register URI handlers
        httpd_uri_t login_page = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = login_page_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &login_page);


        httpd_uri_t login_post = {
            .uri       = "/login",
            .method    = HTTP_POST,
            .handler   = login_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &login_post);
    }


    return NULL;
}

//HTTP ENDS

void init_and_start_wifi(){
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    /*esp_wifi_stop();
    esp_wifi_deinit();
    */
    
    wifi_init_config_t conf = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&conf);


    /*wifi_country_t finland_country  = {
        .cc = "FI" ,
        .schan = 1
        
    };
    esp_wifi_set_country(&finland_country); //Setting country to Finland
    */


    wifi_mode_t mode_conf = WIFI_MODE_STA; //Setting wifi mode
    esp_wifi_set_mode(mode_conf);


    wifi_config_t wifi_config_settings = {
        .sta = {
            .ssid = wifi_ssid,
            .password = wifi_password,
        }

    };

    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_settings);


    esp_wifi_start();
    esp_wifi_connect();



}




void app_main(void)
{   

    init_and_start_wifi();

    //Start webserver begin
    httpd_handle_t server = start_webserver();
    if (server) {
        register_uri_handlers(server);
    }
    //End webserver begin

    /*while(1){
        //esp_task_wdt_reset();
        gpio_set_level(relay_port, 1); //Putting relay on

    }     */

}







