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

process_content processContent;

int main(int argc, char *argv[]) {

    uint8_t process_count;
    if (argc == 3 && strcmp(argv[1], "-p") == 0) {
        process_count = (uint8_t)strtol(argv[2], NULL, 10);
    } else{
        return 1;
    }
    logging_preparation();  // logger
    process_count = process_count + 1;
    set_pipe_descriptors(&processContent, process_count);
    processContent.process_num = process_count;
    processContent.this_process = PARENT_ID;
    for(uint8_t id = 1; id < process_count; ++id ){
        int child_process = fork();
        if(child_process == -1){
            perror("child process does not created"); // error
            exit(EXIT_FAILURE);
        }
        if(child_process == 0){
            processContent.this_process = id;
            close_extra_pipes(&processContent, process_count);
            send_recieve_STARTED_message(&processContent);
            send_recieve_DONE_message(&processContent);
            exit(0);
        }
        if(child_process > 0){
            processContent.this_process = PARENT_ID;
        }
    }
    close_extra_pipes(&processContent, process_count);
    printf("In parent section\n");
    recieve_child_messages(&processContent);
    wait_for_childs();
    logging_finalize();  // logger
    return 0;
}
