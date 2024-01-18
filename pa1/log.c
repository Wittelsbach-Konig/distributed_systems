//
// Created by Vitaliy on 09.12.2023
//

#include <stdio.h>
#include <unistd.h>

#include "common.h"
#include "pa1.h"
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

void logging_process_started(local_id id) {
    printf(log_started_fmt, id, getpid(), getppid());
    fprintf(events_log_f, log_started_fmt, id, getpid(), getppid());
}

void logging_received_all_started_messages(local_id id) {
    printf(log_received_all_started_fmt, id);
    fprintf(events_log_f, log_received_all_started_fmt, id);
}

void logging_process_done(local_id id) {
    printf(log_done_fmt, id);
    fprintf(events_log_f, log_done_fmt, id);
}

void logging_received_all_done_messages(local_id id) {
    printf(log_received_all_done_fmt, id);
    fprintf(events_log_f, log_received_all_done_fmt, id);
}

void logging_pipes_created(local_id to, local_id from, int read_fd, int write_fd) {
    printf(log_pipes_created_fmt, to, from, read_fd, write_fd);
    fprintf(pipes_log_f, log_pipes_created_fmt, to, from, read_fd, write_fd);
}
