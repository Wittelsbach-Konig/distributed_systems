//
// Created by artem on 07.12.2023.
//

#include "general_actions.h"
#include "ipc.h"
#include <stdio.h>

void recieve_messages_from_other_processes(process_content* processContent, MessageType type){
    for(uint8_t process_id = 1; process_id < processContent->process_num; ++process_id){
        if(process_id == processContent->this_process){
            continue;
        }
        Message recieved_msg;
        if(receive(processContent, process_id, &recieved_msg) == 0){
            if(recieved_msg.s_header.s_magic != MESSAGE_MAGIC){
                //return NOT_MAGIC_SIGN;
            }
            if(recieved_msg.s_header.s_type != type){
                //return NOT_VALID_TYPE;
            }
        }
        else{
            //return RECIEVE_UNSUCCESFUL;
        }
    }
}
