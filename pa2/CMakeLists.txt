cmake_minimum_required(VERSION 3.16)
project(lab2_new C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_C_FLAGS "-std=c99 -Wall -pedantic -Werror")

set(PIPES pipe_operations.h pipe_operations.c)
set(PROCESS_ACTIVITY child_operations.c child_operations.h
        parent_operations.c parent_operations.h
        general_actions.c general_actions.h)
set(LOGGER log.h log.c)
set(BANKING banking.h bank_robbery.c banking.c)

add_executable(lab2_new main.c constants.h ipc.h ipc.c  pa2345.h common.h
        ${PIPES} ${PROCESS_ACTIVITY} ${LOGGER} ${BANKING} lamport_logical_time.c lamport_logical_time.h)

find_library(libruntime NAMES libruntime.so PATHS "${PROJECT_SOURCE_DIR}/lib64//")

target_link_libraries(lab2_new PRIVATE ${libruntime})