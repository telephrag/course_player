
cmake_minimum_required(VERSION 3.18)

project(
    coursework
        LANGUAGES CXX C 
        )

set(LIBVLC_HEADERS /usr/include/vlc/)
set(LIBVLC_STATIC  /usr/lib64/)

add_library(libvlc SHARED IMPORTED GLOBAL)
set_target_properties(libvlc PROPERTIES
    IMPORTED_LOCATION ${LIBVLC_STATIC}/libvlc.so
    INTERFACE_INCLUDE_DIRECTORIES ${LIBVLC_HEADERS}
    LINKER_LANGUAGE C
    )

        add_executable(coursework main.cpp)
        
 target_link_libraries(coursework PRIVATE libvlc)
target_compile_options(coursework PRIVATE -lpthread)
 target_link_libraries(coursework PRIVATE pthread)

target_include_directories(coursework PRIVATE threads_src/playlist/)
target_include_directories(coursework PRIVATE threads_src/play/)
