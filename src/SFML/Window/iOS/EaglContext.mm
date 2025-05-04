////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2025 Laurent Gomila (laurent@sfml-dev.org)
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
#include <SFML/Window/iOS/EaglContext.hpp>
#include <SFML/Window/iOS/SFView.hpp>
#include <SFML/Window/iOS/WindowImplUIKit.hpp>

#include <SFML/System/Err.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>

#include <OpenGLES/EAGL.h>
#include <OpenGLES/EAGLDrawable.h>
#include <QuartzCore/CAEAGLLayer.h>
#include <array>
#include <dlfcn.h>
#include <ostream>


#pragma GCC diagnostic ignored "-Wdeprecated-declarations"


namespace
{
    PFNGLBINDFRAMEBUFFERPROC            glBindFramebufferFunc            = nullptr;
    PFNGLBINDRENDERBUFFERPROC           glBindRenderbufferFunc           = nullptr;
    PFNGLCHECKFRAMEBUFFERSTATUSPROC     glCheckFramebufferStatusFunc     = nullptr;
    PFNGLDELETEFRAMEBUFFERSPROC         glDeleteFramebuffersFunc         = nullptr;
    PFNGLDELETERENDERBUFFERSPROC        glDeleteRenderbuffersFunc        = nullptr;
    PFNGLFRAMEBUFFERRENDERBUFFERPROC    glFramebufferRenderbufferFunc    = nullptr;
    PFNGLGENFRAMEBUFFERSPROC            glGenFramebuffersFunc            = nullptr;
    PFNGLGENRENDERBUFFERSPROC           glGenRenderbuffersFunc           = nullptr;
    PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameterivFunc = nullptr;
    PFNGLRENDERBUFFERSTORAGEPROC        glRenderbufferStorageFunc        = nullptr;


void ensureInit()
{
    static bool initialized = false;
    if (!initialized)
    {
        initialized = true;

        glBindFramebufferFunc = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(
            sf::priv::EaglContext::getFunction("glBindFramebuffer"));
        glBindRenderbufferFunc = reinterpret_cast<PFNGLBINDRENDERBUFFERPROC>(
            sf::priv::EaglContext::getFunction("glBindRenderbuffer"));
        glCheckFramebufferStatusFunc = reinterpret_cast<PFNGLCHECKFRAMEBUFFERSTATUSPROC>(
            sf::priv::EaglContext::getFunction("glCheckFramebufferStatus"));
        glDeleteFramebuffersFunc = reinterpret_cast<PFNGLDELETEFRAMEBUFFERSPROC>(
            sf::priv::EaglContext::getFunction("glDeleteFramebuffers"));
        glDeleteRenderbuffersFunc = reinterpret_cast<PFNGLDELETERENDERBUFFERSPROC>(
            sf::priv::EaglContext::getFunction("glDeleteRenderbuffers"));
        glFramebufferRenderbufferFunc = reinterpret_cast<PFNGLFRAMEBUFFERRENDERBUFFERPROC>(
            sf::priv::EaglContext::getFunction("glFramebufferRenderbuffer"));
        glGenFramebuffersFunc = reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(
            sf::priv::EaglContext::getFunction("glGenFramebuffers"));
        glGenRenderbuffersFunc = reinterpret_cast<PFNGLGENRENDERBUFFERSPROC>(
            sf::priv::EaglContext::getFunction("glGenRenderbuffers"));
        glGetRenderbufferParameterivFunc = reinterpret_cast<PFNGLGETRENDERBUFFERPARAMETERIVPROC>(
            sf::priv::EaglContext::getFunction("glGetRenderbufferParameteriv"));
        glRenderbufferStorageFunc = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEPROC>(
            sf::priv::EaglContext::getFunction("glRenderbufferStorage"));
    }
}
} // namespace


namespace sf::priv
{
////////////////////////////////////////////////////////////
EaglContext::EaglContext(EaglContext* shared) : m_context(nil)
{
    ensureInit();

    // Create the context
    if (shared)
        m_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2
                                          sharegroup:[shared->m_context sharegroup]];
    else
        m_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
}


////////////////////////////////////////////////////////////
EaglContext::EaglContext(EaglContext* shared, const ContextSettings& settings, const WindowImpl& owner, unsigned int bitsPerPixel) :
m_context(nil)
{
    ensureInit();

    const auto& window = static_cast<const WindowImplUIKit&>(owner);

    createContext(shared, window, bitsPerPixel, settings);
}


////////////////////////////////////////////////////////////
EaglContext::EaglContext(EaglContext* /* shared */, const ContextSettings& /* settings */, Vector2u /* size */) :
m_context(nil)
{
    ensureInit();

    // This constructor should never be used by implementation
    err() << "Calling bad EaglContext constructor, please contact your developer :)" << std::endl;
}


////////////////////////////////////////////////////////////
EaglContext::~EaglContext()
{
    // Notify unshared OpenGL resources of context destruction
    cleanupUnsharedResources();

    if (m_context)
    {
        // Activate the context, so that we can destroy the buffers
        EAGLContext* const previousContext = [EAGLContext currentContext];
        [EAGLContext setCurrentContext:m_context];

        // Destroy the buffers
        if (m_framebuffer)
            glDeleteFramebuffersFunc(1, &m_framebuffer);
        if (m_colorbuffer)
            glDeleteRenderbuffersFunc(1, &m_colorbuffer);
        if (m_depthbuffer)
            glDeleteRenderbuffersFunc(1, &m_depthbuffer);

        // Restore the previous context
        [EAGLContext setCurrentContext:previousContext];

        if (m_context == [EAGLContext currentContext])
            [EAGLContext setCurrentContext:nil];
    }
}


