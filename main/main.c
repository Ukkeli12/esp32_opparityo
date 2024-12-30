
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


#define relay_port GPIO_NUM_12
#define wifi_ssid "Testi" //ssid for esp wifi
#define wifi_password "Testiverkko" //Password for esp wifi

//#define koulu_setup //school wifi setup

#define MIN(a, b) ((a) < (b) ? (a) : (b))


//Login html starts
static const char *TAG = "LoginServer";

// Define credentials
const char *valid_username = "admin";
const char *valid_password = "password";
//Login html stops 

esp_err_t login_page_handler(httpd_req_t *req) {
    const char *html_response = 
        "<!DOCTYPE html>"
        "<html>"
        "<head><title>Login</title></head>"
        "<body>"
        "<h2>ESP32 Login</h2>"
        "<form action=\"/login\" method=\"POST\">"
        "<label for=\"username\">Username:</label><br>"
        "<input type=\"text\" id=\"username\" name=\"username\"><br><br>"
        "<label for=\"password\">Password:</label><br>"
        "<input type=\"password\" id=\"password\" name=\"password\"><br><br>"
        "<input type=\"submit\" value=\"Login\">"
        "</form>"
        "</body>"
        "</html>";
    
    httpd_resp_send(req, html_response, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

//HTTP STARTS


// Handle login form submission
esp_err_t login_handler(httpd_req_t *req) {
    char buf[100];
    int ret, remaining = req->content_len;

    char username[50] = {0};
    char password[50] = {0};

    // Parse the form data
    while (remaining > 0) {
        ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)));
        if (ret <= 0) {
            ESP_LOGE(TAG, "Error receiving data");
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        buf[ret] = '\0';
        remaining -= ret;
    }

    // Extract username and password from the form data
    sscanf(buf, "username=%[^&]&password=%s", username, password);
    ESP_LOGI(TAG, "Received username: %s, password: %s", username, password);

    // Check credentials
    if (strcmp(username, valid_username) == 0 && strcmp(password, valid_password) == 0) {
        const char *success_response = "Login successful!";
        httpd_resp_send(req, success_response, HTTPD_RESP_USE_STRLEN);
    } else {
        const char *failure_response = "Invalid credentials. Please try again.";
        httpd_resp_send(req, failure_response, HTTPD_RESP_USE_STRLEN);
    }
    return ESP_OK;
}


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


    return server;
}

//HTTP ENDS

void init_and_start_wifi(){
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    
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
            
            #if koulu_setup //Setting schools network
            .ssid = "Panoulu",
            #else
            .ssid = wifi_ssid,
            .password = wifi_password,
            #endif
        }

    };

    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_settings);


    esp_wifi_start();
    esp_wifi_connect();



}




void app_main(void)
{   

    init_and_start_wifi(); //Starting wifi

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







