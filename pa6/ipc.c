//
// Created by artem on 05.12.2023.
//
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#include "ipc.h"
#include "constants.h"

int send(void *self, local_id dst, const Message *msg) {
    process_content *processContent = (process_content *) self;
    int write_pipe = processContent->write_pipes[processContent->this_process][dst];
    int written = write(write_pipe, msg, sizeof(MessageHeader) + msg->s_header.s_payload_len);
    return (written < 0) ? SEND_UNSUCCESFUL : 0;
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

int blocking_payload_read(int read_fd, Message *msg) {
    const int payload_len = msg->s_header.s_payload_len;
    if (payload_len == 0)
        return 0;

    while (1) {
        const int count = read(read_fd, msg->s_payload, payload_len);
        if (count < 0 && errno == EAGAIN) {
            continue; // TODO sleep?
        }
        return (count == payload_len) ? 0 : RECIEVE_UNSUCCESFUL;
    }
}

int asynchronous_header_read(int read_fd, Message *msg) {
    const int header_size = sizeof(msg->s_header);
    const int count = read(read_fd, &msg->s_header, header_size);

    if ((count < 0 && errno == EAGAIN) || count == 0)
        return NO_DATA_RECIEVED;

    return (count == header_size) ? 0 : RECIEVE_UNSUCCESFUL;
}

int receive(void *self, local_id from, Message *msg) {
    process_content* processContent = (process_content*) self;
    const int read_fd = processContent->read_pipes[from][processContent->this_process];
    while (1) {
        int status = asynchronous_header_read(read_fd, msg);
        if (status == NO_DATA_RECIEVED)
            continue;
        if (status == 0)
            break;
        if (status == RECIEVE_UNSUCCESFUL)
            return RECIEVE_UNSUCCESFUL;
    }
    if (msg->s_header.s_magic != MESSAGE_MAGIC)
        return RECIEVE_UNSUCCESFUL;
    return blocking_payload_read(read_fd, msg);
}

int receive_any(void *self, Message *msg) {
    process_content* processContent = (process_content*) self;
    for (local_id from = 0; from < processContent->process_num; from++) {
        if (from == processContent->this_process) {
            continue;
        }
        const int read_fd = processContent->read_pipes[from][processContent->this_process];
        int status = asynchronous_header_read(read_fd, msg);
        assert(status == 0);
        if (status == NO_DATA_RECIEVED)
            continue;
        if (status == RECIEVE_UNSUCCESFUL)
            return RECIEVE_UNSUCCESFUL;


        if (msg->s_header.s_magic != MESSAGE_MAGIC)
            return RECIEVE_UNSUCCESFUL;

        return blocking_payload_read(read_fd, msg);
    }
    return -1; // when no messages
}
