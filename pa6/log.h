//
// Created by Vitaliy on 08.12.2023
//

#ifndef LAB1_LOG_H
#define LAB1_LOG_H

#include "ipc.h"
#include <stdio.h>

static FILE* pipes_log_f = NULL;
static FILE* events_log_f = NULL;

static const char * const log_pipes_created_fmt =
    "Pipe from processes %1d to %1d created, read_fd %2d and write_fd %2d\n";

int logging_preparation(void);

void logging_process_started(timestamp_t lamport_time, local_id id);

void logging_received_all_started_messages(timestamp_t lamport_time, local_id id);

void logging_process_done(timestamp_t lamport_time, local_id id);

void logging_received_all_done_messages(timestamp_t lamport_time, local_id id);

void logging_pipes_created(local_id to, local_id from, int read_fd, int write_fd);

void logging_finalize(void);

#endif // LAB1_LOG_H
