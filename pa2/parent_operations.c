//
// Created by artem on 07.12.2023.
//

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <memory.h>
#include "parent_operations.h"
#include "general_actions.h"
#include "log.h"
#include "lamport_logical_time.h"

void recieve_child_messages(process_content* processContent, MessageType type){
    recieve_messages_from_other_processes(processContent, type);
}

void send_stop_messages(process_content* processContent){
    Message msg;
    memset(&msg,0, sizeof(msg));
    MessageHeader msg_header;
    msg_header.s_magic = MESSAGE_MAGIC;
    msg_header.s_type = STOP;
    msg_header.s_local_time = increase_lamport_time_and_get_it();
    msg.s_header = msg_header;
    if(send_multicast(processContent, &msg) != 0){ // add logging
        printf("error send stop message\n");
    }
    // добавить логирование об отправке stop сообщений
}

void recieve_BalanceHistory_messages(process_content* processContent){
    AllHistory all_history;
    all_history.s_history_len = processContent->process_num - 1;
    for(uint8_t id = 1; id < processContent->process_num; ++id){
        Message msg;
        receive(processContent, id, &msg);
        unsigned long history_length = msg.s_header.s_payload_len / sizeof(*all_history.s_history->s_history);
        BalanceHistory balance_history;
        balance_history.s_id = id;
        balance_history.s_history_len = history_length;
        memcpy(balance_history.s_history, msg.s_payload, msg.s_header.s_payload_len);
        all_history.s_history[id - 1] = balance_history;
    }
    print_history(&all_history);
}

void wait_for_childs() {
    pid_t child_pid = 0;
    int status = 0;
    while ((child_pid = wait(&status)) > 0) {
        printf("child process %d finished with %d.\n", child_pid, status);
    }
}

