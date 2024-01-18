//
// Created by artem on 05.12.2023.
//

#ifndef LAB1_PIPE_OPERATIONS_H
#define LAB1_PIPE_OPERATIONS_H

#include "ipc.h"
#include "constants.h"

void set_pipe_descriptors(process_content* processesContent, int process_count);
void close_extra_pipes(process_content* processContent , int process_count);

#endif //LAB1_PIPE_OPERATIONS_H
