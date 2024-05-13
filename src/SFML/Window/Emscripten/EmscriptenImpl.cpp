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
#include <SFML/Window/Emscripten/SensorImpl.hpp>
#include <SFML/Window/Emscripten/InputImpl.hpp>
#include <SFML/Window/VideoModeImpl.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Lock.hpp>
#include <SFML/System/Err.hpp>
#include <emscripten/html5.h>
#include <emscripten.h>
#include <vector>
#include <cmath>
#include <map>


namespace
{
    sf::priv::WindowImplEmscripten* window = NULL;
    bool windowHasFocus = false;
    bool joysticksConnected[sf::Joystick::Count];
    bool keyStatus[sf::Keyboard::KeyCount];
    bool keyStatusInitialized = false;
    bool mouseStatus[sf::Mouse::ButtonCount];
    bool mouseStatusInitialized = false;
    sf::Vector2i mousePosition;
    bool mousePositionInitialized = false;
    std::map<unsigned int, sf::Vector2i> touchStatus;
    bool fullscreenPending = false;

    sf::Keyboard::Key keyCodeToSF(unsigned long key, unsigned long location)
    {
        switch (key)
        {
            case '\b': return sf::Keyboard::BackSpace;
            case '\t': return sf::Keyboard::Tab;

            case '\r':
            {
                if (location == DOM_KEY_LOCATION_STANDARD)
                    return sf::Keyboard::Return;
                else if (location == DOM_KEY_LOCATION_NUMPAD)
                    return sf::Keyboard::Return;
                break;
            }

            case 16:
            {
                if (location == DOM_KEY_LOCATION_LEFT)
                    return sf::Keyboard::LShift;
                else if (location == DOM_KEY_LOCATION_RIGHT)
                    return sf::Keyboard::RShift;
                break;
            }

            case 17:
            {
                if (location == DOM_KEY_LOCATION_LEFT)
                    return sf::Keyboard::LControl;
                else if (location == DOM_KEY_LOCATION_RIGHT)
                    return sf::Keyboard::RControl;
                break;
            }

            case 18:
            {
                if (location == DOM_KEY_LOCATION_LEFT)
                    return sf::Keyboard::LAlt;
                else if (location == DOM_KEY_LOCATION_RIGHT)
                    return sf::Keyboard::RAlt;
                break;
            }

            case 19:  return sf::Keyboard::Pause;

            // case 20: Caps Lock

            case 27:  return sf::Keyboard::Escape;

            case ' ': return sf::Keyboard::Space;
            case 33:  return sf::Keyboard::PageUp;
            case 34:  return sf::Keyboard::PageDown;
            case 35:  return sf::Keyboard::End;
            case 36:  return sf::Keyboard::Home;
            case 37:  return sf::Keyboard::Left;
            case 39:  return sf::Keyboard::Right;
            case 38:  return sf::Keyboard::Up;
            case 40:  return sf::Keyboard::Down;

            // case 42: Print Screen

            case 45:  return sf::Keyboard::Insert;
            case 46:  return sf::Keyboard::Delete;

            case ';': return sf::Keyboard::SemiColon;

            case '=': return sf::Keyboard::Equal;

            case 'A': return sf::Keyboard::A;
            case 'Z': return sf::Keyboard::Z;
            case 'E': return sf::Keyboard::E;
            case 'R': return sf::Keyboard::R;
            case 'T': return sf::Keyboard::T;
            case 'Y': return sf::Keyboard::Y;
            case 'U': return sf::Keyboard::U;
            case 'I': return sf::Keyboard::I;
            case 'O': return sf::Keyboard::O;
            case 'P': return sf::Keyboard::P;
            case 'Q': return sf::Keyboard::Q;
            case 'S': return sf::Keyboard::S;
            case 'D': return sf::Keyboard::D;
            case 'F': return sf::Keyboard::F;
            case 'G': return sf::Keyboard::G;
            case 'H': return sf::Keyboard::H;
            case 'J': return sf::Keyboard::J;
            case 'K': return sf::Keyboard::K;
            case 'L': return sf::Keyboard::L;
            case 'M': return sf::Keyboard::M;
            case 'W': return sf::Keyboard::W;
            case 'X': return sf::Keyboard::X;
            case 'C': return sf::Keyboard::C;
            case 'V': return sf::Keyboard::V;
            case 'B': return sf::Keyboard::B;
            case 'N': return sf::Keyboard::N;
            case '0': return sf::Keyboard::Num0;
            case '1': return sf::Keyboard::Num1;
            case '2': return sf::Keyboard::Num2;
            case '3': return sf::Keyboard::Num3;
            case '4': return sf::Keyboard::Num4;
            case '5': return sf::Keyboard::Num5;
            case '6': return sf::Keyboard::Num6;
            case '7': return sf::Keyboard::Num7;
            case '8': return sf::Keyboard::Num8;
            case '9': return sf::Keyboard::Num9;

            case 91:
            {
                if (location == DOM_KEY_LOCATION_LEFT)
                    return sf::Keyboard::LSystem;
                else if (location == DOM_KEY_LOCATION_RIGHT)
                    return sf::Keyboard::RSystem;
                break;
            }

            case 93:  return sf::Keyboard::Menu;

            case 96:  return sf::Keyboard::Numpad0;
            case 97:  return sf::Keyboard::Numpad1;
            case 98:  return sf::Keyboard::Numpad2;
            case 99:  return sf::Keyboard::Numpad3;
            case 100: return sf::Keyboard::Numpad4;
            case 101: return sf::Keyboard::Numpad5;
            case 102: return sf::Keyboard::Numpad6;
            case 103: return sf::Keyboard::Numpad7;
            case 104: return sf::Keyboard::Numpad8;
            case 105: return sf::Keyboard::Numpad9;

            case 106: return sf::Keyboard::Multiply;
            case 107: return sf::Keyboard::Add;
            case 109: return sf::Keyboard::Subtract;
            case 111: return sf::Keyboard::Divide;

            case 112: return sf::Keyboard::F1;
            case 113: return sf::Keyboard::F2;
            case 114: return sf::Keyboard::F3;
            case 115: return sf::Keyboard::F4;
            case 116: return sf::Keyboard::F5;
            case 117: return sf::Keyboard::F6;
            case 118: return sf::Keyboard::F7;
            case 119: return sf::Keyboard::F8;
            case 120: return sf::Keyboard::F9;
            case 121: return sf::Keyboard::F10;
            case 122: return sf::Keyboard::F11;
            case 123: return sf::Keyboard::F12;
            case 124: return sf::Keyboard::F13;
            case 125: return sf::Keyboard::F14;
            case 126: return sf::Keyboard::F15;

            // case 144: Num Lock
            // case 145: Scroll Lock

            case 173: return sf::Keyboard::Dash;

            case 188: return sf::Keyboard::Comma;

            case 190: return sf::Keyboard::Period;
            case 191: return sf::Keyboard::Slash;
            case 192: return sf::Keyboard::Tilde;

            case 219: return sf::Keyboard::LBracket;
            case 220: return sf::Keyboard::BackSlash;
            case 221: return sf::Keyboard::RBracket;
            case 222: return sf::Keyboard::Quote;
        }

        return sf::Keyboard::Unknown;
    }

