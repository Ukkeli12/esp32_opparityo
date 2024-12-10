
#include <stdio.h>
#include <inttypes.h>
#include "esp_wifi.h" //Esp wifi library
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
    wifi_init_config_t conf = WIFI_INIT_CONFIG_DEFAULT();

    wifi_country_t finland_country  = {
        .cc = "FI",
        .schan = 1

    };

    esp_wifi_init(&conf);
    esp_wifi_set_country(&finland_country); //Setting country to Finland
    


    wifi_mode_t mode_conf = WIFI_MODE_STA; //Setting wifi mode
    esp_wifi_set_mode(mode_conf);

    wifi_sta_config_t sta_setting = {
        .ssid = wifi_ssid,
        .password = wifi_password,
        //.threshold.authmode = WIFI_AUTH_OPEN //For open wifi
        .threshold.authmode = WIFI_AUTH_WPA2_PSK

    };

    esp_wifi_set_config(WIFI_IF_STA, &sta_setting);

    esp_wifi_start();
    esp_wifi_connect();


}




void app_main(void)
{   

    init_and_start_wifi();

    while(1){


        gpio_set_level(relay_port, 1); //Putting relay on

    }
    

}







