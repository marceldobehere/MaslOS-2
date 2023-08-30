SECTION	.text

[bits 64]
LoadGDT:
    lgdt[rdi]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    pop rdi
    mov rax, 0x08
    push rax
    push rdi
    retfq

GLOBAL LoadGDT


GLOBAL FlushTSS
FlushTSS:
	mov ax, (5 * 8) | 0 ; fifth 8-byte selector, symbolically OR-ed with 0 to set the RPL (requested privilege level).
	ltr ax
	ret




%define GDT_OFFSET_KERNEL_CODE	0x08
%define GDT_OFFSET_KERNEL_DATA	0x10
%define GDT_OFFSET_USER_DATA	0x20
%define GDT_OFFSET_USER_CODE	0x28
%define GDT_OFFSET_TSS			0x30


GLOBAL	gdt_load
gdt_load:	; rdi=[gdt_descriptor]
	; Setup GDT
	cli
	lgdt	[rdi]
	; Setup TSS
	mov		ax, GDT_OFFSET_TSS				; TSS descriptor: idt[5]
	ltr		ax
	; This will exploit iretq's ability to set segments in long mode
	; The order iretq will pop is: RIP, CS, RFLAGS, RSP, SS
	; We first need to save data we wish to keep
	pop		rsi								; keep the return address in rsi
	mov		rdi, rsp						; keep the old stack pointer in rdi
	mov		rax, GDT_OFFSET_KERNEL_DATA		; move the data segments
	mov		ds, ax
	mov		es, ax
	; Now to push what iretq needs
	push	rax								; push stack segment: g_idt[2]
	push	rdi								; push old stack pointer
	pushfq									; push flags
	push	QWORD GDT_OFFSET_KERNEL_CODE	; push code segment: g_idt[1]
	push	rsi								; set rip to the return address from earlier
	iretq									; far return in new code segment




GLOBAL	cpu_enable_features
cpu_enable_features:
	push	rbp
	mov		rbp, rsp
	push	rbx
	; Enable Coprocessors (FPU and SSE)
	mov		rax, cr0
	and		rax, 0xfffffffffffffffb	; disable FPU emulation
	or		rax, 0x22				; enable monitoring coprocessor and numeric error
	mov		cr0, rax
	mov		rax, cr4
	; TODO enable XSAVE here
	or		rax, 0x0406b0			; enable OSFXSR, OSXMMEXCPT and others
	mov		cr4, rax	
	fninit
	; Finish
	pop		rbx
	pop		rbp
	ret