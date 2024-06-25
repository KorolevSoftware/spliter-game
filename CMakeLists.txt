cmake_minimum_required(VERSION 3.14)

include(FetchContent)
project(Splitter
	 	VERSION 1.0.0
	  DESCRIPTION "Window library"
	  LANGUAGES CXX)


set_property(GLOBAL PROPERTY USE_FOLDERS ON)  

FetchContent_Declare(sdl
 GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
 GIT_TAG "ac13ca9ab691e13e8eebe9684740ddcb0d716203"
)


FetchContent_MakeAvailable(sdl)

set(INITIALIZATION
  main.cpp
)

set(ENGINE
	window.cpp
	window.h
)

list(TRANSFORM ENGINE PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/engine/)

source_group("Engine" FILES ${ENGINE})  


add_executable(${PROJECT_NAME} ${INITIALIZATION} ${ENGINE})
target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/dependency/spdlog/include/
	)

target_link_libraries(${PROJECT_NAME}
	SDL2::SDL2
	SDL2::SDL2main
)