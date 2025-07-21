#pragma once

#include <SFML/Config.hpp>


namespace sf
{
class Time;
}

namespace sf::priv
{
void sleepImpl(Time time);
}
