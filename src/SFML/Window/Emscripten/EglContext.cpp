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
#include <SFML/Window/Emscripten/EglContext.hpp>
#include <SFML/Window/WindowImpl.hpp>
#include <SFML/System/Err.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Lock.hpp>
#include <SFML/OpenGL.hpp>
#include <cassert>
#include <string>

namespace
{
    sf::Mutex mutex;

    EGLDisplay sharedDisplay = EGL_NO_DISPLAY;
    int displayReferenceCount = 0;

    EGLSurface sharedSurface = EGL_NO_SURFACE;
    int surfaceReferenceCount = 0;

    EGLContext sharedContext = EGL_NO_CONTEXT;
    int contextReferenceCount = 0;

#ifdef SFML_DEBUG

    void eglCheckError(unsigned int line)
    {
        // Obtain information about the success or failure of the most recent EGL
        // function called in the current thread
        EGLint errorCode = eglGetError();

        if (errorCode != EGL_SUCCESS)
        {
            std::string fileString(__FILE__);
            std::string error = "unknown error";
            std::string description  = "no description";

            // Decode the error code returned
            switch (errorCode)
            {
                case EGL_NOT_INITIALIZED:
                {
                    error = "EGL_NOT_INITIALIZED";
                    description = "EGL is not initialized, or could not be initialized, for the specified display";
                    break;
                }

                case EGL_BAD_ACCESS:
                {
                    error = "EGL_BAD_ACCESS";
                    description = "EGL cannot access a requested resource (for example, a context is bound in another thread)";
                    break;
                }

                case EGL_BAD_ALLOC:
                {
                    error = "EGL_BAD_ALLOC";
                    description = "EGL failed to allocate resources for the requested operation";
                    break;
                }
                case EGL_BAD_ATTRIBUTE:
                {
                    error = "EGL_BAD_ATTRIBUTE";
                    description = "an unrecognized attribute or attribute value was passed in an attribute list";
                    break;
                }

                case EGL_BAD_CONTEXT:
                {
                    error = "EGL_BAD_CONTEXT";
                    description = "an EGLContext argument does not name a valid EGLContext";
                    break;
                }

                case EGL_BAD_CONFIG:
                {
                    error = "EGL_BAD_CONFIG";
                    description = "an EGLConfig argument does not name a valid EGLConfig";
                    break;
                }

                case EGL_BAD_CURRENT_SURFACE:
                {
                    error = "EGL_BAD_CURRENT_SURFACE";
                    description = "the current surface of the calling thread is a window, pbuffer, or pixmap that is no longer valid";
                    break;
                }

                case EGL_BAD_DISPLAY:
                {
                    error = "EGL_BAD_DISPLAY";
                    description = "an EGLDisplay argument does not name a valid EGLDisplay; or, EGL is not initialized on the specified EGLDisplay";
                    break;
                }


                case EGL_BAD_SURFACE:
                {
                    error = "EGL_BAD_SURFACE";
                    description = "an EGLSurface argument does not name a valid surface (window, pbuffer, or pixmap) configured for rendering";
                    break;
                }

                case EGL_BAD_MATCH:
                {
                    error = "EGL_BAD_MATCH";
                    description = "arguments are inconsistent; for example, an otherwise valid context requires buffers (e.g. depth or stencil) not allocated by an otherwise valid surface";
                    break;
                }

                case EGL_BAD_PARAMETER:
                {
                    error = "EGL_BAD_PARAMETER";
                    description = "one or more argument values are invalid";
                    break;
                }

                case EGL_BAD_NATIVE_PIXMAP:
                {
                    error = "EGL_BAD_NATIVE_PIXMAP";
                    description = "an EGLNativePixmapType argument does not refer to a valid native pixmap";
                    break;
                }

                case EGL_BAD_NATIVE_WINDOW:
                {
                    error = "EGL_BAD_NATIVE_WINDOW";
                    description = "an EGLNativeWindowType argument does not refer to a valid native window";
                    break;
                }

                case EGL_CONTEXT_LOST:
                {
                    error = "EGL_CONTEXT_LOST";
                    description = "a power management event has occurred. The application must destroy all contexts and reinitialize client API state and objects to continue rendering";
                    break;
                }
            }

            // Log the error
            sf::err() << "An internal EGL call failed in "
                  << fileString.substr(fileString.find_last_of("\\/") + 1) << " (" << line << ") : "
                  << error << ", " << description
                  << std::endl;
        }
    }

