
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/emscripten)

# all source files
set(SRC ${SRCROOT}/Emscripten.cpp)

sfml_add_example(emscripten GUI_APP
                 SOURCES ${SRC}
                 DEPENDS sfml-graphics freetype)

target_compile_options(emscripten PRIVATE "--use-port=freetype")

# external dependency headers
target_include_directories(emscripten SYSTEM PRIVATE ${PROJECT_SOURCE_DIR}/examples/emscripten)
