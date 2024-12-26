////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2015 Laurent Gomila (laurent@sfml-dev.org)
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
#include <SFML/Window/Emscripten/WindowImplEmscripten.hpp>
#include <SFML/Window/Emscripten/JoystickImpl.hpp>
#include <SFML/Window/VideoModeImpl.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Err.hpp>
#include <emscripten/html5.h>
#include <emscripten.h>
#include <vector>
#include <cmath>
#include <map>


namespace
{
    sf::priv::WindowImplEmscripten* window = nullptr;
    bool windowHasFocus = false;
    bool fullscreenPending = false;

    bool keyStatus[sf::Keyboard::KeyCount] = { false };
    bool keyStatusInitialized = false;
    bool mouseStatus[sf::Mouse::ButtonCount] = { false };
    bool mouseStatusInitialized = false;
    sf::Vector2i mousePosition;
    std::map<unsigned int, sf::Vector2i> touchStatus;
    bool joysticksConnected[sf::Joystick::Count];

    sf::Keyboard::Key keyCodeToSF(unsigned long key, unsigned long location)
    {
        switch (key)
        {
            case '\b': return sf::Keyboard::Key::Backspace;
            case '\t': return sf::Keyboard::Key::Tab;

            case '\r':
            {
                if (location == DOM_KEY_LOCATION_STANDARD)
                    return sf::Keyboard::Key::Enter;
                else if (location == DOM_KEY_LOCATION_NUMPAD)
                    return sf::Keyboard::Key::Enter;
                break;
            }

            case 16:
            {
                if (location == DOM_KEY_LOCATION_LEFT)
                    return sf::Keyboard::Key::LShift;
                else if (location == DOM_KEY_LOCATION_RIGHT)
                    return sf::Keyboard::Key::RShift;
                break;
            }

            case 17:
            {
                if (location == DOM_KEY_LOCATION_LEFT)
                    return sf::Keyboard::Key::LControl;
                else if (location == DOM_KEY_LOCATION_RIGHT)
                    return sf::Keyboard::Key::RControl;
                break;
            }

            case 18:
            {
                if (location == DOM_KEY_LOCATION_LEFT)
                    return sf::Keyboard::Key::LAlt;
                else if (location == DOM_KEY_LOCATION_RIGHT)
                    return sf::Keyboard::Key::RAlt;
                break;
            }

            case 19:  return sf::Keyboard::Key::Pause;

            // case 20: Caps Lock

            case 27:  return sf::Keyboard::Key::Escape;

            case ' ': return sf::Keyboard::Key::Space;
            case 33:  return sf::Keyboard::Key::PageUp;
            case 34:  return sf::Keyboard::Key::PageDown;
            case 35:  return sf::Keyboard::Key::End;
            case 36:  return sf::Keyboard::Key::Home;
            case 37:  return sf::Keyboard::Key::Left;
            case 39:  return sf::Keyboard::Key::Right;
            case 38:  return sf::Keyboard::Key::Up;
            case 40:  return sf::Keyboard::Key::Down;

            // case 42: Print Screen

            case 45:  return sf::Keyboard::Key::Insert;
            case 46:  return sf::Keyboard::Key::Delete;

            case ';': return sf::Keyboard::Key::Semicolon;

            case '=': return sf::Keyboard::Key::Equal;

            case 'A': return sf::Keyboard::Key::A;
            case 'Z': return sf::Keyboard::Key::Z;
            case 'E': return sf::Keyboard::Key::E;
            case 'R': return sf::Keyboard::Key::R;
            case 'T': return sf::Keyboard::Key::T;
            case 'Y': return sf::Keyboard::Key::Y;
            case 'U': return sf::Keyboard::Key::U;
            case 'I': return sf::Keyboard::Key::I;
            case 'O': return sf::Keyboard::Key::O;
            case 'P': return sf::Keyboard::Key::P;
            case 'Q': return sf::Keyboard::Key::Q;
            case 'S': return sf::Keyboard::Key::S;
            case 'D': return sf::Keyboard::Key::D;
            case 'F': return sf::Keyboard::Key::F;
            case 'G': return sf::Keyboard::Key::G;
            case 'H': return sf::Keyboard::Key::H;
            case 'J': return sf::Keyboard::Key::J;
            case 'K': return sf::Keyboard::Key::K;
            case 'L': return sf::Keyboard::Key::L;
            case 'M': return sf::Keyboard::Key::M;
            case 'W': return sf::Keyboard::Key::W;
            case 'X': return sf::Keyboard::Key::X;
            case 'C': return sf::Keyboard::Key::C;
            case 'V': return sf::Keyboard::Key::V;
            case 'B': return sf::Keyboard::Key::B;
            case 'N': return sf::Keyboard::Key::N;
            case '0': return sf::Keyboard::Key::Num0;
            case '1': return sf::Keyboard::Key::Num1;
            case '2': return sf::Keyboard::Key::Num2;
            case '3': return sf::Keyboard::Key::Num3;
            case '4': return sf::Keyboard::Key::Num4;
            case '5': return sf::Keyboard::Key::Num5;
            case '6': return sf::Keyboard::Key::Num6;
            case '7': return sf::Keyboard::Key::Num7;
            case '8': return sf::Keyboard::Key::Num8;
            case '9': return sf::Keyboard::Key::Num9;

            case 91:
            {
                if (location == DOM_KEY_LOCATION_LEFT)
                    return sf::Keyboard::Key::LSystem;
                else if (location == DOM_KEY_LOCATION_RIGHT)
                    return sf::Keyboard::Key::RSystem;
                break;
            }

            case 93:  return sf::Keyboard::Key::Menu;

            case 96:  return sf::Keyboard::Key::Numpad0;
            case 97:  return sf::Keyboard::Key::Numpad1;
            case 98:  return sf::Keyboard::Key::Numpad2;
            case 99:  return sf::Keyboard::Key::Numpad3;
            case 100: return sf::Keyboard::Key::Numpad4;
            case 101: return sf::Keyboard::Key::Numpad5;
            case 102: return sf::Keyboard::Key::Numpad6;
            case 103: return sf::Keyboard::Key::Numpad7;
            case 104: return sf::Keyboard::Key::Numpad8;
            case 105: return sf::Keyboard::Key::Numpad9;

            case 106: return sf::Keyboard::Key::Multiply;
            case 107: return sf::Keyboard::Key::Add;
            case 109: return sf::Keyboard::Key::Subtract;
            case 111: return sf::Keyboard::Key::Divide;

            case 112: return sf::Keyboard::Key::F1;
            case 113: return sf::Keyboard::Key::F2;
            case 114: return sf::Keyboard::Key::F3;
            case 115: return sf::Keyboard::Key::F4;
            case 116: return sf::Keyboard::Key::F5;
            case 117: return sf::Keyboard::Key::F6;
            case 118: return sf::Keyboard::Key::F7;
            case 119: return sf::Keyboard::Key::F8;
            case 120: return sf::Keyboard::Key::F9;
            case 121: return sf::Keyboard::Key::F10;
            case 122: return sf::Keyboard::Key::F11;
            case 123: return sf::Keyboard::Key::F12;
            case 124: return sf::Keyboard::Key::F13;
            case 125: return sf::Keyboard::Key::F14;
            case 126: return sf::Keyboard::Key::F15;

            // case 144: Num Lock
            // case 145: Scroll Lock

            case 173: return sf::Keyboard::Key::Hyphen;

            case 188: return sf::Keyboard::Key::Comma;

            case 190: return sf::Keyboard::Key::Period;
            case 191: return sf::Keyboard::Key::Slash;
            case 192: return sf::Keyboard::Key::Grave;

            case 219: return sf::Keyboard::Key::LBracket;
            case 220: return sf::Keyboard::Key::Backslash;
            case 221: return sf::Keyboard::Key::RBracket;
            case 222: return sf::Keyboard::Key::Apostrophe;
        }

        return sf::Keyboard::Key::Unknown;
    }

