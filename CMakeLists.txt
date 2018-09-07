cmake_minimum_required(VERSION 3.0)
project(win32api)

add_library(win32api SHARED "")
    target_include_directories(win32api PRIVATE user32/)
    target_link_libraries(win32api user32)
    
add_subdirectory(user32/)
