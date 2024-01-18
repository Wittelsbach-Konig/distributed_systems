cmake_minimum_required(VERSION 3.16)
project(lab3 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_C_FLAGS "-std=c99 -Wall -pedantic -Werror")

set(PIPES pipe_operations.h pipe_operations.c)
set(PROCESS_ACTIVITY child_operations.c child_operations.h
        parent_operations.c parent_operations.h
        general_actions.c general_actions.h)
set(LOGGER log.h log.c)
set(LAMPORT lamport_logical_time.c lamport_logical_time.h)
set(QUEUE queue_operations.c queue_operations.h)

add_executable(lab3 main.c constants.h ipc.h ipc.c  pa2345.h common.h
               ${PIPES} ${PROCESS_ACTIVITY} ${LOGGER} ${LAMPORT} ${QUEUE})

find_library(libruntime NAMES libruntime.so PATHS "${PROJECT_SOURCE_DIR}/lib64//")

target_link_libraries(lab3 PRIVATE ${libruntime})