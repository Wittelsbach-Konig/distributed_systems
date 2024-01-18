//
// Created by artem on 14.12.2023.
//

#ifndef LAB3_CHILD_OPERATIONS_H
#define LAB3_CHILD_OPERATIONS_H


#include "constants.h"

void send_STARTED_message(process_content*);

void send_DONE_message(process_content*);

void process_queries(process_content*, int);

#endif //LAB3_CHILD_OPERATIONS_H
