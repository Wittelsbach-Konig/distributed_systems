//
// Created by artem on 07.12.2023.
//
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <assert.h>

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
            , processContent->this_process, getpid(), getppid(), 0);
    msg_header.s_payload_len = len + 1;
    msg.s_header = msg_header;
    if(send_multicast(processContent, &msg) != 0) { // add logging
        printf("error send started message");
    }
    logging_process_started(get_lamport_time_value()
            , processContent->this_process);  // logger
}

void send_DONE_message(process_content* processContent) {
    Message msg;
    memset(&msg,0,sizeof(msg));
    MessageHeader msg_header;
    msg_header.s_magic = MESSAGE_MAGIC;
    msg_header.s_type = DONE;
    msg_header.s_local_time = increase_lamport_time_and_get_it();
    int len = sprintf(msg.s_payload, log_done_fmt, get_lamport_time_value(),
                      processContent->this_process, 0);
    msg_header.s_payload_len = len + 1;
    msg.s_header = msg_header;
    if(send_multicast(processContent, &msg) != 0) { // add logging
        printf("error send done message");
    }
    logging_process_done(get_lamport_time_value()
            , processContent->this_process);  // logger
}

/** Передача маркера запроса
 * Процесс отправляет соседнему процессу маркер запроса, если данный процесс:
 * 1. Владеет маркером;
 * 2. Не владеет вилкой;
 * 3. Процесс хочет зайти в critical section.
 */
int fork_can_be_requested(const Fork fork) {
    return fork.ownership == FORK_FREE && fork.request_token == FORK_HAVE_TOKEN;
}

static int should_send_request(process_content *content) {
    for (local_id i = 1; i < content->forks_num; ++i) {
        if (i == content->this_process)
            continue;
        if (fork_can_be_requested(content->forks[i]))
            return 1;
    }
    return 0;
}

/** Переход из ожидания в critical section (CS)
 * Ожидающий процесс может войти в CS только тогда, когда он владеет всеми вилками своих соседей
 * и для любого его соседа верно, что общая вилка чистая либо у неё нет маркера запроса от соседа.
 * Когда процесс переходит в CS, все его вилки становятся грязными.
 */
static int can_enter_cs(process_content *content) {
    for (local_id i = 1; i < content->forks_num; ++i) {
        if (i == content->this_process)
            continue;
        Fork current_fork = content->forks[i];
        int is_owns_fork = current_fork.ownership == FORK_OWNS;
        int clean_or_no_token = current_fork.purity == FORK_CLEAN || current_fork.request_token == FORK_MISSING_TOKEN;
        if (is_owns_fork && clean_or_no_token)
            continue;
        return 0;
    }
    return 1;
}

/** Передача вилки
 * Процесс пересылает вилку, если данный процесс:
 * 1. Имеет вместе с этой вилкой соответствующий маркер запроса;
 * 2. Вилка грязная;
 * 3. Процессу не нужно входить в critical section.
 * При передаче вилка очищается.
 */
int fork_can_be_released(const Fork fork) {
    return fork.ownership == FORK_OWNS && fork.purity == FORK_DIRTY && fork.request_token == FORK_HAVE_TOKEN;
}

static int should_release_fork(process_content *content) {
    for (local_id i = 1; i < content->forks_num; ++i) {
        if (i == content->this_process)
            continue;
        if (fork_can_be_released(content->forks[i]))
            return 1;
    }
    return 0;
}

static void mark_forks_as_dirty(process_content *content) {
    for (local_id i = 1; i < content->forks_num; ++i) {
        if (i == content->this_process)
            continue;
        content->forks[i].purity = FORK_DIRTY;
    }
}

