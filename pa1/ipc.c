//
// Created by artem on 05.12.2023.
//
#include <unistd.h>
#include "ipc.h"
#include "constants.h"

int send(void * self, local_id dst, const Message * msg){
    process_content* processContent = (process_content*) self;
    int bytes_written = write(processContent->write_pipes[processContent->this_process][dst], &msg->s_header,
                sizeof(MessageHeader));
    if(bytes_written < 0){
        return SEND_UNSUCCESFUL;
    }
    bytes_written = write(processContent->write_pipes[processContent->this_process][dst], &msg->s_payload,
            msg->s_header.s_payload_len);
    return (bytes_written >= 0) ? 0 : SEND_UNSUCCESFUL;
}

int send_multicast(void * self, const Message * msg){
    process_content* processContent = (process_content*) self;
    for(local_id process_id = 0; process_id != processContent->process_num; ++process_id){
        if(process_id == processContent->this_process){
            continue;
        }
        if(send(self, process_id, msg)){
            return MULTICAST_SEND_UNSUCCESFUL;
        }
    }
    return 0;
}

int receive(void * self, local_id from, Message * msg){
    process_content* processContent = (process_content*) self;
    int bytes_read = read(processContent->read_pipes[from][processContent->this_process],
                            &msg->s_header,
                            sizeof(msg->s_header));
    if(bytes_read < 0) {
        return RECIEVE_UNSUCCESFUL;
    }
    if(msg->s_header.s_magic != MESSAGE_MAGIC) {
        return NOT_MAGIC_SIGN;
    }
    bytes_read = read(processContent->read_pipes[from][processContent->this_process],
                        &msg->s_payload,
                        msg->s_header.s_payload_len);
    if(bytes_read < 0){
        return RECIEVE_UNSUCCESFUL;
    }
    return 0;
}

int receive_any(void * self, Message * msg){
    return 0;
}
