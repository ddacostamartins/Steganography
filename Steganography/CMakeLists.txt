cmake_minimum_required(VERSION 3.16)
project(Steganography LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(stego
    Main.cpp
    Files.cpp
    Bits.cpp
    Steganography.cpp
)

if(MSVC)
    target_compile_options(stego PRIVATE /W4)
else()
    target_compile_options(stego PRIVATE -Wall -Wextra -Wpedantic)
endif()