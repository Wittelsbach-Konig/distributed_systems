//
// Created by artem on 07.12.2023.
//
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include "child_operations.h"
#include "ipc.h"
#include "pa2345.h"
#include "log.h"
#include "lamport_logical_time.h"

void send_STARTED_message(process_content* processContent) {
    Message msg;
    memset(&msg,0,sizeof(msg));
    MessageHeader msg_header;
    msg_header.s_magic = MESSAGE_MAGIC;
    msg_header.s_type = STARTED;
    msg_header.s_local_time = increase_lamport_time_and_get_it();
    int len = sprintf(msg.s_payload, log_started_fmt, get_lamport_time_value()
                        , processContent->this_process, getpid(), getppid(), *processContent->process_balance);
    msg_header.s_payload_len = len + 1;
    msg.s_header = msg_header;
    if(send_multicast(processContent, &msg) != 0) { // add logging
        printf("error send started message");
    }
    logging_process_started(get_lamport_time_value()
                            //get_lamport_time(processContent)
                            , processContent->this_process
                            , *processContent->process_balance);  // logger
}

void save_balance_state(process_content* processContent, timestamp_t operation_time, balance_t pending_balance_in){
    uint8_t history_len = processContent->balanceHistory->s_history_len;
    if( history_len > 0){
        BalanceState last_known_state = processContent->balanceHistory->s_history[history_len - 1];
        if(last_known_state.s_time == operation_time){
            return;
        }
        for(timestamp_t time = last_known_state.s_time + 1; time < operation_time; ++time){
            BalanceState intermediate_state;
            intermediate_state.s_balance = last_known_state.s_balance;
            intermediate_state.s_balance_pending_in = 0;
            intermediate_state.s_time = time;
            processContent->balanceHistory->s_history[processContent->balanceHistory->s_history_len] = intermediate_state;
            ++processContent->balanceHistory->s_history_len;
        }
    }
    BalanceState updated_state;
    updated_state.s_balance = *processContent->process_balance;
    updated_state.s_balance_pending_in = pending_balance_in;
    updated_state.s_time = operation_time;
    processContent->balanceHistory->s_history[processContent->balanceHistory->s_history_len] = updated_state;
    ++processContent->balanceHistory->s_history_len;
}

void send_DONE_message(process_content* processContent) {
    Message msg;
    memset(&msg,0,sizeof(msg));
    MessageHeader msg_header;
    msg_header.s_magic = MESSAGE_MAGIC;
    msg_header.s_type = DONE;
    msg_header.s_local_time = increase_lamport_time_and_get_it();
    int len = sprintf(msg.s_payload, log_done_fmt, get_lamport_time_value(),
                      processContent->this_process, *processContent->process_balance);
    msg_header.s_payload_len = len + 1;
    msg.s_header = msg_header;
    if(send_multicast(processContent, &msg) != 0) { // add logging
        printf("error send done message");
    }
    logging_process_done(get_lamport_time_value()
                        , processContent->this_process
                        , *processContent->process_balance);  // logger
}

void process_transfer_queries(process_content* processContent){
    int permission_to_work = 1;
    int num_DONE_process = 0;
    int num_C_processes = processContent->process_num - 2;
    while(permission_to_work || num_DONE_process < num_C_processes){
        Message msg;
        receive_any(processContent, &msg);
        get_lamport_time_from_message(msg.s_header.s_local_time);
        switch (msg.s_header.s_type) {
            case STOP:
            {
                permission_to_work = 0;
                send_DONE_message(processContent);
                break;
            }
            case DONE:
            {
                num_DONE_process++;
                break;
            }
            case TRANSFER:
            {
                TransferOrder recieved_order;
                memcpy(&recieved_order, msg.s_payload, msg.s_header.s_payload_len);
                if(processContent->this_process == recieved_order.s_src) {
                    *processContent->process_balance -= recieved_order.s_amount;
                    save_balance_state(processContent, increase_lamport_time_and_get_it(), recieved_order.s_amount);
                    msg.s_header.s_local_time = get_lamport_time_value();
                    if(send(processContent, recieved_order.s_dst, &msg) != 0){
                        printf("TS could not send transfer from src %d to dst %d", processContent->this_process, recieved_order.s_dst);
                    }
                    logging_transfer_out(get_lamport_time_value(), processContent->this_process,
                                         recieved_order.s_amount, recieved_order.s_dst);
                }
                if(processContent->this_process == recieved_order.s_dst) {
                    *processContent->process_balance += recieved_order.s_amount;
                    save_balance_state(processContent, get_lamport_time_value(), 0);
                    Message ack_message;
                    MessageHeader messageHeader;
                    messageHeader.s_magic = MESSAGE_MAGIC;
                    messageHeader.s_type = ACK;
                    messageHeader.s_payload_len = 0;
                    messageHeader.s_local_time = increase_lamport_time_and_get_it();
                    ack_message.s_header = messageHeader;
                    send(processContent, PARENT_ID, &ack_message);
                    logging_transfer_in(get_lamport_time_value(), processContent->this_process,
                                        recieved_order.s_amount, recieved_order.s_src);
                }
            }
            default:
                break;
        }

    }
    logging_received_all_done_messages(get_lamport_time_value(), processContent->this_process);
}

void send_balance_history_to_parent(process_content* processContent) {
    unsigned long message_length =
            processContent->balanceHistory->s_history_len * sizeof(*processContent->balanceHistory->s_history);
    Message msg;
    MessageHeader msg_header;
    msg_header.s_type = BALANCE_HISTORY;
    msg_header.s_magic = MESSAGE_MAGIC;
    msg_header.s_local_time = increase_lamport_time_and_get_it();
    msg_header.s_payload_len = message_length;
    msg.s_header = msg_header;
    memcpy(msg.s_payload, processContent->balanceHistory->s_history, message_length);
    if(send(processContent, PARENT_ID, &msg)!=0){
        printf("Could not transfer history from process %d", processContent->this_process);
    }
}
