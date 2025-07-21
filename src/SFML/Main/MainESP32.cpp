#include "ABCOS.h"

void init()
{
    // OS::First();
    // OS::InitSD();
    // OS::InitOS();
    setCpuFrequencyMhz(240);
    ESP_LOGE("SFML environment", "initialized");
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
