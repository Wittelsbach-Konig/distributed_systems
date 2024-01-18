//
// Created by artem on 04.12.2023.
//

#ifndef LAB1_CONSTANTS_H
#define LAB1_CONSTANTS_H
#define MAX_PROCESS_NUM 11

#include "ipc.h"

typedef enum{
    NOT_MAGIC_SIGN = 1,
    SEND_UNSUCCESFUL,
    MULTICAST_SEND_UNSUCCESFUL,
    RECIEVE_UNSUCCESFUL,
    NO_DATA_RECIEVED

}Error_statuses;


typedef struct {
    timestamp_t lamport_time;
    local_id process_id;
} Pairs;

typedef struct{
    uint8_t len;
    Pairs processes[MAX_PROCESS_NUM];
} ProcessQueue;

typedef struct {
    local_id this_process;
    int write_pipes[MAX_PROCESS_NUM][MAX_PROCESS_NUM];
    int read_pipes[MAX_PROCESS_NUM][MAX_PROCESS_NUM];
    uint8_t process_num;
    uint8_t print_iterations;
    ProcessQueue process_queue;
} process_content;

#endif //LAB1_CONSTANTS_H
