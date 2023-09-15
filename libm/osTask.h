#pragma once

// typedef struct {
// 	uint32_t eax;
// 	uint32_t ebx;
// 	uint32_t ecx;
// 	uint32_t edx;
// 	uint32_t esi;
// 	uint32_t edi;
// 	uint32_t ebp;

// 	uint32_t intr;
// 	uint32_t error;

// 	uint32_t eip;
// 	uint32_t cs;
// 	uint32_t eflags;
// 	uint32_t esp;
// 	uint32_t ss;
// } cpu_registers_t;

#include "interrupt_frame.h"
#include <libm/list/list_basics.h>
#include <libm/queue/queue_basic_msg.h>
#include <libm/elf/elfDefines.h>

struct osTask
{
	interrupt_frame* frame;
    bool doExit;
	bool removeMe;
	bool active;
	bool isKernelModule;
	bool justYielded;

	uint64_t pid;

    uint8_t* kernelStack;
    uint8_t* userStack;

    void* pageTableContext;

	int taskTimeoutDone;

	int priority;
	int priorityStep;

	List<void*>* requestedPages;
	Queue<GenericMessagePacket*>* messages;

	Elf::LoadedElfFile elfFile;

	const char** argV;
	int argC;

	//int requestedPages;

	//bool active;

	//vmm_context_t* context;

	//void* stack;
	//void* user_stack;

	//int pid;

	//char** argv;
	//char** envp;

	//char pwd[128];

	//resource_t* resources;
	//int num_resources;

	//int wait_time;

	//bool pin;
};