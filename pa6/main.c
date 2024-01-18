#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "ipc.h"
#include "constants.h"
#include "pipe_operations.h"
#include "child_operations.h"
#include "parent_operations.h"
#include "log.h"
#include "lamport_logical_time.h"
#include "general_actions.h"

process_content processContent = {
        .this_process = -1,
        .process_num = -1,
        .forks_num = -1,
        .forks = {{0}},
        .using_mutex = -1
    };

int parse_mutexl_parameter(int argc, char **argv);
int parse_process_count_parameter(int argc, char **argv);

int main(int argc, char *argv[]) {
    uint8_t process_count = parse_process_count_parameter(argc, argv);
    if(process_count == 0){
        printf("Unable to parse cli parameters, try again. Example: -p X [--mutexl]\n");
        return 1;
    }
    uint8_t using_mutex = (uint8_t) parse_mutexl_parameter(argc, argv);
    processContent.using_mutex = using_mutex;
    logging_preparation();  // logger
    process_count = process_count + 1;

    set_pipe_descriptors(&processContent, process_count);
    modify_pipe_set_non_blocking(&processContent, process_count);
    processContent.process_num = process_count;
    for(uint8_t id = 1; id < process_count; ++id ){
        int child_process = fork();
        if(child_process == -1){
            perror("child process does not created"); // error
            exit(EXIT_FAILURE);
        }
        if(child_process == 0){
            processContent.this_process = id;
            processContent.print_iterations = id * 5;
            processContent.forks_num = process_count;
            forks_array_init(processContent.forks, process_count, processContent.this_process);
            close_extra_pipes(&processContent, process_count);
            send_STARTED_message(&processContent);
            recieve_messages_from_other_processes(&processContent, STARTED);
            if (using_mutex) {
                printf("Using mutex\n");
                handle_requests(&processContent);
            } else {
                printf("Not using mutex\n");
                handle_requests_without_cs(&processContent);
            }

            exit(0);
        }
        if(child_process > 0){
            processContent.this_process = PARENT_ID;
        }
    }
    close_extra_pipes(&processContent, process_count);
    recieve_child_messages(&processContent, STARTED);
    send_stop_messages(&processContent);
    recieve_child_messages(&processContent, DONE);
    wait_for_childs();
    logging_finalize();  // logger
    return 0;
}

int parse_process_count_parameter (int argc, char **argv) {
    int flag = -1;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-p") == 0) {
            flag = i;
            break;
        }
    }
    if(flag == -1){
        printf("Error: Please, provide \"-p\" flag to specify X - number of child processes.\n");
        return 0;
    }
    const int process_arg_index = flag + 1;
    if (argc <= process_arg_index) {
        printf("Error: After \"-p\" flag specify integer X - number of child processes.\n");
    }
    int X = strtol(argv[process_arg_index], NULL, 10);
    return X;
}

int parse_mutexl_parameter(int argc,char **argv){
    for(int i=1; i < argc; ++i){
        if(strcmp(argv[i], "--mutexl") == 0){
            return 1;
        }
    }
    return 0;
}
