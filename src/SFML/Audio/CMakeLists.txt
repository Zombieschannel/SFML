
set(INCROOT ${PROJECT_SOURCE_DIR}/include/SFML/Audio)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/SFML/Audio)

# all source files
set(SRC
    ${SRCROOT}/ALCheck.cpp
    ${SRCROOT}/ALCheck.hpp
    ${SRCROOT}/AlResource.cpp
    ${INCROOT}/AlResource.hpp
    ${SRCROOT}/AudioDevice.cpp
    ${SRCROOT}/AudioDevice.hpp
    ${INCROOT}/Export.hpp
    ${SRCROOT}/Listener.cpp
    ${INCROOT}/Listener.hpp
    ${SRCROOT}/Music.cpp
    ${INCROOT}/Music.hpp
    ${SRCROOT}/Sound.cpp
    ${INCROOT}/Sound.hpp
    ${SRCROOT}/SoundBuffer.cpp
    ${INCROOT}/SoundBuffer.hpp
    ${SRCROOT}/SoundBufferRecorder.cpp
    ${INCROOT}/SoundBufferRecorder.hpp
    ${SRCROOT}/InputSoundFile.cpp
    ${INCROOT}/InputSoundFile.hpp
    ${SRCROOT}/OutputSoundFile.cpp
    ${INCROOT}/OutputSoundFile.hpp
    ${SRCROOT}/SoundRecorder.cpp
    ${INCROOT}/SoundRecorder.hpp
    ${SRCROOT}/SoundSource.cpp
    ${INCROOT}/SoundSource.hpp
    ${SRCROOT}/SoundStream.cpp
    ${INCROOT}/SoundStream.hpp
)
source_group("" FILES ${SRC})

set(CODECS_SRC
    ${SRCROOT}/SoundFileFactory.cpp
    ${INCROOT}/SoundFileFactory.hpp
    ${INCROOT}/SoundFileFactory.inl
    ${INCROOT}/SoundFileReader.hpp
    ${SRCROOT}/SoundFileReaderFlac.hpp
    ${SRCROOT}/SoundFileReaderFlac.cpp
    ${SRCROOT}/SoundFileReaderMp3.hpp
    ${SRCROOT}/SoundFileReaderMp3.cpp
    ${SRCROOT}/SoundFileReaderOgg.hpp
    ${SRCROOT}/SoundFileReaderOgg.cpp
    ${SRCROOT}/SoundFileReaderWav.hpp
    ${SRCROOT}/SoundFileReaderWav.cpp
    ${INCROOT}/SoundFileWriter.hpp
    ${SRCROOT}/SoundFileWriterFlac.hpp
    ${SRCROOT}/SoundFileWriterFlac.cpp
    ${SRCROOT}/SoundFileWriterOgg.hpp
    ${SRCROOT}/SoundFileWriterOgg.cpp
    ${SRCROOT}/SoundFileWriterWav.hpp
    ${SRCROOT}/SoundFileWriterWav.cpp
)
source_group("codecs" FILES ${CODECS_SRC})

# let CMake know about our additional audio libraries paths (on Windows and OSX)
if(SFML_OS_WINDOWS)
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${PROJECT_SOURCE_DIR}/extlibs/headers/AL")
elseif(SFML_OS_MACOSX)
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${PROJECT_SOURCE_DIR}/extlibs/libs-osx/Frameworks")
elseif(SFML_OS_ANDROID)
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${PROJECT_SOURCE_DIR}/extlibs/headers/AL")
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${PROJECT_SOURCE_DIR}/extlibs/android")
endif()

set(EXTLIBS_SRC "")

if(SFML_OS_EMSCRIPTEN)
    # OpenAL provided automatically by the toolchain
    set(VORBIS_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/extlibs/headers")
    set(FLAC_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/extlibs/headers")

    set(EXTLIBS_SRC
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libogg/bitwise.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libogg/framing.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/analysis.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/bitrate.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/block.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/codebook.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/envelope.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/floor0.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/floor1.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/info.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/lookup.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/lpc.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/lsp.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/mapping0.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/mdct.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/psy.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/registry.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/res0.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/sharedbook.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/smallft.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/synthesis.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/vorbisenc.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/vorbisfile.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis/window.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/bitmath.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/bitreader.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/bitwriter.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/cpu.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/crc.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/fixed.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/fixed_intrin_sse2.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/fixed_intrin_ssse3.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/float.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/format.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/lpc.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/lpc_intrin_avx2.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/lpc_intrin_sse.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/lpc_intrin_sse2.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/lpc_intrin_sse41.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/md5.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/memory.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/metadata_iterators.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/metadata_object.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/ogg_decoder_aspect.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/ogg_encoder_aspect.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/ogg_helper.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/ogg_mapping.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/stream_decoder.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/stream_encoder.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/stream_encoder_framing.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/stream_encoder_intrin_avx2.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/stream_encoder_intrin_sse2.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/stream_encoder_intrin_ssse3.c
        ${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/window.c
    )
    source_group("extlibs" FILES ${EXTLIBS_SRC})

    include_directories(${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libvorbis)
    include_directories(${PROJECT_SOURCE_DIR}/extlibs/libs-emscripten/libflac/include)
endif()

# find external libraries
if (NOT SFML_OS_EMSCRIPTEN)
    sfml_find_package(OpenAL INCLUDE "OPENAL_INCLUDE_DIR" LINK "OPENAL_LIBRARY")
endif()
sfml_find_package(VORBIS INCLUDE "VORBIS_INCLUDE_DIRS" LINK "VORBIS_LIBRARIES")
sfml_find_package(FLAC INCLUDE "FLAC_INCLUDE_DIR" LINK "FLAC_LIBRARY")

# avoids warnings in vorbisfile.h
target_compile_definitions(VORBIS INTERFACE "OV_EXCLUDE_STATIC_CALLBACKS")
target_compile_definitions(FLAC INTERFACE "FLAC__NO_DLL")

# define the sfml-audio target
sfml_add_library(sfml-audio
                 SOURCES ${SRC} ${CODECS_SRC} ${EXTLIBS_SRC})

# setup dependencies
target_link_libraries(sfml-audio PRIVATE OpenAL)

# minimp3 sources
target_include_directories(sfml-audio SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/extlibs/headers/minimp3")

if(SFML_OS_ANDROID)
    target_link_libraries(sfml-audio PRIVATE android OpenSLES)
endif()

target_link_libraries(sfml-audio
                      PUBLIC sfml-system
                      PRIVATE VORBIS FLAC)