    void updatePluggedList()
    {
        int numJoysticks = emscripten_get_num_gamepads();

        if (numJoysticks == EMSCRIPTEN_RESULT_NOT_SUPPORTED)
        {
            for (int i = 0; i < static_cast<int>(sf::Joystick::Count); ++i)
            {
                joysticksConnected[i] = false;
            }

            return;
        }

        for (int i = 0; (i < static_cast<int>(sf::Joystick::Count)) && (i < numJoysticks); ++i)
        {
            EmscriptenGamepadEvent gamepadEvent;
            if (emscripten_get_gamepad_status(i, &gamepadEvent) != EMSCRIPTEN_RESULT_SUCCESS)
            {
                sf::err() << "Failed to get status of gamepad " << i << std::endl;
                joysticksConnected[i] = false;
                continue;
            }

            if (gamepadEvent.connected)
                joysticksConnected[i] = true;
            else
                joysticksConnected[i] = false;
        }
    }
    
    EM_BOOL canvasSizeChangedCallback(int /* eventType */, const void* /* reserved */, void* /* userData */)
    {
        if (!window)
            return 0;

        int width{};
        int height{};
        emscripten_get_canvas_element_size("#canvas", &width, &height);

        sf::Event::Resized event;
        event.size.x = static_cast<unsigned int>(width);
        event.size.y = static_cast<unsigned int>(height);
        window->pushHtmlEvent(event);

        return 0;
    }

