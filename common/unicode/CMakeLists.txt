find_package(ICU REQUIRED COMPONENTS dt in uc)

add_library(unicode STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/unicode.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/unicodestring.cpp)
    set_target_properties(unicode PROPERTIES
            CXX_VISIBILITY_PRESET       hidden)
    target_compile_features(unicode
            PUBLIC
                cxx_std_17)
    target_compile_definitions(unicode
            PRIVATE
                U_USING_ICU_NAMESPACE=0)
    target_include_directories(unicode
            PUBLIC
                ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(unicode
            PRIVATE
                ICU::dt ICU::in ICU::uc
                fmt::fmt)