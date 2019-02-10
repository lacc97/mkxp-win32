cmake_minimum_required(VERSION 3.0)
project(win32api)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED COMPONENTS filesystem locale log)
find_package(SDL2 REQUIRED)
find_package(X11 REQUIRED)

include_directories(.)
add_definitions("-DTRACE")

set(WIN32_INCLUDE_DIRS advapi32/ gdi32/ kernel32/ user32/)

add_library(win32api SHARED lib.cpp)
    target_include_directories(win32api PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIR}
        ${X11_X11_INCLUDE_PATH})
    target_include_directories(win32api PRIVATE
        common/
        ${WIN32_INCLUDE_DIRS})
    target_link_libraries(win32api
        ${Boost_LIBRARIES}
        ${SDL2_LIBRARY}
        ${X11_X11_LIB})

add_executable(win32api_test main.cpp)
    target_include_directories(win32api_test PRIVATE ${WIN32_INCLUDE_DIRS})
    target_link_libraries(win32api_test win32api)

add_subdirectory(advapi32/)
add_subdirectory(common/)
add_subdirectory(extra/)
add_subdirectory(gdi32/)
add_subdirectory(kernel32/)
add_subdirectory(user32/)
