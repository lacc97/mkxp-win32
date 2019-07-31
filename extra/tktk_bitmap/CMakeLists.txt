find_package(PkgConfig REQUIRED)
    pkg_check_modules(png REQUIRED IMPORTED_TARGET libpng)


# Most of the code for tktk_bitmap can be found in
# https://www.tktkgame.com/tkool/rgss_common/bitmap_extension.html
# with some changes needed to remove dependencies on Windows functions.
#target_sources(win32api
#    PRIVATE
#        ${CMAKE_CURRENT_SOURCE_DIR}/tktk_bitmap.cpp)
#target_link_libraries(win32api
#    PRIVATE
#        PkgConfig::png)