    void updatePluggedList()
    {
        int numJoysticks = emscripten_get_num_gamepads();

        if (numJoysticks == EMSCRIPTEN_RESULT_NOT_SUPPORTED)
        {
            for (int i = 0; i < sf::Joystick::Count; ++i)
            {
                joysticksConnected[i] = false;
            }

            return;
        }

        for (int i = 0; (i < sf::Joystick::Count) && (i < numJoysticks); ++i)
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

    EM_BOOL canvasSizeChangedCallback(int eventType, const void* reserved, void* userData)
    {
        if (!window)
            return 0;

        int width, height, fullscreen;
        emscripten_get_canvas_size(&width, &height, &fullscreen);

        sf::Event event;
        event.type        = sf::Event::Resized;
        event.size.width  = width;
        event.size.height = height;
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

        emscripten_request_fullscreen_strategy(0, 0, &fullscreenStrategy);
    }

    EM_BOOL keyCallback(int eventType, const EmscriptenKeyboardEvent* e, void* userData)
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

                keyStatus[key] = true;

                sf::Event event;
                event.type        = sf::Event::KeyPressed;
                event.key.alt     = e->altKey   != 0;
                event.key.control = e->ctrlKey  != 0;
                event.key.shift   = e->shiftKey != 0;
                event.key.system  = e->metaKey  != 0;
                event.key.code    = key;
                window->pushHtmlEvent(event);

                // We try to prevent some keystrokes from bubbling
                // If we try to prevent bubbling for all keys,
                // it prevents keypress events from being generated
                if ((key == sf::Keyboard::Tab) ||
                    (key == sf::Keyboard::BackSpace) ||
                    (key == sf::Keyboard::Menu) ||
                    (key == sf::Keyboard::LSystem) ||
                    (key == sf::Keyboard::RSystem))
                    return 1;

                return 0;
            }
            case EMSCRIPTEN_EVENT_KEYUP:
            {
                keyStatus[key] = false;

                sf::Event event;
                event.type        = sf::Event::KeyReleased;
                event.key.alt     = e->altKey   != 0;
                event.key.control = e->ctrlKey  != 0;
                event.key.shift   = e->shiftKey != 0;
                event.key.system  = e->metaKey  != 0;
                event.key.code    = key;
                window->pushHtmlEvent(event);
                return 1;
            }
            case EMSCRIPTEN_EVENT_KEYPRESS:
            {
                if (e->charCode == 0)
                    return 1;

                sf::Event event;
                event.type         = sf::Event::TextEntered;
                event.text.unicode = e->charCode;
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

    EM_BOOL mouseCallback(int eventType, const EmscriptenMouseEvent* e, void* userData)
    {
        mousePosition.x = e->clientX;
        mousePosition.y = e->clientY;

        if (!window)
            return 0;

        switch (eventType)
        {
            case EMSCRIPTEN_EVENT_MOUSEDOWN:
            {
                sf::Event event;
                event.type               = sf::Event::MouseButtonPressed;
                event.mouseButton.x      = e->clientX;
                event.mouseButton.y      = e->clientY;

                if (fullscreenPending)
                {
                    requestFullscreen();
                    fullscreenPending = false;
                }

                if (e->button == 0)
                {
                    event.mouseButton.button = sf::Mouse::Left;
                    mouseStatus[sf::Mouse::Left] = true;
                }
                else if (e->button == 1)
                {
                    event.mouseButton.button = sf::Mouse::Middle;
                    mouseStatus[sf::Mouse::Middle] = true;
                }
                else if (e->button == 2)
                {
                    event.mouseButton.button = sf::Mouse::Right;
                    mouseStatus[sf::Mouse::Right] = true;
                }
                else
                {
                    event.mouseButton.button = sf::Mouse::ButtonCount;
                }

                window->pushHtmlEvent(event);
                return 1;
            }
            case EMSCRIPTEN_EVENT_MOUSEUP:
            {
                sf::Event event;
                event.type          = sf::Event::MouseButtonReleased;
                event.mouseButton.x = e->clientX;
                event.mouseButton.y = e->clientY;

                if (e->button == 0)
                {
                    event.mouseButton.button = sf::Mouse::Left;
                    mouseStatus[sf::Mouse::Left] = false;
                }
                else if (e->button == 1)
                {
                    event.mouseButton.button = sf::Mouse::Middle;
                    mouseStatus[sf::Mouse::Middle] = false;
                }
                else if (e->button == 2)
                {
                    event.mouseButton.button = sf::Mouse::Right;
                    mouseStatus[sf::Mouse::Right] = false;
                }
                else
                {
                    event.mouseButton.button = sf::Mouse::ButtonCount;
                }

                window->pushHtmlEvent(event);
                return 1;
            }
            case EMSCRIPTEN_EVENT_MOUSEMOVE:
            {
                sf::Event event;
                event.type        = sf::Event::MouseMoved;
                event.mouseMove.x = e->clientX;
                event.mouseMove.y = e->clientY;
                window->pushHtmlEvent(event);
                return 1;
            }
            case EMSCRIPTEN_EVENT_MOUSEENTER:
            {
                sf::Event event;
                event.type = sf::Event::MouseEntered;
                window->pushHtmlEvent(event);
                return 1;
            }
            case EMSCRIPTEN_EVENT_MOUSELEAVE:
            {
                sf::Event event;
                event.type = sf::Event::MouseLeft;
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

    EM_BOOL wheelCallback(int eventType, const EmscriptenWheelEvent* e, void* userData)
    {
        if (!window)
            return 0;

        switch (eventType)
        {
            case EMSCRIPTEN_EVENT_WHEEL:
            {
                if (std::fabs(e->deltaY) > 0.0)
                {
                    sf::Event event;

                    event.type             = sf::Event::MouseWheelMoved;
                    event.mouseWheel.delta = -static_cast<int>(e->deltaY);
                    event.mouseWheel.x     = e->mouse.clientX;
                    event.mouseWheel.y     = e->mouse.clientY;
                    window->pushHtmlEvent(event);

                    event.type                   = sf::Event::MouseWheelScrolled;
                    event.mouseWheelScroll.wheel = sf::Mouse::VerticalWheel;
                    event.mouseWheelScroll.delta = -static_cast<float>(e->deltaY);
                    event.mouseWheelScroll.x     = e->mouse.clientX;
                    event.mouseWheelScroll.y     = e->mouse.clientY;
                    window->pushHtmlEvent(event);
                }

                if (std::fabs(e->deltaX) > 0.0)
                {
                    sf::Event event;

                    event.type                   = sf::Event::MouseWheelScrolled;
                    event.mouseWheelScroll.wheel = sf::Mouse::HorizontalWheel;
                    event.mouseWheelScroll.delta = static_cast<float>(e->deltaX);
                    event.mouseWheelScroll.x     = e->mouse.clientX;
                    event.mouseWheelScroll.y     = e->mouse.clientY;
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

    EM_BOOL uieventCallback(int eventType, const EmscriptenUiEvent* e, void* userData)
    {
        if (!window)
            return 0;

        switch (eventType)
        {
            case EMSCRIPTEN_EVENT_RESIZE:
            {
                int width, height, fullscreen;
                emscripten_get_canvas_size(&width, &height, &fullscreen);

                sf::Event event;
                event.type        = sf::Event::Resized;
                event.size.width  = width;
                event.size.height = height;
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

    EM_BOOL focuseventCallback(int eventType, const EmscriptenFocusEvent* e, void* userData)
    {
        if (!window)
            return 0;

        switch (eventType)
        {
            case EMSCRIPTEN_EVENT_FOCUS:
            {
                sf::Event event;
                event.type = sf::Event::GainedFocus;
                window->pushHtmlEvent(event);

                windowHasFocus = true;

                return 1;
            }
            case EMSCRIPTEN_EVENT_BLUR:
            {
                sf::Event event;
                event.type = sf::Event::LostFocus;
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

    EM_BOOL deviceorientationCallback(int eventType, const EmscriptenDeviceOrientationEvent* e, void* userData)
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

    EM_BOOL devicemotionCallback(int eventType, const EmscriptenDeviceMotionEvent* e, void* userData)
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

    EM_BOOL orientationchangeCallback(int eventType, const EmscriptenOrientationChangeEvent* e, void* userData)
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

    EM_BOOL fullscreenchangeCallback(int eventType, const EmscriptenFullscreenChangeEvent* e, void* userData)
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

    EM_BOOL pointerlockchangeCallback(int eventType, const EmscriptenPointerlockChangeEvent* e, void* userData)
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

    EM_BOOL visibilitychangeCallback(int eventType, const EmscriptenVisibilityChangeEvent* e, void* userData)
    {
        if (!window)
            return 0;

        switch (eventType)
        {
            case EMSCRIPTEN_VISIBILITY_UNLOADED:
            {
                sf::Event event;
                event.type = sf::Event::Closed;
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

    EM_BOOL touchCallback(int eventType, const EmscriptenTouchEvent* e, void* userData)
    {
        if (!window)
            return 0;

        switch (eventType)
        {
            case EMSCRIPTEN_EVENT_TOUCHSTART:
            {
                sf::Event event;
                event.type = sf::Event::TouchBegan;

                for (int i = 0; i < e->numTouches; ++i)
                {
                    event.touch.finger = e->touches[i].identifier;
                    event.touch.x      = e->touches[i].clientX;
                    event.touch.y      = e->touches[i].clientY;
                    window->pushHtmlEvent(event);

                    touchStatus.insert(std::make_pair(static_cast<unsigned int>(e->touches[i].identifier), sf::Vector2i(e->touches[i].clientX, e->touches[i].clientY)));
                }

                return 1;
            }
            case EMSCRIPTEN_EVENT_TOUCHEND:
            {
                sf::Event event;
                event.type = sf::Event::TouchEnded;

                for (int i = 0; i < e->numTouches; ++i)
                {
                    event.touch.finger = e->touches[i].identifier;
                    event.touch.x      = e->touches[i].clientX;
                    event.touch.y      = e->touches[i].clientY;
                    window->pushHtmlEvent(event);

                    touchStatus.erase(static_cast<unsigned int>(e->touches[i].identifier));
                }

                return 1;
            }
            case EMSCRIPTEN_EVENT_TOUCHMOVE:
            {
                sf::Event event;
                event.type = sf::Event::TouchMoved;

                for (int i = 0; i < e->numTouches; ++i)
                {
                    event.touch.finger = e->touches[i].identifier;
                    event.touch.x      = e->touches[i].clientX;
                    event.touch.y      = e->touches[i].clientY;
                    window->pushHtmlEvent(event);

                    touchStatus[static_cast<unsigned int>(e->touches[i].identifier)] = sf::Vector2i(e->touches[i].clientX, e->touches[i].clientY);
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

    EM_BOOL gamepadCallback(int eventType, const EmscriptenGamepadEvent* e, void* userData)
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
                    for (int i = 0; i < sf::Joystick::Count; ++i)
                    {
                        if (!previousConnected[i] && joysticksConnected[i])
                        {
                            sf::Event event;
                            event.type = sf::Event::JoystickConnected;
                            event.joystickConnect.joystickId = i;
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
                    for (int i = 0; i < sf::Joystick::Count; ++i)
                    {
                        if (previousConnected[i] && !joysticksConnected[i])
                        {
                            sf::Event event;
                            event.type = sf::Event::JoystickDisconnected;
                            event.joystickConnect.joystickId = i;
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

        if (emscripten_set_keypress_callback(0, 0, 1, keyCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set keypress callback" << std::endl;

        if (emscripten_set_keydown_callback(0, 0, 1, keyCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set keydown callback" << std::endl;

        if (emscripten_set_keyup_callback(0, 0, 1, keyCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set keyup callback" << std::endl;

        if (emscripten_set_click_callback(0, 0, 1, mouseCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set click callback" << std::endl;

        if (emscripten_set_mousedown_callback(0, 0, 1, mouseCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set mousedown callback" << std::endl;

        if (emscripten_set_mouseup_callback(0, 0, 1, mouseCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set mouseup callback" << std::endl;

        if (emscripten_set_dblclick_callback(0, 0, 1, mouseCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set dblclick callback" << std::endl;

        if (emscripten_set_mousemove_callback(0, 0, 1, mouseCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set mousemove callback" << std::endl;

        if (emscripten_set_mouseenter_callback(0, 0, 1, mouseCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set mouseenter callback" << std::endl;

        if (emscripten_set_mouseleave_callback(0, 0, 1, mouseCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set mouseleave callback" << std::endl;

        if (emscripten_set_mouseover_callback(0, 0, 1, mouseCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set mouseover callback" << std::endl;

        if (emscripten_set_mouseout_callback(0, 0, 1, mouseCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set mouseout callback" << std::endl;

        if (emscripten_set_wheel_callback(0, 0, 1, wheelCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set wheel callback" << std::endl;

        if (emscripten_set_resize_callback(0, 0, 1, uieventCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set resize callback" << std::endl;

        if (emscripten_set_scroll_callback(0, 0, 1, uieventCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set scroll callback" << std::endl;

        if (emscripten_set_blur_callback(0, 0, 1, focuseventCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set blur callback" << std::endl;

        if (emscripten_set_focus_callback(0, 0, 1, focuseventCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set focus callback" << std::endl;

        if (emscripten_set_focusin_callback(0, 0, 1, focuseventCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set focusin callback" << std::endl;

        if (emscripten_set_focusout_callback(0, 0, 1, focuseventCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set focusout callback" << std::endl;

        if (emscripten_set_deviceorientation_callback(0, 1, deviceorientationCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set deviceorientation callback" << std::endl;

        if (emscripten_set_devicemotion_callback(0, 1, devicemotionCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set devicemotion callback" << std::endl;

        if (emscripten_set_orientationchange_callback(0, 1, orientationchangeCallback) != EMSCRIPTEN_RESULT_SUCCESS)
            sf::err() << "Failed to set orientationchange callback" << std::endl;

        if (!keyStatusInitialized)
        {
            for (int i = 0; i < sf::Keyboard::KeyCount; ++i)
            {
                keyStatus[i] = false;
            }

            keyStatusInitialized = true;
        }

        if (!mouseStatusInitialized)
        {
            for (int i = 0; i < sf::Mouse::ButtonCount; ++i)
            {
                mouseStatus[i] = false;
            }

            mouseStatusInitialized = true;
        }

        callbacksSet = true;
    }
}


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
WindowImplEmscripten::WindowImplEmscripten(WindowHandle handle) :
m_keyRepeatEnabled(true)
{
    err() << "Creating a window from a WindowHandle unsupported" << std::endl;
    std::abort();
}


////////////////////////////////////////////////////////////
WindowImplEmscripten::WindowImplEmscripten(VideoMode mode, const String& title, unsigned long style, const ContextSettings& settings) :
m_keyRepeatEnabled(true)
{
    if (window)
    {
        err() << "Creating multiple windows is unsupported" << std::endl;
        std::abort();
    }

    setCallbacks();

    window = this;

    setSize(Vector2u(mode.width, mode.height));

    if (style & Style::Fullscreen)
    {
        fullscreenPending = true;
    }
}


////////////////////////////////////////////////////////////
WindowImplEmscripten::~WindowImplEmscripten()
{
    window = NULL;
}


////////////////////////////////////////////////////////////
WindowHandle WindowImplEmscripten::getSystemHandle() const
{
    // Not applicable
    return 0;
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
    return Vector2i();
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setPosition(const Vector2i& position)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
Vector2u WindowImplEmscripten::getSize() const
{
    int width, height, fullscreen;
    emscripten_get_canvas_size(&width, &height, &fullscreen);

    return Vector2u(width, height);
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setSize(const Vector2u& size)
{
    emscripten_set_canvas_size(size.x, size.y);
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setTitle(const String& title)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setIcon(unsigned int width, unsigned int height, const Uint8* pixels)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setVisible(bool visible)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setMouseCursorVisible(bool visible)
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


////////////////////////////////////////////////////////////
bool InputImpl::isKeyPressed(Keyboard::Key key)
{
    if (!keyStatusInitialized)
    {
        for (int i = 0; i < sf::Keyboard::KeyCount; ++i)
        {
            keyStatus[i] = false;
        }

        keyStatusInitialized = true;

        return false;
    }

    return keyStatus[key];
}

////////////////////////////////////////////////////////////
void InputImpl::setVirtualKeyboardVisible(bool visible)
{
    // Not applicable
}

////////////////////////////////////////////////////////////
bool InputImpl::isMouseButtonPressed(Mouse::Button button)
{
    if (!mouseStatusInitialized)
    {
        for (int i = 0; i < sf::Mouse::ButtonCount; ++i)
        {
            mouseStatus[i] = false;
        }

        mouseStatusInitialized = true;

        return false;
    }

    return mouseStatus[button];
}


////////////////////////////////////////////////////////////
Vector2i InputImpl::getMousePosition()
{
    return mousePosition;
}


////////////////////////////////////////////////////////////
Vector2i InputImpl::getMousePosition(const Window& relativeTo)
{
    return getMousePosition();
}


////////////////////////////////////////////////////////////
void InputImpl::setMousePosition(const Vector2i& position)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void InputImpl::setMousePosition(const Vector2i& position, const Window& relativeTo)
{
    setMousePosition(position);
}


////////////////////////////////////////////////////////////
bool InputImpl::isTouchDown(unsigned int finger)
{
    if (touchStatus.find(finger) == touchStatus.end())
        return false;

    return true;
}


////////////////////////////////////////////////////////////
Vector2i InputImpl::getTouchPosition(unsigned int finger)
{
    std::map<unsigned int, Vector2i>::const_iterator iter = touchStatus.find(finger);
    if (iter == touchStatus.end())
        return Vector2i();

    return iter->second;
}


////////////////////////////////////////////////////////////
Vector2i InputImpl::getTouchPosition(unsigned int finger, const Window& relativeTo)
{
    return getTouchPosition(finger);
}


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

    if (index >= numJoysticks)
        return false;

    EmscriptenGamepadEvent gamepadEvent;
    if (emscripten_get_gamepad_status(index, &gamepadEvent) != EMSCRIPTEN_RESULT_SUCCESS)
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

    m_index = index;

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
    if (emscripten_get_gamepad_status(m_index, &gamepadEvent) != EMSCRIPTEN_RESULT_SUCCESS)
    {
        sf::err() << "Failed to get status of gamepad " << m_index << std::endl;
        joysticksConnected[m_index] = false;
        return caps;
    }

    // Get the number of buttons
    caps.buttonCount = gamepadEvent.numButtons;

    if (caps.buttonCount > Joystick::ButtonCount)
        caps.buttonCount = Joystick::ButtonCount;

    // Only support the "standard" mapping for now
    if (std::strcmp(gamepadEvent.mapping, "standard") == 0)
    {
        caps.axes[Joystick::X]    = true;
        caps.axes[Joystick::Y]    = true;
        caps.axes[Joystick::Z]    = false;
        caps.axes[Joystick::R]    = true;
        caps.axes[Joystick::U]    = true;
        caps.axes[Joystick::V]    = false;
        caps.axes[Joystick::PovX] = false;
        caps.axes[Joystick::PovY] = false;
    }
    else
    {
        caps.axes[Joystick::X]    = false;
        caps.axes[Joystick::Y]    = false;
        caps.axes[Joystick::Z]    = false;
        caps.axes[Joystick::R]    = false;
        caps.axes[Joystick::U]    = false;
        caps.axes[Joystick::V]    = false;
        caps.axes[Joystick::PovX] = false;
        caps.axes[Joystick::PovY] = false;
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
    if (emscripten_get_gamepad_status(m_index, &gamepadEvent) != EMSCRIPTEN_RESULT_SUCCESS)
    {
        sf::err() << "Failed to get status of gamepad " << m_index << std::endl;
        joysticksConnected[m_index] = false;
        return state;
    }

    for (int i = 0; (i < gamepadEvent.numButtons) && (i < Joystick::ButtonCount); ++i)
    {
        state.buttons[i] = gamepadEvent.digitalButton[i];
    }

    if (std::strcmp(gamepadEvent.mapping, "standard") == 0)
    {
        state.axes[Joystick::X] = static_cast<float>(gamepadEvent.axis[0] * 100.0);
        state.axes[Joystick::Y] = static_cast<float>(gamepadEvent.axis[1] * 100.0);
        state.axes[Joystick::R] = static_cast<float>(gamepadEvent.axis[2] * 100.0);
        state.axes[Joystick::U] = static_cast<float>(gamepadEvent.axis[3] * 100.0);
    }

    return state;
}


////////////////////////////////////////////////////////////
void SensorImpl::initialize()
{
    // Not applicable
}


////////////////////////////////////////////////////////////
void SensorImpl::cleanup()
{
    // Not applicable
}


////////////////////////////////////////////////////////////
bool SensorImpl::isAvailable(Sensor::Type sensor)
{
    // Not applicable
    return false;
}


////////////////////////////////////////////////////////////
bool SensorImpl::open(Sensor::Type sensor)
{
    // Not applicable
    return false;
}


////////////////////////////////////////////////////////////
void SensorImpl::close()
{
    // Not applicable
}


////////////////////////////////////////////////////////////
Vector3f SensorImpl::update()
{
    // Not applicable
    return Vector3f(0, 0, 0);
}


////////////////////////////////////////////////////////////
void SensorImpl::setEnabled(bool enabled)
{
    // Not applicable
}


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
    return VideoMode(width, height);
}

} // namespace priv
} // namespace sf
