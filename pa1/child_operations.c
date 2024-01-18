//
// Created by artem on 07.12.2023.
//
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include "child_operations.h"
#include "ipc.h"
#include "pa1.h"
#include "general_actions.h"
#include "log.h"

void send_recieve_STARTED_message(process_content* processContent) {
    Message msg;
    memset(&msg,0,sizeof(msg));
    MessageHeader msg_header;
    msg_header.s_magic = MESSAGE_MAGIC;
    msg_header.s_type = STARTED;
    msg_header.s_local_time = 0;
    int len = sprintf(msg.s_payload, log_started_fmt, processContent->this_process, getpid(), getppid());
    msg_header.s_payload_len = len + 1;
    msg.s_header = msg_header;
    if(send_multicast(processContent, &msg) != 0){ // add logging
        printf("error send started message");
    }
    logging_process_started(processContent->this_process);  // logger
    recieve_messages_from_other_processes(processContent, STARTED);
    logging_received_all_started_messages(processContent->this_process);  // logger
}

void send_recieve_DONE_message(process_content* processContent) {
    Message msg;
    memset(&msg,0,sizeof(msg));
    MessageHeader msg_header;
    msg_header.s_magic = MESSAGE_MAGIC;
    msg_header.s_type = DONE;
    msg_header.s_local_time = 0;
    int len = sprintf(msg.s_payload, log_done_fmt, processContent->this_process);
    msg_header.s_payload_len = len + 1;
    msg.s_header = msg_header;
    if(send_multicast(processContent, &msg) != 0){ // add logging
        printf("error send done message");
    }
    logging_process_done(processContent->this_process);  // logger
    recieve_messages_from_other_processes(processContent, DONE);
    logging_received_all_done_messages(processContent->this_process);  // logger
}
