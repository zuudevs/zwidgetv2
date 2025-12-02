cmake_minimum_required(VERSION 3.28) # 4.2.0? Kamu dari masa depan? CMake sekarang masih v3.x lho.
project(zwidget VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directory biar rapi
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

include_directories(include)

# Source files
set(zwidget_sources
    src/test_multi_window.cpp
)

# 1. Define Executable dulu
add_executable(${PROJECT_NAME} ${zwidget_sources})

# 2. BARU Link library-nya ke executable tersebut
target_link_libraries(${PROJECT_NAME} PRIVATE 
    d2d1 
    dwrite 
    windowscodecs
    shcore
)

# Tambahan: Nyalain warning level tinggi biar error "reference to local temp" tadi kelihatan jelas
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()