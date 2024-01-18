//
// Created by artem on 05.12.2023.
//
#include <unistd.h>
#include "pipe_operations.h"
#include "constants.h"
#include "log.h"

void set_pipe_descriptors(process_content* processContent, int process_count) {
    for(uint8_t from = 0; from < process_count - 1; ++from){
        for(uint8_t to = 1 + from; to < process_count; ++to){
            int file_descriptors[2];
            pipe(file_descriptors);
            processContent->read_pipes[from][to] = file_descriptors[0];
            processContent->write_pipes[from][to] = file_descriptors[1];
            logging_pipes_created(from, to, file_descriptors[0], file_descriptors[0]);  // logger
            int file_descriptors1[2];
            pipe(file_descriptors1);
            processContent->read_pipes[to][from] = file_descriptors1[0];
            processContent->write_pipes[to][from] = file_descriptors1[1];
            // add logging
            logging_pipes_created(to, from, file_descriptors1[0], file_descriptors1[0]);  // logger
        }
    }
}

void close_extra_pipes(process_content* processContent , int process_count){
    for(uint8_t from = 0; from < process_count; ++from){
        for(uint8_t to = 0; to < process_count; ++to){
            if(to != processContent->this_process && from != processContent->this_process && from != to){
                close(processContent->write_pipes[from][to]);
                close(processContent->read_pipes[from][to]);
            }
            if(to == processContent->this_process && from!= processContent->this_process){
                close(processContent->write_pipes[from][to]);
            }
            if(from == processContent->this_process && to != processContent->this_process){
                close(processContent->read_pipes[from][to]);
            }
        }
    }
}
