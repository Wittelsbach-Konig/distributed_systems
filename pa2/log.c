//
// Created by Vitaliy on 09.12.2023
//

#include <stdio.h>
#include <unistd.h>

#include "common.h"
#include "pa2345.h"
#include "log.h"


int logging_preparation(void) {
    if (
        (pipes_log_f = fopen(pipes_log, "w")) == NULL ||
        (events_log_f = fopen(events_log, "w")) == NULL
    ) {
        perror("fopen() failed");
        logging_finalize();
        return -1;
    }

    return 0;
}

void logging_finalize(void) {
    fclose(pipes_log_f);
    fclose(events_log_f);
}

void logging_process_started(timestamp_t lamport_time, local_id id, balance_t amount) {
    printf(log_started_fmt, lamport_time, id, getpid(), getppid(), amount);
    fprintf(events_log_f, log_started_fmt, lamport_time, id, getpid(), getppid(), amount);
}

void logging_received_all_started_messages(timestamp_t lamport_time, local_id id) {
    printf(log_received_all_started_fmt, lamport_time, id);
    fprintf(events_log_f, log_received_all_started_fmt, lamport_time, id);
}

void logging_process_done(timestamp_t lamport_time, local_id id, balance_t amount) {
    printf(log_done_fmt, lamport_time, id, amount);
    fprintf(events_log_f, log_done_fmt, lamport_time, id, amount);
}

void logging_received_all_done_messages(timestamp_t lamport_time, local_id id) {
    printf(log_received_all_done_fmt, lamport_time, id);
    fprintf(events_log_f, log_received_all_done_fmt, lamport_time, id);
}

void logging_pipes_created(local_id to, local_id from, int read_fd, int write_fd) {
    printf(log_pipes_created_fmt, to, from, read_fd, write_fd);
    fprintf(pipes_log_f, log_pipes_created_fmt, to, from, read_fd, write_fd);
}

void logging_transfer_out(timestamp_t lamport_time, local_id from, balance_t amount, local_id to){
    printf(log_transfer_out_fmt, lamport_time, from, amount, to);
    fprintf(events_log_f, log_transfer_out_fmt, lamport_time, from, amount, to);
}

void logging_transfer_in(timestamp_t lamport_time, local_id from, balance_t amount, local_id to){
    printf(log_transfer_in_fmt, lamport_time, from, amount, to);
    fprintf(events_log_f, log_transfer_in_fmt, lamport_time, from, amount, to);
}
