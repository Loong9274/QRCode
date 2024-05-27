cmake_minimum_required(VERSION 3.10)
project(QRCode VERSION 1.0)

set(CMAKE_C_STANDARD 99)

include_directories(include)

add_library(qrcode STATIC src/QRCode.c)

# Add the test subdirectory
add_subdirectory(test)