void handle_requests(process_content *content) {
    const int CHILDREN_COUNT = content->process_num - 2; // minus parent and current process
    const int MAX_LOOP_COUNT = content->print_iterations;
    int stop_signal_received = 0;
    int done_messages_count = 0;
    int loop_iteration = 1;
    int done_sent = 0;

    while (1) {
        if (stop_signal_received && done_messages_count == CHILDREN_COUNT) {
            logging_received_all_done_messages(get_lamport_time_value(), content->this_process);
            return;
        }

        if (done_sent == 0 && loop_iteration > MAX_LOOP_COUNT) {
            send_DONE_message(content);
            logging_process_done(get_lamport_time_value(), content->this_process);
            done_sent = 1;
            continue;
        }

        if (loop_iteration <= MAX_LOOP_COUNT && should_send_request(content)) {
            request_cs(content);
            continue;
        }

        if (should_release_fork(content)) {
            release_cs(content);
            continue;
        }

        Message incoming_message;
        int receive_any_status = receive_any(content, &incoming_message);
        printf("Receive message %d\n", receive_any_status);
        if (receive_any_status == -1) {
            //  means no messages
            if (loop_iteration <= MAX_LOOP_COUNT && can_enter_cs(content)) {
                char loop_operation[70] = {0};
                sprintf(loop_operation, log_loop_operation_fmt, content->this_process, loop_iteration++, MAX_LOOP_COUNT);
                print(loop_operation);
                mark_forks_as_dirty(content);
            }
            continue;
        }

        assert(receive_any_status == 0);
        assert(incoming_message.s_header.s_magic == MESSAGE_MAGIC);
        get_lamport_time_from_message(incoming_message.s_header.s_local_time);

        switch (incoming_message.s_header.s_type) {
            case STOP: {
                stop_signal_received++;
                assert(incoming_message.s_header.s_payload_len == 0);
                assert(stop_signal_received == 1); // check stop signal received only once
                break;
            }
            case DONE: {
                done_messages_count++;
                assert(incoming_message.s_header.s_payload_len == strlen(incoming_message.s_payload) + 1);
                assert(done_messages_count <= CHILDREN_COUNT);
                break;
            }
            case CS_REPLY: {
                assert(incoming_message.s_header.s_payload_len == 0);
                break;
            }
            case CS_REQUEST: {
                Request request;
                assert(incoming_message.s_header.s_payload_len == sizeof(request));
                memcpy(&request, incoming_message.s_payload, incoming_message.s_header.s_payload_len);
                content->forks[request.request_from].request_token = FORK_HAVE_TOKEN;
                break;
            }
            case CS_RELEASE: {
                Request request;
                assert(incoming_message.s_header.s_payload_len == sizeof(request));
                memcpy(&request, incoming_message.s_payload, incoming_message.s_header.s_payload_len);
                content->forks[request.request_from].ownership = FORK_OWNS;
                content->forks[request.request_from].purity = FORK_CLEAN;
                break;
            }
            default: {
                assert(0); // unreachable code
                break;
            }
        }
    }
}

void handle_requests_without_cs(process_content *content) {
    const int CHILDREN_COUNT = content->process_num - 2; // minus parent and current process
    const int MAX_LOOP_COUNT = content->print_iterations;
    int stop_signal_received = 0;
    int done_messages_count = 0;

    for (int i = 1; i <= MAX_LOOP_COUNT; ++i) {
        char loop_operation[70] = {0};
        sprintf(loop_operation, log_loop_operation_fmt, content->this_process, i, MAX_LOOP_COUNT);
        print(loop_operation);
    }
    send_DONE_message(content);
    logging_process_done(get_lamport_time_value(), content->this_process);

    while (1) {
        if (stop_signal_received && done_messages_count == CHILDREN_COUNT) {
            logging_received_all_done_messages(get_lamport_time_value(), content->this_process);
            return;
        }

        Message incoming_message;
        assert(receive_any(content, &incoming_message) == 0);
        assert(incoming_message.s_header.s_magic == MESSAGE_MAGIC);
        get_lamport_time_from_message(incoming_message.s_header.s_local_time);

        switch (incoming_message.s_header.s_type) {
            case DONE: {
                done_messages_count++;
                assert(incoming_message.s_header.s_payload_len == strlen(incoming_message.s_payload) + 1);
                assert(done_messages_count <= CHILDREN_COUNT);
                break;
            }
            case STOP: {
                stop_signal_received++;
                assert(incoming_message.s_header.s_payload_len == 0);
                assert(stop_signal_received == 1); // check stop signal received only once
                break;
            }
            default: {
                assert(0); // unreachable code
                break;
            }
        }
    }
}

void forks_array_init(Fork *forks, local_id forks_num, local_id child_id) {
    for (local_id i = 1; i < forks_num; ++i) {
        if (i == child_id)
            continue;
        if (i < child_id) {
            Fork current_fork;
            current_fork.ownership = FORK_FREE;
            current_fork.purity = FORK_CLEAN;
            current_fork.request_token = FORK_HAVE_TOKEN;
            forks[i] = current_fork;
            continue;
        }
        Fork current_fork;
        current_fork.ownership = FORK_OWNS;
        current_fork.purity = FORK_CLEAN;
        current_fork.request_token = FORK_MISSING_TOKEN;
        forks[i] = current_fork;
    }
}
