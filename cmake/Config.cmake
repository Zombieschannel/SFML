# detect the OS
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(SFML_OS_WINDOWS 1)

    # don't use the OpenGL ES implementation on Windows
    set(OPENGL_ES 0)

    # detect the architecture
    if("${CMAKE_GENERATOR_PLATFORM}" MATCHES "ARM64" OR "${MSVC_CXX_ARCHITECTURE_ID}" MATCHES "ARM64" OR "${CMAKE_SYSTEM_PROCESSOR}" MATCHES "ARM64")
        set(ARCH_ARM64 1)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(ARCH_X86 1)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH_X64 1)
    else()
        message(FATAL_ERROR "Unsupported architecture")
        return()
    endif()
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(SFML_OS_UNIX 1)

    if(ANDROID)
        set(SFML_OS_ANDROID 1)

        # use the OpenGL ES implementation on Android
        set(OPENGL_ES 1)
    else()
        set(SFML_OS_LINUX 1)

        # don't use the OpenGL ES implementation on Linux
        set(OPENGL_ES 0)
    endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "^k?FreeBSD$")
    set(SFML_OS_FREEBSD 1)

    # don't use the OpenGL ES implementation on FreeBSD
    set(OPENGL_ES 0)
elseif(CMAKE_SYSTEM_NAME MATCHES "^OpenBSD$")
    set(SFML_OS_OPENBSD 1)

    # don't use the OpenGL ES implementation on OpenBSD
    set(OPENGL_ES 0)
elseif(CMAKE_SYSTEM_NAME MATCHES "^NetBSD$")
    set(SFML_OS_NETBSD 1)

    # don't use the OpenGL ES implementation on NetBSD
    set(OPENGL_ES 0)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    set(SFML_OS_IOS 1)

    # use the OpenGL ES implementation on iOS
    set(OPENGL_ES 1)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(SFML_OS_MACOS 1)

    # don't use the OpenGL ES implementation on macOS
    set(OPENGL_ES 0)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    set(SFML_OS_ANDROID 1)

    # use the OpenGL ES implementation on Android
    set(OPENGL_ES 1)

# comparing CMAKE_SYSTEM_NAME with "CYGWIN" generates a false warning depending on the CMake version
# let's avoid it so the actual error is more visible
elseif(${CYGWIN})
    message(FATAL_ERROR "Unfortunately SFML doesn't support Cygwin's 'hybrid' status between both Windows and Linux derivatives.\nIf you insist on using the GCC, please use a standalone build of MinGW without the Cygwin environment instead.")
elseif(${EMSCRIPTEN})
    set(SFML_OS_EMSCRIPTEN 1)
    # use the OpenGL ES implementation on Emscripten
    set(OPENGL_ES 1)
    set(SFML_EMSCRIPTEN_TARGET_COMPILE_OPTIONS_DEBUG -g3 -gsource-map -O0)
    set(SFML_EMSCRIPTEN_TARGET_COMPILE_OPTIONS_RELEASE -O3)
    set(SFML_EMSCRIPTEN_TARGET_COMPILE_OPTIONS
        $<$<CONFIG:Debug>:${SFML_EMSCRIPTEN_TARGET_COMPILE_OPTIONS_DEBUG}>
        $<$<CONFIG:Release>:${SFML_EMSCRIPTEN_TARGET_COMPILE_OPTIONS_RELEASE}>
        #-pthread     # Enable threading support
    )
    set(SFML_EMSCRIPTEN_TARGET_LINK_OPTIONS_DEBUG -g3 -gsource-map -sASSERTIONS=2 -sCHECK_NULL_WRITES=1 -sSAFE_HEAP=1 -sSTACK_OVERFLOW_CHECK=1)
    set(SFML_EMSCRIPTEN_TARGET_LINK_OPTIONS_RELEASE -O3 -SMINIFY_HTML=1)
    set(SFML_EMSCRIPTEN_TARGET_LINK_OPTIONS_ASYNCIFY
        -sASYNCIFY=1                        # Support async operations in the compiled code (used for game loop)
        -sASYNCIFY_IGNORE_INDIRECT=1        # Assume indirect calls can’t lead to an unwind/rewind of the stack (faster)
    )
    set(SFML_EMSCRIPTEN_TARGET_LINK_OPTIONS_JSPI
        -sJSPI=1                            # Use VM support for the JavaScript Promise Integration proposal
    )
    set(SFML_EMSCRIPTEN_TARGET_LINK_OPTIONS
        $<$<CONFIG:Debug>:${SFML_EMSCRIPTEN_TARGET_LINK_OPTIONS_DEBUG}>
        $<$<CONFIG:Release>:${SFML_EMSCRIPTEN_TARGET_LINK_OPTIONS_RELEASE}>
        ${SFML_EMSCRIPTEN_TARGET_LINK_OPTIONS_ASYNCIFY}
        # ${SFML_EMSCRIPTEN_TARGET_LINK_OPTIONS_JSPI}
        #-pthread                            # Enable threading support
        -Wno-limited-postlink-optimizations # warning: running limited binaryen optimizations because DWARF info requested (or indirectly required)
        -Wno-pthreads-mem-growth            # warning: -pthread + ALLOW_MEMORY_GROWTH may run non-wasm code slowly, see https://github.com/WebAssembly/design/issues/1271
        -sALLOW_MEMORY_GROWTH=1             # Grow the memory arrays at runtime
        -sEXIT_RUNTIME=1                    # Execute cleanup (e.g. `atexit`) after `main` completes
        -sFETCH=1                           # Enables `emscripten_fetch` API
        -sFORCE_FILESYSTEM=1                # Makes full filesystem support be included
        -sFULL_ES2=1                        # Forces support for all GLES2 features, not just the WebGL-friendly subset
        -sMAX_WEBGL_VERSION=1               # Specifies the highest WebGL version to target
        -sMIN_WEBGL_VERSION=1               # Specifies the lowest WebGL version to target
        -sSTACK_SIZE=4mb                    # Set the total stack size
        #-sUSE_PTHREADS=1                    # Enable threading support
        -sWASM=1                            # Compile code to WebAssembly
        -sGL_EXPLICIT_UNIFORM_LOCATION=1
        -sGL_EXPLICIT_UNIFORM_BINDING=1
        --emrun                             # Add native support for `emrun` (I/O capture)
    )
else()
    message(FATAL_ERROR "Unsupported operating system or environment")
    return()
endif()

# detect the compiler
# Note: The detection is order is important because:
# - Visual Studio can both use MSVC and Clang
# - GNUCXX can still be set on macOS when using Clang
if(MSVC)
    set(SFML_COMPILER_MSVC 1)

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(SFML_COMPILER_CLANG_CL 1)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(SFML_COMPILER_CLANG 1)

    execute_process(COMMAND "${CMAKE_CXX_COMPILER}" "-v" OUTPUT_VARIABLE CLANG_COMPILER_VERSION ERROR_VARIABLE CLANG_COMPILER_VERSION)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(SFML_COMPILER_GCC 1)

    execute_process(COMMAND "${CMAKE_CXX_COMPILER}" "-v" OUTPUT_VARIABLE GCC_COMPILER_VERSION ERROR_VARIABLE GCC_COMPILER_VERSION)
    string(REGEX MATCHALL ".*(tdm[64]*-[1-9]).*" SFML_COMPILER_GCC_TDM "${GCC_COMPILER_VERSION}")
else()
    message(WARNING "Unrecognized compiler: ${CMAKE_CXX_COMPILER_ID}. Use at your own risk.")
endif()
