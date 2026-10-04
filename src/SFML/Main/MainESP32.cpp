#include "esp_littlefs.h"
#include <filesystem>
#include "esp_log.h"
#include "esp32-hal-cpu.h"

void init()
{
    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/littlefs",
        .partition_label = "littlefs",
        .format_if_mount_failed = true,
        .dont_mount = false,
    };

    ESP_ERROR_CHECK(esp_vfs_littlefs_register(&conf));
    setCpuFrequencyMhz(80);
}

extern int main(int argc, char* argv[]);

extern "C"
{
#include "nvs_flash.h"
#ifndef main_app
#define main_app void app_main
#endif
    main_app()
    {
        nvs_flash_init();
        init();
        main(0, nullptr);
    }
}
