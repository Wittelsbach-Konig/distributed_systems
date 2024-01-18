//
// Created by artem on 10.12.2023.
//

#include "lamport_logical_time.h"

static timestamp_t lamport_time = 0;

timestamp_t get_lamport_time_value() {
    return lamport_time;
}

timestamp_t increase_lamport_time_and_get_it() {
    lamport_time++;
    return get_lamport_time_value();
}

timestamp_t get_lamport_time_from_message(timestamp_t received_time) {
    lamport_time = (lamport_time > received_time) ? lamport_time : received_time;
    return increase_lamport_time_and_get_it();
}
