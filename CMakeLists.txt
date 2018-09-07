cmake_minimum_required(VERSION 2.6)
project(win32api)

add_executable(win32api main.cpp)

install(TARGETS win32api RUNTIME DESTINATION bin)
