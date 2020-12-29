section .text
use64

extern main

global _start
_start:
mov rcx, 0xc0000082
rdmsr
shl rax, 32
shr rax, 32
shl rdx, 32
or rax, rdx
sub rax, 0x1c0
mov [rel kernel_base], rax
jmp main

kernel_base:
dq 0

global printf
printf:
mov r11, [rel kernel_base]
lea r11, [r11+0xbc730]
jmp r11

global get_sysent
get_sysent:
mov rax, [rel kernel_base]
add rax, 0x1125660
ret

; ...
