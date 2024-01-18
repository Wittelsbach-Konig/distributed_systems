//
// Created by artem on 15.12.2023.
//

#include "queue_operations.h"

void add_element_to_queue(ProcessQueue *queue, Pairs time_pid) {
    if (queue->len == 0) {
        queue->processes[queue->len++] = time_pid;
        return;
    }
    queue->len++;
    //assert(queue->length <= MAX_PROCESS_ID);
    for (int i = 0; i < queue->len - 1; i++) {
        Pairs compared_pairs = queue->processes[i];
        int request_have_smaller_time = time_pid.lamport_time < compared_pairs.lamport_time;
        int time_is_equal_but_smaller_id = time_pid.lamport_time == compared_pairs.lamport_time && time_pid.process_id < compared_pairs.process_id;
        if (request_have_smaller_time || time_is_equal_but_smaller_id) {
            for (int j = queue->len - 2; j >= i; j--) {
                queue->processes[j + 1] = queue->processes[j];
            }
            queue->processes[i] = time_pid;
            return;
        }
    }
    queue->processes[queue->len - 1] = time_pid;
}

void delete_element_from_queue(ProcessQueue *queue) {
    for (int i = 0; i < queue->len - 1; i++) {
        queue->processes[i] = queue->processes[i + 1];
    }
    queue->len = queue->len - 1;
}
