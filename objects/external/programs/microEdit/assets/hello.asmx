format ELF executable at 0xA0000000
entry _start

segment readable executable

_start:
	mov ecx, hello
	call strlen
	mov edx, eax

	mov eax, 0x4
	mov ebx, 1
	mov ecx, hello
	; mov edx, 12
	mov esi, 0
	int 0x30

	mov eax, 0xd
	mov ebx, 0
	int 0x30

; buf in ecx
strlen:
	mov eax, ecx
.loop:
	cmp [eax], byte 0
	je .out
	inc eax
	jmp .loop
.out:
	sub eax, ecx
	ret

hello: db "Hello world!", 0