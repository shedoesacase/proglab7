cmake_minimum_required(VERSION 3.10.0)
project(LAB7 VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED COMPONENTS filesystem system program_options)
find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(LAB7 CommandLineParser.cpp FileScanner.cpp main.cpp DuplicateFinder.cpp Hash.cpp)

target_link_libraries(LAB7 ${Boost_LIBRARIES} ZLIB::ZLIB OpenSSL::Crypto)

set(CPACK_PACKAGE_NAME "LAB7")

set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

if(WIN32)
    target_compile_definitions(LAB7 PRIVATE BOOST_ALL_NO_LIB _CRT_SECURE_NO_WARNINGS)
    if(MSVC)
        target_compile_options(LAB7 PRIVATE /W4)
    else()
        target_compile_options(LAB7 PRIVATE -Wall -Wextra)
    endif()
endif()

if(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Ivan Obuhov <vanushabrend@gmail.com>")
endif()

install(TARGETS LAB7 DESTINATION bin)
include(CPack)