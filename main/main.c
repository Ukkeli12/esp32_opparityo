
#include <stdio.h>
#include <inttypes.h>
#include "esp_wifi.h" //Esp wifi library
#include "esp_http_server.h" //http server library for esp32


void init_wifi();

void app_main(void)
{   
    init_wifi();
    printf("Hello world!\n");

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

/*
void start_http_server(){


}


void stop_http_server(){

}
*/