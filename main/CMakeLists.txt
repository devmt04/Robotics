idf_component_register(SRCS "main.c" "common/common.c" "motor/motors.c" "wifi/wifi.c" "http/http.c" "Engine/engine.c" "ir/ir.c" "mic/mic.c"
                    INCLUDE_DIRS ".")


#PUT a cmakelist file in each directory

                    # file(GLOB_RECURSE SOURCES "*.c" "*.cpp")

# idf_component_register(SRCS ${SOURCES}
#                        INCLUDE_DIRS "."
#                        REQUIRES esp_http_server esp_wifi)

