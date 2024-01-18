//
// Created by artem on 07.12.2023.
//

#include "general_actions.h"
#include "lamport_logical_time.h"
#include "ipc.h"
#include "log.h"
#include <stdio.h>

// for started and done messages
void recieve_messages_from_other_processes(process_content* processContent, MessageType type){
    for(uint8_t process_id = 1; process_id < processContent->process_num; ++process_id){
        if(process_id == processContent->this_process){
            continue;
        }
        Message recieved_msg;
        if(receive(processContent, process_id, &recieved_msg)!=0){
            printf("MESSAGE NOT RECIEVED!!!\n");
        }
        get_lamport_time_from_message(recieved_msg.s_header.s_local_time);
    }
    if(type == STARTED){
        logging_received_all_started_messages(get_lamport_time_value(), processContent->this_process);
    }
    else if (type == DONE){
        logging_received_all_done_messages(get_lamport_time_value(), processContent->this_process);
    }
}
