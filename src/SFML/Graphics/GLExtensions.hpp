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

#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Config.hpp>

#include <glad/gl.h>

#ifdef SFML_OPENGL_ES

// Core since 3.0
#define GLEXT_packed_depth_stencil SF_GLAD_GL_OES_packed_depth_stencil
#define GLEXT_GL_DEPTH24_STENCIL8  GL_DEPTH24_STENCIL8_OES

// Core since 3.0
#define GLEXT_framebuffer_blit false
#define GLEXT_glBlitFramebuffer \
    glBlitFramebufferEXT // Placeholder to satisfy the compiler, entry point is not loaded in GLES
#define GLEXT_GL_READ_FRAMEBUFFER         0
#define GLEXT_GL_DRAW_FRAMEBUFFER         0
#define GLEXT_GL_DRAW_FRAMEBUFFER_BINDING 0
#define GLEXT_GL_READ_FRAMEBUFFER_BINDING 0

// Core since 3.0
#define GLEXT_framebuffer_multisample false
#define GLEXT_glRenderbufferStorageMultisample \
    glRenderbufferStorageMultisampleEXT // Placeholder to satisfy the compiler, entry point is not loaded in GLES
#define GLEXT_GL_MAX_SAMPLES 0

// Core since 3.0 - NV_copy_buffer
#define GLEXT_copy_buffer          false
#define GLEXT_GL_COPY_READ_BUFFER  0
#define GLEXT_GL_COPY_WRITE_BUFFER 0
#define GLEXT_glCopyBufferSubData \
    glCopyBufferSubData // Placeholder to satisfy the compiler, entry point is not loaded in GLES

// Core since 3.0 - EXT_sRGB
#define GLEXT_texture_sRGB    false
#define GLEXT_GL_SRGB8_ALPHA8 0

// Core since 3.0 - ARB_framebuffer_sRGB
#define GLEXT_framebuffer_sRGB false

// Core since 3.0 - EXT_blend_minmax
#define GLEXT_blend_minmax SF_GLAD_GL_EXT_blend_minmax
// glBlendEquation is provided by OES_blend_subtract, see above
#define GLEXT_GL_MIN GL_MIN_EXT
#define GLEXT_GL_MAX GL_MAX_EXT

#define GLEXT_EXT_blend_minmax_dependencies SF_GLAD_GL_EXT_blend_minmax

#else

#define GLEXT_packed_depth_stencil true
#define GLEXT_GL_DEPTH24_STENCIL8  GL_DEPTH24_STENCIL8

#define GLEXT_framebuffer_blit true
#define GLEXT_glBlitFramebuffer glBlitFramebuffer
#define GLEXT_GL_READ_FRAMEBUFFER         GL_READ_FRAMEBUFFER
#define GLEXT_GL_DRAW_FRAMEBUFFER         GL_DRAW_FRAMEBUFFER
#define GLEXT_GL_DRAW_FRAMEBUFFER_BINDING GL_DRAW_FRAMEBUFFER_BINDING
#define GLEXT_GL_READ_FRAMEBUFFER_BINDING GL_READ_FRAMEBUFFER_BINDING

#define GLEXT_framebuffer_multisample true
#define GLEXT_glRenderbufferStorageMultisample glRenderbufferStorageMultisample
#define GLEXT_GL_MAX_SAMPLES GL_MAX_SAMPLES

// Core since 3.0 - NV_copy_buffer
#define GLEXT_copy_buffer          true
#define GLEXT_GL_COPY_READ_BUFFER  GL_COPY_READ_BUFFER
#define GLEXT_GL_COPY_WRITE_BUFFER GL_COPY_WRITE_BUFFER
#define GLEXT_glCopyBufferSubData glCopyBufferSubData

#define GLEXT_texture_sRGB    true
#define GLEXT_GL_SRGB8_ALPHA8 GL_SRGB8_ALPHA8

#define GLEXT_blend_minmax true
#define GLEXT_GL_MIN GL_MIN
#define GLEXT_GL_MAX GL_MAX

#endif

// OpenGL Versions
#define GLEXT_GL_VERSION_1_0 SF_GLAD_GL_VERSION_1_0
#define GLEXT_GL_VERSION_1_1 SF_GLAD_GL_VERSION_1_1
#define GLEXT_GL_VERSION_1_2 SF_GLAD_GL_VERSION_1_2
#define GLEXT_GL_VERSION_1_3 SF_GLAD_GL_VERSION_1_3
#define GLEXT_GL_VERSION_1_4 SF_GLAD_GL_VERSION_1_4
#define GLEXT_GL_VERSION_1_5 SF_GLAD_GL_VERSION_1_5
#define GLEXT_GL_VERSION_2_0 SF_GLAD_GL_VERSION_2_0
#define GLEXT_GL_VERSION_2_1 SF_GLAD_GL_VERSION_2_1
#define GLEXT_GL_VERSION_3_0 SF_GLAD_GL_VERSION_3_0
#define GLEXT_GL_VERSION_3_1 SF_GLAD_GL_VERSION_3_1
#define GLEXT_GL_VERSION_3_2 SF_GLAD_GL_VERSION_3_2
#define GLEXT_GL_VERSION_3_3 SF_GLAD_GL_VERSION_3_3

namespace sf::priv
{

////////////////////////////////////////////////////////////
/// \brief Make sure that extensions are initialized
///
////////////////////////////////////////////////////////////
void ensureExtensionsInit();

} // namespace sf::priv
