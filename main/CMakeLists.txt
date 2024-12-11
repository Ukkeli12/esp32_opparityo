idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ""
                    REQUIRES esp_wifi
                    REQUIRES esp_http_server
                    REQUIRES nvs_flash
                    REQUIRES esp_driver_gpio
                    REQUIRES esp_https_server)
                        
                    