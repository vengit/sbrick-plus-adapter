cmake_minimum_required(VERSION 3.1)
find_package (PkgConfig REQUIRED)
pkg_check_modules (GLIB2 REQUIRED glib-2.0)

include_directories(include ${GLIB2_INCLUDE_DIRS})
link_directories(${GLIB2_LIBRARY_DIRS})

add_library(EV3Lib STATIC
    src/ev3.c
    src/ev3_proc.c
    src/ev3_utils.c
    include/mode_conf.h
    include/ev3.h
    include/ev3_proc.h
    include/ev3_utils.h)

add_executable(EV3LibTest
    test/test.c
    include/mode_conf.h
    include/ev3.h
    include/ev3_proc.h
    include/ev3_utils.h)

add_definitions (${GLIB2_CFLAGS_OTHER})
target_link_libraries (EV3LibTest EV3Lib ${GLIB2_LIBRARIES})