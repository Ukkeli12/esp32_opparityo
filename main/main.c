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
#include "esp_task_wdt.h"
#include "mbedtls/base64.h"

#define relay_port GPIO_NUM_32 //Port of reley that used in program
#define wifi_ssid "Jotakin" //ssid for esp wifi
#define wifi_password "ToomiHan0" //Password for esp wifi

//#define school_setup //school wifi setup

//HTTP login starts
const char *username = "user";
const char *password = "password";


static const char *TAG = "LED_Control";

void generate_base64_auth(char *out_buf, size_t out_buf_len) {
    char auth_str[128];
    snprintf(auth_str, sizeof(auth_str), "%s:%s", username, password);

    size_t output_len;
    mbedtls_base64_encode((unsigned char *)out_buf, out_buf_len, &output_len, 
                          (const unsigned char *)auth_str, strlen(auth_str));
}



//HTTP login stops

void initialize_led(){ //Setting led to work
    gpio_reset_pin(relay_port);
    gpio_set_direction(relay_port, GPIO_MODE_OUTPUT);
    gpio_set_level(relay_port, 0); //Putting relay off
}

//HTTP STARTS


esp_err_t root_get_handler(httpd_req_t *req) {
    const char *response = "<!DOCTYPE html>"
                            "<html>"
                            "<body>"
                            "<h1>ESP32 Web Server</h1>"
                            "<p>Petterin verkkosivusto</p>"
                            "<button onclick=\"fetch('/led?state=on')\">Turn ON</button>"
                            "<button onclick=\"fetch('/led?state=off')\">Turn OFF</button>"
                            "</body>"
                            "</html>";

    const char *expected_auth = "Basic dXNlcjpwYXNzd29yZA=="; // Base64 of "user:password"


    // Retrieve Authorization header
    char auth_value[128];
    if (httpd_req_get_hdr_value_str(req, "Authorization", auth_value, sizeof(auth_value)) == ESP_OK) {
        if (strcmp(auth_value, expected_auth) == 0) {
            // Authorized
            httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);
            return ESP_OK;
        }
    }


    // Unauthorized
    httpd_resp_set_status(req, "401 Unauthorized");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"ESP32\"");
    httpd_resp_send(req, "Unauthorized", HTTPD_RESP_USE_STRLEN);
    return ESP_ERR_ESP_NETIF_IF_NOT_READY; 
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


//LED handler starts
esp_err_t led_get_handler(httpd_req_t *req) {
    char* buf;
    size_t buf_len = httpd_req_get_url_query_len(req) + 1;

    if (buf_len > 1) {
        buf = malloc(buf_len);
        httpd_req_get_url_query_str(req, buf, buf_len);

        char param[32];
        if (httpd_query_key_value(buf, "state", param, sizeof(param)) == ESP_OK) {
            ESP_LOGI(TAG, "LED state: %s", param);

            if (strcmp(param, "on") == 0) {
                gpio_set_level(relay_port, 1); // Turn ON LED
            } else if (strcmp(param, "off") == 0) {
                gpio_set_level(relay_port, 0); // Turn OFF LED
            }
        }
        free(buf);
    }

    httpd_resp_send(req, "OK", strlen("OK"));
    return ESP_OK;
}

//LED handler ends





static httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    /*if (httpd_start(&server, &config) == ESP_OK) {
        return server;
    }
        return NULL;
        */

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t root_uri = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_get_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &root_uri);

    httpd_uri_t led_uri = {
            .uri = "/led",
            .method = HTTP_GET,
            .handler = led_get_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &led_uri);
    }
    return server;
}


//HTTP ENDS
// Wi-Fi event handler starts
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        esp_netif_ip_info_t* ip_info = (esp_netif_ip_info_t*)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&ip_info->ip));
    }
}
// Wi-Fi event handler ends



void init_and_start_wifi(){
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
   
    
    wifi_init_config_t conf = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&conf);

        esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip);

    wifi_config_t wifi_config_settings = {
        .sta = {
            
            #if school_setup //Setting schools network
            .ssid = "Panoulu",
            #else
            .ssid = wifi_ssid,
            .password = wifi_password,
            #endif
        }

    };

    wifi_mode_t mode_conf = WIFI_MODE_STA; //Setting wifi mode
    esp_wifi_set_mode(mode_conf);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_settings);

    esp_wifi_start();
    esp_wifi_connect();



}




void app_main(void)
{   
    initialize_led(); //Initalize leds
    init_and_start_wifi(); //Starting wifi
    
    //Start webserver begin
    httpd_handle_t server = start_webserver();
    if (server) {
        register_uri_handlers(server);
    }
    //End webserver begin


}







