//
// Created by artem on 11.12.2023.
//
#include "banking.h"
#include "constants.h"
#include "lamport_logical_time.h"
#include "log.h"
#include <string.h>

void transfer(void *parent_data, local_id src, local_id dst, balance_t amount) {
    TransferOrder order = {.s_src = src, .s_dst = dst, .s_amount = amount};
    Message transfer_message = {.s_header = {
            .s_magic = MESSAGE_MAGIC,
            .s_payload_len = sizeof(order),
            .s_type = TRANSFER,
            .s_local_time = increase_lamport_time_and_get_it()
    }};
    memcpy(transfer_message.s_payload, &order, sizeof(order));
    if(send(parent_data, src, &transfer_message)!=0){
        printf("transfer from src %d to dst %d unsuccessful", src, dst);
    }
    Message ack_message;
    receive(parent_data, dst, &ack_message);
    get_lamport_time_from_message(ack_message.s_header.s_local_time);
}
