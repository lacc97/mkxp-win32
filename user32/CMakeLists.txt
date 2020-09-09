target_sources(win32api PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/mkxpGlue.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/user32.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/user32_dc.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/vkcodes.cpp)
