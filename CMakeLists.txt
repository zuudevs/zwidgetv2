cmake_minimum_required(VERSION 4.2.0)
project(zwidget VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

include_directories(include)

set(zwidget_sources
	src/test_multi_window.cpp
)

add_executable(${PROJECT_NAME} ${zwidget_sources})

add_compile_options(PRIVATE
	
)