    void requestFullscreen()
    {
        EmscriptenFullscreenStrategy fullscreenStrategy;

        fullscreenStrategy.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_STRETCH;
        fullscreenStrategy.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_HIDEF;
        fullscreenStrategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_BILINEAR;
        fullscreenStrategy.canvasResizedCallback = canvasSizeChangedCallback;
        fullscreenStrategy.canvasResizedCallbackUserData = 0;

        emscripten_request_fullscreen_strategy("#canvas", 0, &fullscreenStrategy);
    }

    EM_BOOL keyCallback(int eventType, const EmscriptenKeyboardEvent* e, void* /* userData */)
    {
        if (!window)
            return 0;

        sf::Keyboard::Key key = keyCodeToSF(e->which, e->location);

        switch (eventType)
        {
            case EMSCRIPTEN_EVENT_KEYDOWN:
            {
                if (e->repeat && !window->getKeyRepeatEnabled())
                    return 1;

                if (fullscreenPending)
                {
                    requestFullscreen();
                    fullscreenPending = false;
                }

                keyStatus[static_cast<int>(key)] = true;

                sf::Event::KeyPressed event;
                event.alt     = e->altKey   != 0;
                event.control = e->ctrlKey  != 0;
                event.shift   = e->shiftKey != 0;
                event.system  = e->metaKey  != 0;
                event.code    = key;
                window->pushHtmlEvent(event);

                // We try to prevent some keystrokes from bubbling
                // If we try to prevent bubbling for all keys,
                // it prevents keypress events from being generated
                if ((key == sf::Keyboard::Key::Tab) ||
                    (key == sf::Keyboard::Key::Backspace) ||
                    (key == sf::Keyboard::Key::Menu) ||
                    (key == sf::Keyboard::Key::LSystem) ||
                    (key == sf::Keyboard::Key::RSystem))
                    return 1;

                return 0;
            }
            case EMSCRIPTEN_EVENT_KEYUP:
            {
                keyStatus[static_cast<int>(key)] = false;

                sf::Event::KeyReleased event;
                event.alt     = e->altKey   != 0;
                event.control = e->ctrlKey  != 0;
                event.shift   = e->shiftKey != 0;
                event.system  = e->metaKey  != 0;
                event.code    = key;
                window->pushHtmlEvent(event);
                return 1;
            }
            case EMSCRIPTEN_EVENT_KEYPRESS:
            {
                if (e->charCode == 0)
                    return 1;

                sf::Event::TextEntered event;
                event.unicode = e->charCode;
                window->pushHtmlEvent(event);

                return 1;
            }
            default:
            {
                break;
            }
        }

        return 0;
    }

    EM_BOOL mouseCallback(int eventType, const EmscriptenMouseEvent* e, void* /* userData */)
    {

        if (!window)
            return 0;

        mousePosition.x = e->targetX;
        mousePosition.y = e->targetY;

        switch (eventType)
        {
            case EMSCRIPTEN_EVENT_MOUSEDOWN:
            {
                sf::Event::MouseButtonPressed event;
                event.position.x      = e->targetX;
                event.position.y      = e->targetY;

                if (fullscreenPending)
                {
                    requestFullscreen();
                    fullscreenPending = false;
                }

                if (e->button == 0)
                {
                    event.button = sf::Mouse::Button::Left;
                    mouseStatus[static_cast<int>(sf::Mouse::Button::Left)] = true;
                }
                else if (e->button == 1)
                {
                    event.button = sf::Mouse::Button::Middle;
                    mouseStatus[static_cast<int>(sf::Mouse::Button::Middle)] = true;
                }
                else if (e->button == 2)
                {
                    event.button = sf::Mouse::Button::Right;
                    mouseStatus[static_cast<int>(sf::Mouse::Button::Right)] = true;
                }
                else if (e->button == 3)
                {
                    event.button = sf::Mouse::Button::Extra1;
                    mouseStatus[static_cast<int>(sf::Mouse::Button::Extra1)] = true;
                }
                else if (e->button == 4)
                {
                    event.button = sf::Mouse::Button::Extra2;
                    mouseStatus[static_cast<int>(sf::Mouse::Button::Extra2)] = true;
                }

                window->pushHtmlEvent(event);
                return 1;
            }
            case EMSCRIPTEN_EVENT_MOUSEUP:
            {
                sf::Event::MouseButtonReleased event;
                event.position.x = e->targetX;
                event.position.y = e->targetY;

                if (e->button == 0)
                {
                    event.button = sf::Mouse::Button::Left;
                    mouseStatus[static_cast<int>(sf::Mouse::Button::Left)] = false;
                }
                else if (e->button == 1)
                {
                    event.button = sf::Mouse::Button::Middle;
                    mouseStatus[static_cast<int>(sf::Mouse::Button::Middle)] = false;
                }
                else if (e->button == 2)
                {
                    event.button = sf::Mouse::Button::Right;
                    mouseStatus[static_cast<int>(sf::Mouse::Button::Right)] = false;
                }
                else if (e->button == 3)
                {
                    event.button = sf::Mouse::Button::Extra1;
                    mouseStatus[static_cast<int>(sf::Mouse::Button::Extra1)] = true;
                }
                else if (e->button == 4)
                {
                    event.button = sf::Mouse::Button::Extra2;
                    mouseStatus[static_cast<int>(sf::Mouse::Button::Extra2)] = true;
                }

                window->pushHtmlEvent(event);
                return 1;
            }
            case EMSCRIPTEN_EVENT_MOUSEMOVE:
            {
                sf::Event::MouseMoved event;
                event.position.x = e->targetX;
                event.position.y = e->targetY;
                window->pushHtmlEvent(event);
                return 1;
            }
            case EMSCRIPTEN_EVENT_MOUSEENTER:
            {
                sf::Event::MouseEntered event;
                window->pushHtmlEvent(event);
                return 1;
            }
            case EMSCRIPTEN_EVENT_MOUSELEAVE:
            {
                sf::Event::MouseLeft event;
                window->pushHtmlEvent(event);
                return 1;
            }
            default:
            {
                break;
            }
        }

        return 0;
    }

