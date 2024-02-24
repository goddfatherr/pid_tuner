#ifndef _PID_TUNER_H_
#define _PID_TUNER_H_

#include <stdint.h>
#include <esp_system.h>

extern int32_t P, I, D; 

esp_err_t load_pid_values();

void stop_local_pid_tuner();
void start_local_pid_tuner();

void stop_socket_pid_tuner();
void start_socket_pid_tuner();

#endif