    //// In debug mode, perform a test on every EGL call
    #define eglCheck(x) x; eglCheckError(__LINE__);

#else

    // Else, we don't add any overhead
    #define eglCheck(x) (x)

#endif

    EGLDisplay openDisplay()
    {
        sf::Lock lock(mutex);

        assert(displayReferenceCount >= 0);

        if (displayReferenceCount == 0)
        {
            sharedDisplay = eglCheck(eglGetDisplay(EGL_DEFAULT_DISPLAY));
            eglCheck(eglInitialize(sharedDisplay, NULL, NULL));
        }

        displayReferenceCount++;
        return sharedDisplay;
    }

    void closeDisplay(EGLDisplay display)
    {
        sf::Lock lock(mutex);

        assert(display == sharedDisplay);

        displayReferenceCount--;

        assert(displayReferenceCount >= 0);

        if (displayReferenceCount == 0)
        {
            eglCheck(eglTerminate(sharedDisplay));

            sharedDisplay = EGL_NO_DISPLAY;
        }
    }

    EGLSurface openSurface(EGLDisplay display, EGLConfig config, EGLNativeWindowType window)
    {
        sf::Lock lock(mutex);

        assert(surfaceReferenceCount >= 0);

        if (surfaceReferenceCount == 0)
        {
            sharedSurface = eglCheck(eglCreateWindowSurface(display, config, window, NULL));
            if (sharedSurface == EGL_NO_SURFACE)
                sf::err() << "Failed to create EGL surface" << std::endl;
        }

        surfaceReferenceCount++;
        return sharedSurface;
    }

    void closeSurface(EGLDisplay display, EGLSurface surface)
    {
        sf::Lock lock(mutex);

        assert(surface == sharedSurface);

        surfaceReferenceCount--;

        assert(surfaceReferenceCount >= 0);

        if (surfaceReferenceCount == 0)
        {
            eglCheck(eglDestroySurface(display, sharedSurface));

            sharedSurface = EGL_NO_SURFACE;
        }
    }

    EGLContext openContext(EGLDisplay display, EGLConfig config, EGLContext share, const EGLint* attribs)
    {
        sf::Lock lock(mutex);

        assert(contextReferenceCount >= 0);

        if (contextReferenceCount == 0)
        {
            sharedContext = eglCheck(eglCreateContext(display, config, share, attribs));
            if (sharedContext == EGL_NO_CONTEXT)
                sf::err() << "Failed to create EGL context" << std::endl;
        }

        contextReferenceCount++;
        return sharedContext;
    }

    void closeContext(EGLDisplay display, EGLContext context)
    {
        sf::Lock lock(mutex);

        assert(context == sharedContext);

        contextReferenceCount--;

        assert(contextReferenceCount >= 0);

        if (contextReferenceCount == 0)
        {
            eglCheck(eglDestroyContext(display, sharedContext));

            sharedContext = EGL_NO_SURFACE;
        }
    }

