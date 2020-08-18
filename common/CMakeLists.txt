target_sources(win32api PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp)
target_link_libraries(win32api PRIVATE
        buffer)

add_subdirectory(buffer)
add_subdirectory(unicode)