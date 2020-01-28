find_package(Threads REQUIRED)

add_executable(rgssad ${CMAKE_CURRENT_SOURCE_DIR}/rgssad.cpp)
    set_target_properties(rgssad PROPERTIES
        CXX_VISIBILITY_PRESET   hidden)
    target_compile_features(rgssad
        PRIVATE
            cxx_std_17)
    target_include_directories(rgssad
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/..)
    target_link_libraries(rgssad
        PRIVATE
            fmt::fmt spdlog::spdlog)
