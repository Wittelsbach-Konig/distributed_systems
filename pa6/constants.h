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

typedef enum {
    FORK_FREE = 0,
    FORK_OWNS = 1,
} FORK_OWNERSHIP;

typedef enum {
    FORK_CLEAN = 0,
    FORK_DIRTY = 1,
} FORK_PURITY;

typedef enum {
    FORK_MISSING_TOKEN = 0,
    FORK_HAVE_TOKEN = 1,
} FORK_REQUEST_TOKEN;

typedef struct {
    /* Имеет ли процесс общую вилку с другим процессом */
    FORK_OWNERSHIP ownership;
    /* Чистая или грязная вилка */
    FORK_PURITY purity;
    /* Есть ли запрос у процесса на общую с другим процессом вилку*/
    FORK_REQUEST_TOKEN request_token; 
} Fork;

typedef struct {
    local_id request_from;
} Request;

typedef struct {
    local_id this_process;
    int write_pipes[MAX_PROCESS_NUM][MAX_PROCESS_NUM];
    int read_pipes[MAX_PROCESS_NUM][MAX_PROCESS_NUM];
    uint8_t process_num;
    uint8_t print_iterations;
    local_id forks_num;
    Fork forks[MAX_PROCESS_ID];
    uint8_t using_mutex;
} process_content;

#endif //LAB2_CONSTANTS_H
