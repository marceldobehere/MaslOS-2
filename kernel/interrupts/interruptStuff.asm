[extern intr_common_handler_c]
[extern CloseCurrentTask]

%macro intr_stub 1
	GLOBAL intr_stub_%1
intr_stub_%1:
	push 0
	push %1
	jmp intr_common_handler
%endmacro

%macro intr_stub_err 1
	GLOBAL intr_stub_%1
intr_stub_%1:
	push %1
	jmp intr_common_handler
%endmacro

%macro pusha 0
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popa 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax
%endmacro

intr_stub 0
intr_stub 1
intr_stub 2
intr_stub 3
intr_stub 4
intr_stub 5
intr_stub 6
intr_stub 7
intr_stub_err 8
intr_stub 9
intr_stub_err 10
intr_stub_err 11
intr_stub_err 12
intr_stub_err 13
intr_stub_err 14
intr_stub 15
intr_stub 16
intr_stub_err 17
intr_stub 18

intr_stub 19
intr_stub 20
intr_stub_err 21

intr_stub 28
intr_stub_err 29
intr_stub_err 30

intr_stub 32
intr_stub 33

intr_stub 34
intr_stub 35
intr_stub 36
intr_stub 37
intr_stub 38
intr_stub 39
intr_stub 40
intr_stub 41
intr_stub 42
intr_stub 43
intr_stub 44
intr_stub 45
intr_stub 46
intr_stub 47

intr_stub 49

intr_stub 254; generic

intr_stub 255

;# intr_common_handler-signature: void intr_common_handler()
;# intr_common_handler-doc: The common interrupt handler. Every interrupt except for the syscall lands here first. Calls the init_comon_handler_c function.

intr_common_handler:
	; cli
	pusha

	mov rax, cr0
	push rax
	mov rax, cr2
	push rax
	mov rax, cr3
	push rax
	mov rax, cr4
	push rax


	mov rdi, rsp

	call intr_common_handler_c

	pop rax
	pop rax
	pop rax
	pop rax
	popa

	add rsp, 16

	; sti
	iretq




; NOTE: the addres to the target task needs to be in rax
; we do this to prevent jumping to random code if the task returns

;#task_entry-discard
[global task_entry]
task_entry:
	sti

	mov rbp, 0 ; mark bottom of stack trace
	
	call rax

.exit:
	call CloseCurrentTask ; if the task ever returns just exit
	
	jmp $