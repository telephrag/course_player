
cmake_minimum_required(VERSION 3.18)

project(
    coursework
        LANGUAGES CXX C 
        )


set(LIBVLC_HEADERS /usr/include/vlc )
set(LIBVLC_STATIC  /usr/lib64       )

add_library(libvlc SHARED IMPORTED GLOBAL)
set_target_properties(libvlc PROPERTIES
    IMPORTED_LOCATION ${LIBVLC_STATIC}/libvlc.so
    INTERFACE_INCLUDE_DIRECTORIES ${LIBVLC_HEADERS}
    LINKER_LANGUAGE C
    )
    

set(SQLITE_HEADERS /usr/include )
set(SQLITE_STATIC  /usr/lib64   )

add_library(sqlite SHARED IMPORTED GLOBAL)
set_target_properties(sqlite PROPERTIES
    IMPORTED_LOCATION ${SQLITE_STATIC}/libsqlite3.so
    INTERFACE_INCLUDE_DIRECTORIES ${SQLITE_HEADERS}
    LINKER_LANGUAGE C
    )


set(PYTHON_HEADERS /usr/include/python3.9 )
set(PYTHON_STATIC  /usr/lib64             )

add_library(python_embeded SHARED IMPORTED GLOBAL)
set_target_properties(python_embeded PROPERTIES
    IMPORTED_LOCATION ${PYTHON_STATIC}/libpython3.9.so
    INTERFACE_INCLUDE_DIRECTORIES ${PYTHON_HEADERS}
    LINKER_LANGUAGE C
    )
    
    
        add_executable(coursework main.cpp)
 set_target_properties(coursework PROPERTIES
    LINKER_LANGUAGE CXX
    )
        target_sources(coursework PRIVATE
            threads_src/common.hpp
            threads_src/common_sqlite.hpp
            threads_src/logged.hpp
            threads_src/manage_db.hpp
            threads_src/play.hpp
            threads_src/Playlist.hpp
            threads_src/Session.hpp
            threads_src/Session.cpp
            threads_src/video_controls.hpp
            )
    
    
target_compile_options(coursework PRIVATE -l)

target_compile_options(coursework PRIVATE sqlite3)
 target_link_libraries(coursework PRIVATE sqlite)

 target_link_libraries(coursework PRIVATE libvlc)

target_compile_options(coursework PRIVATE -lpthread)
 target_link_libraries(coursework PRIVATE pthread)
 
 target_link_libraries(coursework PRIVATE python_embeded)
 