    EM_BOOL wheelCallback(int eventType, const EmscriptenWheelEvent* e, void* /* userData */)
    {
        if (!window)
            return 0;

        switch (eventType)
        {
            case EMSCRIPTEN_EVENT_WHEEL:
            {
                if (std::fabs(e->deltaY) > 0.0)
                {
                    sf::Event::MouseWheelScrolled event;

                    event.wheel = sf::Mouse::Wheel::Vertical;

                    switch (e->deltaMode)
                    {
                    case DOM_DELTA_PIXEL: event.delta = -static_cast<float>(e->deltaY) / 100.f; break;
                    case DOM_DELTA_LINE: event.delta = -static_cast<float>(e->deltaY) / 3.f; break;
                    case DOM_DELTA_PAGE: event.delta = -static_cast<float>(e->deltaY) * 80.f; break;
                    }
                    
                    event.position.x     = e->mouse.targetX;
                    event.position.y     = e->mouse.targetY;
                    window->pushHtmlEvent(event);
                }

                if (std::fabs(e->deltaX) > 0.0)
                {
                    sf::Event::MouseWheelScrolled event;

                    event.wheel = sf::Mouse::Wheel::Horizontal;
                    event.delta = static_cast<float>(e->deltaX);
                    event.position.x     = e->mouse.targetX;
                    event.position.y     = e->mouse.targetY;
                    window->pushHtmlEvent(event);
                }

                return 1;
            }
            default:
            {
                break;
            }
        }

        return 0;
    }

    EM_BOOL uieventCallback(int eventType, const EmscriptenUiEvent* /* e */, void* /* userData */)
    {
        if (!window)
            return 0;

        switch (eventType)
        {
            case EMSCRIPTEN_EVENT_RESIZE:
            {
                int width{};
                int height{};
                emscripten_get_canvas_element_size("#canvas", &width, &height);

                sf::Event::Resized event;
                event.size.x = static_cast<unsigned int>(width);
                event.size.y = static_cast<unsigned int>(height);
                window->pushHtmlEvent(event);

                return 1;
            }
            default:
            {
                break;
            }
        }

        return 0;
    }

    EM_BOOL focuseventCallback(int eventType, const EmscriptenFocusEvent* /* e */, void* /* userData */)
    {
        if (!window)
            return 0;

        switch (eventType)
        {
            case EMSCRIPTEN_EVENT_FOCUS:
            {
                sf::Event::FocusGained event;
                window->pushHtmlEvent(event);

                windowHasFocus = true;

                return 1;
            }
            case EMSCRIPTEN_EVENT_BLUR:
            {
                sf::Event::FocusLost event;
                window->pushHtmlEvent(event);

                windowHasFocus = false;

                return 1;
            }
            default:
            {
                break;
            }
        }

        return 0;
    }

    EM_BOOL deviceorientationCallback(int eventType, const EmscriptenDeviceOrientationEvent* /* e */, void* /* userData */)
    {
        if (!window)
            return 0;

        switch (eventType)
        {
            default:
                break;
        }

        return 0;
    }

    EM_BOOL devicemotionCallback(int eventType, const EmscriptenDeviceMotionEvent* /* e */, void* /* userData */)
    {
        if (!window)
            return 0;

        switch (eventType)
        {
            default:
                break;
        }

        return 0;
    }

