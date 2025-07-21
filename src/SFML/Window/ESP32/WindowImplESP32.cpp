////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2013 Jonathan De Wachter (dewachter.jonathan@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window/ESP32/WindowImplESP32.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/WindowEnums.hpp>

#include <SFML/System/Err.hpp>

void ESPCreateWindow(uint16_t width, uint16_t height);

#include <mutex>
#include <ostream>

////////////////////////////////////////////////////////////
// Private data
////////////////////////////////////////////////////////////
namespace sf::priv
{

////////////////////////////////////////////////////////////
WindowImplESP32::WindowImplESP32(WindowHandle /* handle */)
{
}


////////////////////////////////////////////////////////////
WindowImplESP32::WindowImplESP32(VideoMode mode,
                                     const String& /* title */,
                                     std::uint32_t /* style */,
                                     State state,
                                     const ContextSettings& /* settings */) :
m_size(mode.size)
{
    ESPCreateWindow(mode.size.x, mode.size.y);
}


////////////////////////////////////////////////////////////
WindowImplESP32::~WindowImplESP32()
{
}


////////////////////////////////////////////////////////////
WindowHandle WindowImplESP32::getNativeHandle() const
{
    return nullptr;
}


////////////////////////////////////////////////////////////
void WindowImplESP32::processEvents()
{
    return;
}


////////////////////////////////////////////////////////////
Vector2i WindowImplESP32::getPosition() const
{
    // Not applicable
    return {};
}


////////////////////////////////////////////////////////////
void WindowImplESP32::setPosition(Vector2i /* position */)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
Vector2u WindowImplESP32::getSize() const
{
    return m_size;
}


////////////////////////////////////////////////////////////
void WindowImplESP32::setSize(Vector2u /* size */)
{
}


////////////////////////////////////////////////////////////
void WindowImplESP32::setMinimumSize(const std::optional<Vector2u>& /* minimumSize */)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void WindowImplESP32::setMaximumSize(const std::optional<Vector2u>& /* maximumSize */)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void WindowImplESP32::setTitle(const String& /* title */)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void WindowImplESP32::setIcon(Vector2u /* size */, const std::uint8_t* /* pixels */)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void WindowImplESP32::setVisible(bool /* visible */)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void WindowImplESP32::setMouseCursorVisible(bool /* visible */)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void WindowImplESP32::setMouseCursorGrabbed(bool /* grabbed */)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void WindowImplESP32::setMouseCursor(const CursorImpl& /* cursor */)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void WindowImplESP32::setKeyRepeatEnabled(bool /* enabled */)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void WindowImplESP32::requestFocus()
{
    // Not applicable
}


////////////////////////////////////////////////////////////
bool WindowImplESP32::hasFocus() const
{
    return m_hasFocus;
}


////////////////////////////////////////////////////////////
void WindowImplESP32::forwardEvent(const Event& event)
{
    return;
}


} // namespace sf::priv
