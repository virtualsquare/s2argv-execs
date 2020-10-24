cmake_minimum_required(VERSION 3.12)
project(s2argv-execs
    VERSION 1.3
    DESCRIPTION "execute a file taking its arguments from a string"
    HOMEPAGE_URL "https://github.com/rd235/s2argv-execs"
    LANGUAGES C)

set(LIB_VERSION 0.9.0)
set(LIB_SOVERSION 0)

include(GNUInstallDirs)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(execs SHARED execs.c noshell.c)
set_target_properties(execs PROPERTIES VERSION ${LIB_VERSION}
        SOVERSION ${LIB_SOVERSION})

add_library(execs-embedded SHARED execs.c)
target_compile_options(execs-embedded PUBLIC -DEEXECS)
set_target_properties(execs-embedded PROPERTIES VERSION ${LIB_VERSION}
        SOVERSION ${LIB_SOVERSION})

add_library(execs_static STATIC execs.c noshell.c)
set_target_properties(execs_static PROPERTIES OUTPUT_NAME execs)

add_library(execs-embedded_static STATIC execs.c)
set_target_properties(execs-embedded_static PROPERTIES OUTPUT_NAME execs-embedded)
target_compile_options(execs-embedded_static PUBLIC -DEEXECS)

install(TARGETS execs LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS execs-embedded LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS execs_static LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS execs-embedded_static LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES execs.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

add_executable(exectest execstest.c)
target_link_libraries(exectest execs)

add_subdirectory(man)

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${PROJECT_SOURCE_DIR}/Uninstall.cmake")

