cmake_minimum_required(VERSION 3.14)

include(FetchContent)
project(Splitter
    VERSION 1.0.0
    DESCRIPTION "Window library")


set_property(GLOBAL PROPERTY USE_FOLDERS ON)


set(INITIALIZATION
    main.cpp
)

set(ENGINE
    window.cpp
    window.h
    graphics.mm
    graphics.h
    gui.cpp
    gui.h
)
set(SDLTTF_VENDORED ON)
if((APPLE AND NOT CMAKE_SYSTEM_NAME MATCHES "Darwin") OR EMSCRIPTEN)
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "") # Disable shared builds on platforms where it does not make sense to use them
    set(SDL_SHARED OFF)
else()
    set(SDL_SHARED ON)
endif()

add_subdirectory(dependency/SDL)
add_subdirectory(dependency/SDL2Image)
add_subdirectory(dependency/SDL_ttf)


# set some extra configs for each platform
set_target_properties(${EXECUTABLE_NAME} PROPERTIES 
    # On macOS, make a proper .app bundle instead of a bare executable
    MACOSX_BUNDLE TRUE
    # Set the Info.plist file for Apple Mobile platforms. Without this file, your app
    # will not launch. 
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"

    # in Xcode, create a Scheme in the schemes dropdown for the app.
    XCODE_GENERATE_SCHEME TRUE
    # Identification for Xcode
    XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.ravbug.sdl3-sample"
	XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.ravbug.sdl3-sample"
	XCODE_ATTRIBUTE_CURRENTYEAR "${CURRENTYEAR}"
)

list(TRANSFORM ENGINE PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/engine/)
source_group("Engine" FILES ${ENGINE})

include(dependency/sokol-toolkit-bin/sokol_shader_utils.cmake)

sokol_shader(triangle-sapp.glsl metal_macos)
# sokol_shader(triangle-sapp.glsl glsl300es)


message("ssssssss")
add_executable(${PROJECT_NAME} ${INITIALIZATION} ${ENGINE} dependency/glad/src/glad.c triangle-sapp.glsl.h)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/dependency/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dependency/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dependency/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/dependency/sokol
)

make_directory(${CMAKE_CURRENT_BINARY_DIR}/compile_shaders)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/compile_shaders)

target_link_libraries(${PROJECT_NAME}
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf

    "-framework Metal"
    "-framework MetalKit"
    "-framework AppKit"
    "-framework Foundation"
    "-framework QuartzCore"
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
