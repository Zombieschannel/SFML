#include <SFML/System/Time.hpp>
#include <SFML/System/ESP32/SleepImpl.hpp>

#include "esp_rom_sys.h"

#include <cerrno>
#include <ctime>


namespace sf::priv
{

void sleepImpl(Time time)
{
    esp_rom_delay_us(time.asMicroseconds());
}

}
