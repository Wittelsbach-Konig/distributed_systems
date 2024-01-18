//
// Created by artem on 10.12.2023.
//

#ifndef LAB2_LAMPORT_LOGICAL_TIME_H
#define LAB2_LAMPORT_LOGICAL_TIME_H

#include "banking.h"
#include "constants.h"

timestamp_t get_lamport_time_value();

timestamp_t increase_lamport_time_and_get_it();

timestamp_t get_lamport_time_from_message(timestamp_t received_time);

#endif //LAB2_LAMPORT_LOGICAL_TIME_H
