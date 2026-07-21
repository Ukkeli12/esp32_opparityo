idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ""
                    REQUIRES esp_wifi
                    REQUIRES esp_http_server
                    REQUIRES nvs_flash
                    REQUIRES esp_driver_gpio
                    REQUIRES esp_https_server)
                        
target_add_binary_data(${COMPONENT_TARGET} server.crt TEXT) 
target_add_binary_data(${COMPONENT_TARGET} server.key TEXT)