    EM_BOOL orientationchangeCallback(int eventType, const EmscriptenOrientationChangeEvent* /* e */ , void* /* userData */)
    {
        if (!window)
            return 0;

        switch (eventType)
        {
            default:
                break;
        }

        return 0;
    }

    EM_BOOL fullscreenchangeCallback(int eventType, const EmscriptenFullscreenChangeEvent* /* e */, void* /* userData */)
    {
        if (!window)
            return 0;

        switch (eventType)
        {
            default:
                break;
        }

        return 0;
    }

    EM_BOOL pointerlockchangeCallback(int eventType, const EmscriptenPointerlockChangeEvent* /* e */, void* /* userData */)
    {
        if (!window)
            return 0;

        switch (eventType)
        {
            default:
                break;
        }

        return 0;
    }

    EM_BOOL visibilitychangeCallback(int eventType, const EmscriptenVisibilityChangeEvent* /* e */, void* /* userData */)
    {
        if (!window)
            return 0;

        switch (eventType)
        {
            case EMSCRIPTEN_VISIBILITY_UNLOADED:
            {
                sf::Event::Closed event;
                window->pushHtmlEvent(event);

                return 1;
            }
            default:
            {
                break;
            }
        }

        return 0;
    }

    EM_BOOL touchCallback(int eventType, const EmscriptenTouchEvent* e, void* /* userData */)
    {
        if (!window)
            return 0;

        switch (eventType)
        {
            case EMSCRIPTEN_EVENT_TOUCHSTART:
            {
                sf::Event::TouchBegan event;

                for (int i = 0; i < e->numTouches; ++i)
                {
                    event.finger = static_cast<unsigned int>(e->touches[i].identifier);
                    event.position.x      = e->touches[i].targetX;
                    event.position.y      = e->touches[i].targetY;
                    window->pushHtmlEvent(event);

                    touchStatus.insert(std::make_pair(static_cast<unsigned int>(e->touches[i].identifier), sf::Vector2i(e->touches[i].targetX, e->touches[i].targetY)));
                }

                return 1;
            }
            case EMSCRIPTEN_EVENT_TOUCHEND:
            {
                sf::Event::TouchEnded event;

                for (int i = 0; i < e->numTouches; ++i)
                {
                    event.finger = static_cast<unsigned int>(e->touches[i].identifier);
                    event.position.x      = e->touches[i].targetX;
                    event.position.y      = e->touches[i].targetY;
                    window->pushHtmlEvent(event);

                    touchStatus.erase(static_cast<unsigned int>(e->touches[i].identifier));
                }

                return 1;
            }
            case EMSCRIPTEN_EVENT_TOUCHMOVE:
            {
                sf::Event::TouchMoved event;

                for (int i = 0; i < e->numTouches; ++i)
                {
                    event.finger = static_cast<unsigned int>(e->touches[i].identifier);
                    event.position.x      = e->touches[i].targetX;
                    event.position.y      = e->touches[i].targetY;
                    window->pushHtmlEvent(event);

                    touchStatus[static_cast<unsigned int>(e->touches[i].identifier)] = sf::Vector2i(e->touches[i].targetX, e->touches[i].targetY);
                }

                return 1;
            }
            default:
            {
                break;
            }
        }

        return 0;
    }

    EM_BOOL gamepadCallback(int eventType, const EmscriptenGamepadEvent* /* e */, void* /* userData */)
    {
        switch (eventType)
        {
            case EMSCRIPTEN_EVENT_GAMEPADCONNECTED:
            {
                bool previousConnected[sf::Joystick::Count];
                std::memcpy(previousConnected, joysticksConnected, sizeof(previousConnected));

                updatePluggedList();

                if (window)
                {
                    for (int i = 0; i < static_cast<int>(sf::Joystick::Count); ++i)
                    {
                        if (!previousConnected[i] && joysticksConnected[i])
                        {
                            sf::Event::JoystickConnected event;
                            event.joystickId = static_cast<unsigned int>(i);
                            window->pushHtmlEvent(event);
                        }
                    }
                }

                return 1;
            }
            case EMSCRIPTEN_EVENT_GAMEPADDISCONNECTED:
            {
                bool previousConnected[sf::Joystick::Count];
                std::memcpy(previousConnected, joysticksConnected, sizeof(previousConnected));

                updatePluggedList();

                if (window)
                {
                    for (int i = 0; i < static_cast<int>(sf::Joystick::Count); ++i)
                    {
                        if (previousConnected[i] && !joysticksConnected[i])
                        {
                            sf::Event::JoystickDisconnected event;
                            event.joystickId = static_cast<unsigned int>(i);
                            window->pushHtmlEvent(event);
                        }
                    }
                }

                return 1;
            }
            default:
            {
                break;
            }
        }

        return 0;
    }

