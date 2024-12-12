
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

#define relay_port GPIO_NUM_12
#define wifi_ssid "Testi" //ssid for esp wifi
#define wifi_password "Testisalasana" //Password for esp wifi


void init_and_start_wifi(){
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    esp_wifi_stop();
    esp_wifi_deinit();

    esp_err_t ret;
    
    wifi_init_config_t conf = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&conf);
    ESP_ERROR_CHECK(ret);

    /*wifi_country_t finland_country  = {
        .cc = "FI" ,
        .schan = 1
        
    };
    esp_wifi_set_country(&finland_country); //Setting country to Finland
    */


    wifi_mode_t mode_conf = WIFI_MODE_STA; //Setting wifi mode
    ret = esp_wifi_set_mode(mode_conf);
    ESP_ERROR_CHECK(ret);


    wifi_config_t wifi_config_settings = {
        .sta = {
            .ssid = wifi_ssid,
            .password = wifi_password,
        }

    };

    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_settings));


    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
    ESP_ERROR_CHECK(ret);


}




void app_main(void)
{   

    init_and_start_wifi();

    while(1){

        gpio_set_level(relay_port, 1); //Putting relay on

    }     

}







