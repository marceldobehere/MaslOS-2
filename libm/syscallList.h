#pragma once

#define SYSCALL_GET_ARGC 100
#define SYSCALL_GET_ARGV 101
#define SYSCALL_GET_ENV 102
#define SYSCALL_GET_PID 103
#define SYSCALL_PID_EXISTS 104
#define SYSCALL_REQUEST_NEXT_PAGES 110

#define SYSCALL_EXIT  130
#define SYSCALL_CRASH  131
#define SYSCALL_YIELD  132
#define SYSCALL_WAIT  133
#define SYSCALL_SET_PRIORITY 134

#define SYSCALL_LAUNCH_TEST_ELF_USER 178
#define SYSCALL_LAUNCH_TEST_ELF_KERNEL 179

#define SYSCALL_SERIAL_PRINT  200
#define SYSCALL_SERIAL_PRINTLN 201

#define SYSCALL_SERIAL_PRINT_CHAR 202
#define SYSCALL_SERIAL_READ_CHAR 203
#define SYSCALL_SERIAL_CAN_READ_CHAR 204


#define SYSCALL_GLOBAL_PRINT  301
#define SYSCALL_GLOBAL_PRINTLN 302
#define SYSCALL_GLOBAL_PRINT_CHAR  303
#define SYSCALL_GLOBAL_CLS  310

#define SYSCALL_ENV_GET_TIME_MS 400
#define SYSCALL_ENV_GET_DESKTOP_PID 401

#define SYSCALL_ENV_GET_MOUSE_STATE 410
#define SYSCALL_ENV_GET_KEY_STATE 411

#define SYSCALL_MSG_GET_COUNT 510
#define SYSCALL_MSG_GET_MSG 511
#define SYSCALL_MSG_SEND_MSG 512
#define SYSCALL_MSG_GET_MSG_CONVO 513

#define SYSCALL_RNG_UINT64 600