////////////////////////////////////////////////////////////
GlFunctionPointer EaglContext::getFunction(const char* name)
{
    static void* module = nullptr;

    static constexpr std::array libs = {"libGLESv2.dylib",
                                        "/System/Library/Frameworks/OpenGLES.framework/OpenGLES",
                                        "OpenGLES.framework/OpenGLES"};

    for (const auto& lib : libs)
    {
        if (!module)
            module = dlopen(lib, RTLD_LAZY | RTLD_LOCAL);
    }

    if (module)
        return reinterpret_cast<GlFunctionPointer>(reinterpret_cast<uintptr_t>(dlsym(module, name)));

    return nullptr;
}


////////////////////////////////////////////////////////////
void EaglContext::recreateRenderBuffers(SFView* glView)
{
    // Activate the context
    EAGLContext* const previousContext = [EAGLContext currentContext];
    [EAGLContext setCurrentContext:m_context];

    // Bind the frame buffer
    glBindFramebufferFunc(GL_FRAMEBUFFER, m_framebuffer);

    // Destroy previous render-buffers
    if (m_colorbuffer)
        glDeleteRenderbuffersFunc(1, &m_colorbuffer);
    if (m_depthbuffer)
        glDeleteRenderbuffersFunc(1, &m_depthbuffer);

    // Create the color buffer
    glGenRenderbuffersFunc(1, &m_colorbuffer);
    glBindRenderbufferFunc(GL_RENDERBUFFER, m_colorbuffer);
    if (glView)
        [m_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(static_cast<CAEAGLLayer*>(glView.layer))];
    glFramebufferRenderbufferFunc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_colorbuffer);

    // Create a depth buffer if requested
    if (m_settings.depthBits > 0)
    {
        // Find the best internal format
        const GLenum format = m_settings.depthBits > 16
                                  ? (m_settings.stencilBits == 0 ? GL_DEPTH_COMPONENT24 : GL_DEPTH24_STENCIL8)
                                  : GL_DEPTH_COMPONENT16;

        // Get the size of the color-buffer (which fits the current size of the GL view)
        GLint width  = 0;
        GLint height = 0;
        glGetRenderbufferParameterivFunc(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
        glGetRenderbufferParameterivFunc(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);

        // Create the depth buffer
        glGenRenderbuffersFunc(1, &m_depthbuffer);
        glBindRenderbufferFunc(GL_RENDERBUFFER, m_depthbuffer);
        glRenderbufferStorageFunc(GL_RENDERBUFFER, format, width, height);
        glFramebufferRenderbufferFunc(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthbuffer);
        if (m_settings.stencilBits > 0)
            glFramebufferRenderbufferFunc(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthbuffer);
    }

    // Make sure that everything's ok
    const GLenum status = glCheckFramebufferStatusFunc(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        err() << "Failed to create a valid frame buffer (error code: " << status << ")" << std::endl;

    // Restore the previous context
    [EAGLContext setCurrentContext:previousContext];
}


////////////////////////////////////////////////////////////
bool EaglContext::makeCurrent(bool current)
{
    if (current)
        return [EAGLContext setCurrentContext:m_context];

    return [EAGLContext setCurrentContext:nil];
}


////////////////////////////////////////////////////////////
void EaglContext::display()
{
    glBindRenderbufferFunc(GL_RENDERBUFFER, m_colorbuffer);
    [m_context presentRenderbuffer:GL_RENDERBUFFER];

    // The proper way of doing v-sync on iOS would be to use CADisplayLink
    // notifications, but it is not compatible with the way SFML is designed;
    // therefore we fake it with a manual framerate limit
    if (m_vsyncEnabled)
    {
        constexpr Time frameDuration = seconds(1.f / 60.f);
        sleep(frameDuration - m_clock.getElapsedTime());
        m_clock.restart();
    }
}


////////////////////////////////////////////////////////////
void EaglContext::setVerticalSyncEnabled(bool enabled)
{
    m_vsyncEnabled = enabled;
}


////////////////////////////////////////////////////////////
void EaglContext::createContext(EaglContext*           shared,
                                const WindowImplUIKit& window,
                                unsigned int /* bitsPerPixel */,
                                const ContextSettings& settings)
{
    // Save the settings
    m_settings = settings;

    // Adjust the depth buffer format to those available
    if (m_settings.depthBits > 16)
        m_settings.depthBits = 24;
    else if (m_settings.depthBits > 0)
        m_settings.depthBits = 16;

    // Create the context
    if (shared)
    {
        [EAGLContext setCurrentContext:nil];

        m_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2
                                          sharegroup:[shared->m_context sharegroup]];
    }
    else
    {
        m_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    }

    // Activate it
    makeCurrent(true);

    // Create the framebuffer (this is the only allowed drawable on iOS)
    glGenFramebuffersFunc(1, &m_framebuffer);

    // Create the render buffers
    recreateRenderBuffers(window.getGlView());

    // Attach the context to the GL view for future updates
    window.getGlView().context = this;

    // Deactivate it
    makeCurrent(false);
}

} // namespace sf::priv
