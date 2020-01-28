cmake_minimum_required(VERSION 3.14)
project(win32api)

find_package(PkgConfig REQUIRED)

find_package(Boost REQUIRED COMPONENTS locale log)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(X11 REQUIRED)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)


set(WIN32_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/advapi32
    ${CMAKE_CURRENT_SOURCE_DIR}/gdi32
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel32
    ${CMAKE_CURRENT_SOURCE_DIR}/user32)

if(${BUILD_WIN32_STATIC})
    add_library(win32api STATIC ${CMAKE_CURRENT_SOURCE_DIR}/lib.cpp)
else()
    add_library(win32api SHARED ${CMAKE_CURRENT_SOURCE_DIR}/lib.cpp)
endif()
    set_target_properties(win32api PROPERTIES
        CXX_VISIBILITY_PRESET       hidden)
    target_compile_features(win32api
        PRIVATE
            cxx_std_17)
    target_compile_definitions(win32api
        PRIVATE
            WIN32_EXPORTS
            $<$<CONFIG:Debug>:TRACE>)
    target_include_directories(win32api
        PUBLIC
            ${WIN32_INCLUDE_DIRS}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/common)
    target_link_libraries(win32api
        PRIVATE
            Boost::locale
            fmt::fmt
            PkgConfig::SDL2
            spdlog::spdlog_header_only
            X11::X11)

add_subdirectory(rgssad/)

add_subdirectory(advapi32/)
add_subdirectory(common/)
add_subdirectory(gdi32/)
add_subdirectory(kernel32/)
add_subdirectory(user32/)

add_subdirectory(extra/camouse)
add_subdirectory(extra/tktk_bitmap)
add_subdirectory(extra/wfcrypt)
