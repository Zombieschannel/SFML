set(SRC Emscripten.cpp)
sfml_add_example(Emscripten GUI_APP
                 SOURCES ${SRC}
                 DEPENDS SFML::Graphics)
target_include_directories(Emscripten SYSTEM PRIVATE ${PROJECT_SOURCE_DIR}/examples/emscripten)