    void setCallbacks()
    {
        static bool callbacksSet = false;

        if (callbacksSet)
            return;

        if (emscripten_set_keypress_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, keyCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set keypress callback" << std::endl;

        if (emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, keyCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set keydown callback" << std::endl;

        if (emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, keyCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set keyup callback" << std::endl;

        if (emscripten_set_click_callback("#canvas", nullptr, true, mouseCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set click callback" << std::endl;

        if (emscripten_set_mousedown_callback("#canvas", nullptr, true, mouseCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set mousedown callback" << std::endl;

        if (emscripten_set_mouseup_callback("#canvas", nullptr, true, mouseCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set mouseup callback" << std::endl;

        if (emscripten_set_dblclick_callback("#canvas", nullptr, true, mouseCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set dblclick callback" << std::endl;

        if (emscripten_set_mousemove_callback("#canvas", nullptr, true, mouseCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set mousemove callback" << std::endl;

        if (emscripten_set_mouseenter_callback("#canvas", nullptr, true, mouseCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set mouseenter callback" << std::endl;

        if (emscripten_set_mouseleave_callback("#canvas", nullptr, true, mouseCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set mouseleave callback" << std::endl;

        if (emscripten_set_mouseover_callback("#canvas", nullptr, true, mouseCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set mouseover callback" << std::endl;

        if (emscripten_set_mouseout_callback("#canvas", nullptr, true, mouseCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set mouseout callback" << std::endl;

        if (emscripten_set_wheel_callback("#canvas", nullptr, true, wheelCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set wheel callback" << std::endl;

        if (emscripten_set_resize_callback("#canvas", nullptr, true, uieventCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set resize callback" << std::endl;

        if (emscripten_set_scroll_callback("#canvas", nullptr, true, uieventCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set scroll callback" << std::endl;

        if (emscripten_set_blur_callback("#canvas", nullptr, true, focuseventCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set blur callback" << std::endl;

        if (emscripten_set_focus_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, focuseventCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set focus callback" << std::endl;

        if (emscripten_set_focusin_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, focuseventCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set focusin callback" << std::endl;

        if (emscripten_set_focusout_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, focuseventCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set focusout callback" << std::endl;

        if (emscripten_set_deviceorientation_callback(nullptr, true, deviceorientationCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set deviceorientation callback" << std::endl;

        if (emscripten_set_devicemotion_callback(nullptr, true, devicemotionCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set devicemotion callback" << std::endl;

        if (emscripten_set_orientationchange_callback(nullptr, true, orientationchangeCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set orientationchange callback" << std::endl;

        if (emscripten_set_touchstart_callback("#canvas", nullptr, true, touchCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set touchstart callback" << std::endl;

        if (emscripten_set_touchend_callback("#canvas", nullptr, true, touchCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set touchend callback" << std::endl;

        if (emscripten_set_touchmove_callback("#canvas", nullptr, true, touchCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set touchmove callback" << std::endl;

        if (emscripten_set_touchcancel_callback("#canvas", nullptr, true, touchCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set touchcancel callback" << std::endl;

        if (emscripten_set_fullscreenchange_callback("#canvas", nullptr, true, fullscreenchangeCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set fullscreenchange callback" << std::endl;

        if (emscripten_set_pointerlockchange_callback("#canvas", nullptr, true, pointerlockchangeCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set pointerlockchange callback" << std::endl;

        if (emscripten_set_visibilitychange_callback(nullptr, true, visibilitychangeCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set visibilitychange callback" << std::endl;

        callbacksSet = true;
    }
}


namespace sf::priv
{
////////////////////////////////////////////////////////////
WindowImplEmscripten::WindowImplEmscripten(WindowHandle /* handle */) :
m_keyRepeatEnabled(true)
{
    err() << "Creating a window from a WindowHandle unsupported" << std::endl;
    std::abort();
}


////////////////////////////////////////////////////////////
WindowImplEmscripten::WindowImplEmscripten(VideoMode mode, const String& /* title */, std::uint32_t /* style */, State state, const ContextSettings& /* settings */) :
m_keyRepeatEnabled(true)
{
    if (window)
    {
        err() << "Creating multiple windows is unsupported" << std::endl;
        std::abort();
    }

    setCallbacks();

    window = this;

    setSize({ mode.size.x, mode.size.y });

    if (state == State::Fullscreen)
    {
        fullscreenPending = true;
    }
}


////////////////////////////////////////////////////////////
WindowImplEmscripten::~WindowImplEmscripten()
{
    window = nullptr;
}


////////////////////////////////////////////////////////////
WindowHandle WindowImplEmscripten::getNativeHandle() const
{
    // Not applicable
    return {};
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::processEvents()
{
    // Not applicable
}


////////////////////////////////////////////////////////////
Vector2i WindowImplEmscripten::getPosition() const
{
    // Not applicable
    return {};
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setPosition(Vector2i /* position */)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
Vector2u WindowImplEmscripten::getSize() const
{
    int width{};
    int height{};
    emscripten_get_canvas_element_size("#canvas", &width, &height);

    return {static_cast<unsigned int>(width), static_cast<unsigned int>(height)};
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setSize(Vector2u size)
{
    emscripten_set_canvas_element_size("#canvas", static_cast<int>(size.x), static_cast<int>(size.y));
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setTitle(const String& /* title */)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setIcon(Vector2u /* size */, const std::uint8_t* /* pixels */)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setVisible(bool /* visible */)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setMouseCursorVisible(bool /* visible */)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setMouseCursorGrabbed(bool /* grabbed */)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setMouseCursor(const CursorImpl& /* cursor */)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
bool WindowImplEmscripten::getKeyRepeatEnabled() const
{
    return m_keyRepeatEnabled;
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setKeyRepeatEnabled(bool enabled)
{
    m_keyRepeatEnabled = enabled;
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::requestFocus()
{
    // Not applicable
}


////////////////////////////////////////////////////////////
bool WindowImplEmscripten::hasFocus() const
{
    return windowHasFocus;
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::pushHtmlEvent(const Event& event)
{
    pushEvent(event);
}

} // namespace sf::priv

namespace sf::priv::InputImpl
{
////////////////////////////////////////////////////////////
bool isKeyPressed(Keyboard::Key key)
{
    if (!keyStatusInitialized)
    {
        for (int i = 0; i < static_cast<int>(sf::Keyboard::KeyCount); ++i)
        {
            keyStatus[i] = false;
        }

        keyStatusInitialized = true;

        return false;
    }

    return keyStatus[static_cast<int>(key)];
}

////////////////////////////////////////////////////////////
void setVirtualKeyboardVisible(bool /* visible */)
{
    // Not applicable
}

////////////////////////////////////////////////////////////
bool isMouseButtonPressed(Mouse::Button button)
{
    if (!mouseStatusInitialized)
    {
        for (int i = 0; i < static_cast<int>(sf::Mouse::ButtonCount); ++i)
        {
            mouseStatus[i] = false;
        }

        mouseStatusInitialized = true;

        return false;
    }

    return mouseStatus[static_cast<int>(button)];
}


////////////////////////////////////////////////////////////
Vector2i getMousePosition()
{
    return mousePosition;
}


////////////////////////////////////////////////////////////
Vector2i getMousePosition(const WindowBase& /* relativeTo */)
{
    return getMousePosition();
}


////////////////////////////////////////////////////////////
void setMousePosition(const Vector2i& /* position */)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void setMousePosition(const Vector2i& position, const WindowBase& /* relativeTo */)
{
    setMousePosition(position);
}


////////////////////////////////////////////////////////////
bool isTouchDown(unsigned int finger)
{
    if (touchStatus.find(finger) == touchStatus.end())
        return false;

    return true;
}


////////////////////////////////////////////////////////////
Vector2i getTouchPosition(unsigned int finger)
{
    std::map<unsigned int, Vector2i>::const_iterator iter = touchStatus.find(finger);
    if (iter == touchStatus.end())
        return Vector2i();

    return iter->second;
}


////////////////////////////////////////////////////////////
Vector2i getTouchPosition(unsigned int finger, const WindowBase& /* relativeTo */)
{
    return getTouchPosition(finger);
}

} // namespace sf::priv::InputImpl

namespace sf::priv
{
////////////////////////////////////////////////////////////
void JoystickImpl::initialize()
{
    static bool callbacksSet = false;

    if (callbacksSet)
        return;

    if (emscripten_set_gamepadconnected_callback(0, 1, gamepadCallback) != EMSCRIPTEN_RESULT_SUCCESS)
        sf::err() << "Failed to set gamepadconnected callback" << std::endl;

    if (emscripten_set_gamepaddisconnected_callback(0, 1, gamepadCallback) != EMSCRIPTEN_RESULT_SUCCESS)
        sf::err() << "Failed to set gamepaddisconnected callback" << std::endl;

    callbacksSet = true;
}



////////////////////////////////////////////////////////////
void JoystickImpl::cleanup()
{
}


////////////////////////////////////////////////////////////
bool JoystickImpl::isConnected(unsigned int index)
{
    return joysticksConnected[index];
}


////////////////////////////////////////////////////////////
bool JoystickImpl::open(unsigned int index)
{
    if (!isConnected(index))
        return false;

    int numJoysticks = emscripten_get_num_gamepads();

    if (numJoysticks == EMSCRIPTEN_RESULT_NOT_SUPPORTED)
        return false;

    if (index >= static_cast<unsigned int>(numJoysticks))
        return false;

    EmscriptenGamepadEvent gamepadEvent;
    if (emscripten_get_gamepad_status(static_cast<int>(m_index), &gamepadEvent) != EMSCRIPTEN_RESULT_SUCCESS)
    {
        sf::err() << "Failed to get status of gamepad " << index << std::endl;
        joysticksConnected[index] = false;
        return false;
    }

    if (!gamepadEvent.connected)
    {
        joysticksConnected[index] = false;
        return false;
    }

    m_index = static_cast<int>(index);

    m_identification.name = String::fromUtf8(gamepadEvent.id, gamepadEvent.id + 64);
    m_identification.vendorId = 0;
    m_identification.productId = 0;

    return true;
}


////////////////////////////////////////////////////////////
void JoystickImpl::close()
{
    m_index = 0;
}


////////////////////////////////////////////////////////////
JoystickCaps JoystickImpl::getCapabilities() const
{
    JoystickCaps caps;

    EmscriptenGamepadEvent gamepadEvent;
    if (emscripten_get_gamepad_status(static_cast<int>(m_index), &gamepadEvent) != EMSCRIPTEN_RESULT_SUCCESS)
    {
        sf::err() << "Failed to get status of gamepad " << m_index << std::endl;
        joysticksConnected[m_index] = false;
        return caps;
    }

    // Get the number of buttons
    caps.buttonCount = static_cast<unsigned int>(gamepadEvent.numButtons);

    if (caps.buttonCount > Joystick::ButtonCount)
        caps.buttonCount = Joystick::ButtonCount;

    // Only support the "standard" mapping for now
    if (std::strcmp(gamepadEvent.mapping, "standard") == 0)
    {
        caps.axes[Joystick::Axis::X]    = true;
        caps.axes[Joystick::Axis::Y]    = true;
        caps.axes[Joystick::Axis::Z]    = false;
        caps.axes[Joystick::Axis::R]    = true;
        caps.axes[Joystick::Axis::U]    = true;
        caps.axes[Joystick::Axis::V]    = false;
        caps.axes[Joystick::Axis::PovX] = false;
        caps.axes[Joystick::Axis::PovY] = false;
    }
    else
    {
        caps.axes[Joystick::Axis::X]    = false;
        caps.axes[Joystick::Axis::Y]    = false;
        caps.axes[Joystick::Axis::Z]    = false;
        caps.axes[Joystick::Axis::R]    = false;
        caps.axes[Joystick::Axis::U]    = false;
        caps.axes[Joystick::Axis::V]    = false;
        caps.axes[Joystick::Axis::PovX] = false;
        caps.axes[Joystick::Axis::PovY] = false;
    }

    return caps;
}


////////////////////////////////////////////////////////////
Joystick::Identification JoystickImpl::getIdentification() const
{
    return m_identification;
}


////////////////////////////////////////////////////////////
JoystickState JoystickImpl::update()
{
    JoystickState state;

    EmscriptenGamepadEvent gamepadEvent;
    if (emscripten_get_gamepad_status(static_cast<int>(m_index), &gamepadEvent) != EMSCRIPTEN_RESULT_SUCCESS)
    {
        sf::err() << "Failed to get status of gamepad " << m_index << std::endl;
        joysticksConnected[m_index] = false;
        return state;
    }

    for (int i = 0; (i < gamepadEvent.numButtons) && (i < static_cast<int>(Joystick::ButtonCount)); ++i)
    {
        state.buttons[static_cast<unsigned int>(i)] = static_cast<bool>(gamepadEvent.digitalButton[i]);
    }

    if (std::strcmp(gamepadEvent.mapping, "standard") == 0)
    {
        state.axes[Joystick::Axis::X] = static_cast<float>(gamepadEvent.axis[0] * 100.0);
        state.axes[Joystick::Axis::Y] = static_cast<float>(gamepadEvent.axis[1] * 100.0);
        state.axes[Joystick::Axis::R] = static_cast<float>(gamepadEvent.axis[2] * 100.0);
        state.axes[Joystick::Axis::U] = static_cast<float>(gamepadEvent.axis[3] * 100.0);
    }

    return state;
}

} // namespace sf::priv

namespace sf::priv
{
////////////////////////////////////////////////////////////
std::vector<VideoMode> VideoModeImpl::getFullscreenModes()
{
    VideoMode desktop = getDesktopMode();

    std::vector<VideoMode> modes;
    modes.push_back(desktop);
    return modes;
}


////////////////////////////////////////////////////////////
VideoMode VideoModeImpl::getDesktopMode()
{
    int width = emscripten_run_script_int("screen.width");
    int height = emscripten_run_script_int("screen.height");
    return VideoMode({static_cast<unsigned int>(width), static_cast<unsigned int>(height)});
}

} // namespace sf::priv
