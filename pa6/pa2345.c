#include <assert.h>
#include <string.h>

#include "general_actions.h"
#include "child_operations.h"
#include "lamport_logical_time.h"

int request_cs(const void * self){
    process_content *content = (process_content *) self;
    timestamp_t lamp_time = increase_lamport_time_and_get_it();
    Request request;
    request.request_from = content->this_process;
    Message request_msg;
    MessageHeader request_msg_header;
    request_msg_header.s_magic = MESSAGE_MAGIC;
    request_msg_header.s_type = CS_REQUEST;
    request_msg_header.s_local_time = lamp_time;
    request_msg_header.s_payload_len = sizeof(request);
    request_msg.s_header = request_msg_header;
    memcpy(request_msg.s_payload, &request, sizeof(request));
    
    for (local_id i = 1; i < content->process_num; ++i) {
        if (i == content->this_process)
            continue;
        Fork *current_fork = &content->forks[i];
        if (fork_can_be_requested(*current_fork)) {
            current_fork->request_token = FORK_MISSING_TOKEN;
            assert(send(content, i, &request_msg) == 0);
        }
    }
    return 0;
}

int release_cs(const void* self){
    process_content *content = (process_content *) self;
    Request request;
    request.request_from = content->this_process;
    Message release_msg;
    MessageHeader release_msg_header;
    release_msg_header.s_local_time = increase_lamport_time_and_get_it();
    release_msg_header.s_magic = MESSAGE_MAGIC;
    release_msg_header.s_type = CS_RELEASE;
    release_msg_header.s_payload_len = sizeof(request);
    release_msg.s_header = release_msg_header;
    memcpy(release_msg.s_payload, &request, sizeof(request));
    for (local_id i = 1; i < content->process_num; ++i) {
        if (i == content->this_process)
            continue;
        Fork *current_fork = &content->forks[i];
        if (fork_can_be_released(*current_fork)) {
            current_fork->ownership = FORK_FREE;
            assert(send(content, i, &release_msg) == 0);
        }
    }
    return 0;
}