    EGLConfig getBestConfig(EGLDisplay display, unsigned int bitsPerPixel, const sf::ContextSettings& settings)
    {
        // Set our video settings constraint
        const EGLint attributes[] = {
            EGL_BUFFER_SIZE,    bitsPerPixel,
            EGL_RED_SIZE,       8,
            EGL_BLUE_SIZE,      8,
            EGL_GREEN_SIZE,     8,
            EGL_DEPTH_SIZE,     settings.depthBits,
            EGL_STENCIL_SIZE,   settings.stencilBits,
            EGL_SAMPLE_BUFFERS, settings.antialiasingLevel,
            EGL_SURFACE_TYPE,   EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
            EGL_NONE,           EGL_NONE
        };

        EGLint configCount;
        EGLConfig config;

        // Ask EGL for the best config matching our video settings
        eglCheck(eglChooseConfig(display, attributes, &config, 1, &configCount));

        if (configCount == 0)
            sf::err() << "Failed to get any EGL frame buffer configurations" << std::endl;

        return config;
    }
}


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
EglContext::EglContext(EglContext* shared) :
m_display (EGL_NO_DISPLAY),
m_context (EGL_NO_CONTEXT),
m_surface (EGL_NO_SURFACE),
m_config  (NULL)
{
    // Get the shared EGL display
    m_display = openDisplay();

    // Get the best EGL config matching the default video settings
    m_config = getBestConfig(m_display, VideoMode::getDesktopMode().bitsPerPixel, ContextSettings());

    EGLNativeWindowType dummyWindow;

    // Create EGL surface
    m_surface = openSurface(m_display, m_config, dummyWindow);

    // Create EGL context
    createContext(shared);
}


////////////////////////////////////////////////////////////
EglContext::EglContext(EglContext* shared, const ContextSettings& settings, const WindowImpl* owner, unsigned int bitsPerPixel) :
m_display (EGL_NO_DISPLAY),
m_context (EGL_NO_CONTEXT),
m_surface (EGL_NO_SURFACE),
m_config  (NULL)
{
    // Get the shared EGL display
    m_display = openDisplay();

    // Get the best EGL config matching the requested video settings
    m_config = getBestConfig(m_display, bitsPerPixel, settings);

    // Create EGL surface
    m_surface = openSurface(m_display, m_config, owner->getSystemHandle());

    // Create EGL context
    createContext(shared);
}


////////////////////////////////////////////////////////////
EglContext::EglContext(EglContext* shared, const ContextSettings& settings, unsigned int width, unsigned int height) :
m_display (EGL_NO_DISPLAY),
m_context (EGL_NO_CONTEXT),
m_surface (EGL_NO_SURFACE),
m_config  (NULL)
{
    // Get the shared EGL display
    m_display = openDisplay();

    // Get the best EGL config matching the requested video settings
    m_config = getBestConfig(m_display, VideoMode::getDesktopMode().bitsPerPixel, settings);

    EGLNativeWindowType dummyWindow;

    // Create EGL surface
    m_surface = openSurface(m_display, m_config, dummyWindow);

    // Create EGL context
    createContext(shared);
}


////////////////////////////////////////////////////////////
EglContext::~EglContext()
{
    // Destroy context
    closeContext(m_display, m_context);

    // Destroy surface
    closeSurface(m_display, m_surface);

    closeDisplay(m_display);
}


////////////////////////////////////////////////////////////
bool EglContext::makeCurrent()
{
    return m_surface != EGL_NO_SURFACE && eglCheck(eglMakeCurrent(m_display, m_surface, m_surface, m_context));
}


////////////////////////////////////////////////////////////
void EglContext::display()
{
    if (m_surface != EGL_NO_SURFACE)
        eglCheck(eglSwapBuffers(m_display, m_surface));
}


////////////////////////////////////////////////////////////
void EglContext::setVerticalSyncEnabled(bool enabled)
{
    eglCheck(eglSwapInterval(m_display, enabled ? 1 : 0));
}


////////////////////////////////////////////////////////////
void EglContext::createContext(EglContext* shared)
{
    const EGLint contextVersion[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE,                   EGL_NONE
    };

    EGLContext toShared;

    if (shared)
        toShared = shared->m_context;
    else
        toShared = EGL_NO_CONTEXT;

    // Create EGL context
    m_context = openContext(m_display, m_config, toShared, contextVersion);
}

} // namespace priv

} // namespace sf
