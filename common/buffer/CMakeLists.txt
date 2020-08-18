add_library(buffer INTERFACE)
    target_sources(buffer
            INTERFACE
                ${CMAKE_CURRENT_SOURCE_DIR}/include/buffer/buffer.hpp
                ${CMAKE_CURRENT_SOURCE_DIR}/include/buffer/buffer_span.hpp
                ${CMAKE_CURRENT_SOURCE_DIR}/include/buffer/buffer_view.hpp
                ${CMAKE_CURRENT_SOURCE_DIR}/include/buffer/small_buffer.hpp
                ${CMAKE_CURRENT_SOURCE_DIR}/include/buffer/static_buffer.hpp)
    target_compile_features(buffer
            INTERFACE
                cxx_std_17)
    target_include_directories(buffer
            INTERFACE
                ${CMAKE_CURRENT_SOURCE_DIR}/include)