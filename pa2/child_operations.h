//
// Created by artem on 07.12.2023.
//

#ifndef LAB1_CHILD_OPERATIONS_H
#define LAB1_CHILD_OPERATIONS_H

#include "constants.h"
#include "banking.h"

void send_STARTED_message(process_content*);

void save_balance_state(process_content*, timestamp_t, balance_t);

void send_DONE_message(process_content*);

void process_transfer_queries(process_content*);

void send_balance_history_to_parent(process_content* processContent);

#endif //LAB1_CHILD_OPERATIONS_H
