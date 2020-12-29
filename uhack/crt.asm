section .text
use64

extern main

global _start
_start:
jmp main

global printf
printf:
jmp qword [rel .addr]
.addr:
dq 0xffffffff8086bf90

global get_sysent
get_sysent:
mov rax, 0xffffffff81072280
ret
; ...
