set(INCLUDE "." "./ai_chat" "./ui" "./ui/generated" "./ui/custom" "./ui/generated/guider_customer_fonts" "./ui/generated/guider_fonts" "./ui/generated/images")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS ${INCLUDE}
                    PRIV_REQUIRES display esp_timer nvs_flash ap_wifi app_update esp_http_server esp_http_client esp-tls esp_adc esp_driver_gpio esp_driver_uart esp_wifi fatfs esp_driver_sdspi lvgl__lvgl)

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/audio_decoder/third_party_decoders.c PROPERTIES COMPILE_FLAGS "-w")
                  
spiffs_create_partition_image(html ../html FLASH_IN_PROJECT)
spiffs_create_partition_image(img ../img FLASH_IN_PROJECT)
