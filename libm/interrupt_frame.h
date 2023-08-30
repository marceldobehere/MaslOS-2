#pragma once
#include <stdint.h>
#include <stddef.h>

typedef struct 
{
    // struct 
    // {
    //     uint64_t    cr4;
    //     uint64_t    cr3;
    //     uint64_t    cr2;
    //     uint64_t    cr0;
    // } control_registers;

    // struct 
    // {
    //     uint64_t    rdi;
    //     uint64_t    rsi;
    //     uint64_t    rdx;
    //     uint64_t    rcx;
    //     uint64_t    rbx;
    //     uint64_t    rax;
    // } general_registers;
	
    // struct 
    // {
    //     uint64_t    rbp;
    //     uint64_t    vector;
    //     uint64_t    error_code;
    //     uint64_t    rip;
    //     uint64_t    cs;
    //     uint64_t    rflags;
    //     uint64_t    rsp;
    //     uint64_t    dss;
    // } base_frame;
    //uint64_t ss;
    uint64_t cr4;
    uint64_t cr3;
    uint64_t cr2;
    uint64_t cr0;
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rax;
    uint64_t interrupt_number;
    uint64_t error_code;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} interrupt_frame;