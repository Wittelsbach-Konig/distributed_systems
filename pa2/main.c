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
#include "banking.h"
#include "lamport_logical_time.h"
#include "general_actions.h"

process_content processContent;

void parse_process_balances(char *argv[], int proc_count, balance_t* process_balances);

int main(int argc, char *argv[]) {
    uint8_t process_count;
    balance_t process_balances[MAX_PROCESS_NUM];
    if (argc >= 3 && strcmp(argv[1], "-p") == 0) {
        process_count = (uint8_t)strtol(argv[2], NULL, 10);
        parse_process_balances(argv, process_count, &process_balances[0]);
    } else{
        return 1;
    }
    logging_preparation();  // logger
    process_count = process_count + 1;
    set_pipe_descriptors(&processContent, process_count);
    modify_pipe_set_non_blocking(&processContent, process_count);
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
            balance_t proc_bal = process_balances[id - 1];
            processContent.process_balance = &proc_bal;
            BalanceHistory process_balance_history;
            process_balance_history.s_history_len = 0;
            process_balance_history.s_id = id;
            processContent.balanceHistory = &process_balance_history;
            close_extra_pipes(&processContent, process_count);
            save_balance_state(&processContent, get_lamport_time_value(), 0);
            send_STARTED_message(&processContent);
            recieve_messages_from_other_processes(&processContent, STARTED);
            process_transfer_queries(&processContent);
            save_balance_state(&processContent, get_lamport_time_value(), 0);
            send_balance_history_to_parent(&processContent);
            exit(0);
        }
        if(child_process > 0){
            processContent.this_process = PARENT_ID;
        }
    }
    close_extra_pipes(&processContent, process_count);
    balance_t parent_bal = -1;
    processContent.process_balance = &parent_bal;
    recieve_child_messages(&processContent, STARTED);
    bank_robbery(&processContent, process_count-1);
    send_stop_messages(&processContent);
    recieve_child_messages(&processContent, DONE);
    recieve_BalanceHistory_messages(&processContent);
    wait_for_childs();
    logging_finalize();  // logger
    return 0;
}

void parse_process_balances(char *argv[], int proc_count, balance_t * process_balances) {
    const int position = 3;
    int k = 0;
    const int end_position = position + proc_count;
    for(int pos = position; pos < end_position; ++pos, ++k){
        process_balances[k] = (balance_t )strtol(argv[pos], NULL, 10);
    }
}
