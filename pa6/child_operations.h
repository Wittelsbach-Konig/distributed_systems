//
// Created by artem on 07.12.2023.
//

#ifndef LAB1_CHILD_OPERATIONS_H
#define LAB1_CHILD_OPERATIONS_H

#include "constants.h"

void send_STARTED_message(process_content*);

void send_DONE_message(process_content*);

void handle_requests_without_cs(process_content*);

void handle_requests(process_content*);

int fork_can_be_requested(Fork fork);

int fork_can_be_released(Fork fork);

void forks_array_init(Fork *forks, local_id forks_num, local_id child_id);

#endif //LAB1_CHILD_OPERATIONS_H
