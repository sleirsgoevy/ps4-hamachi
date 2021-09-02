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
lea r11, [r11+0x123280]
jmp r11

global if_alloc
if_alloc:
mov r11, [rel kernel_base]
lea r11, [r11+0x3a64f0]
jmp r11

global if_initname
if_initname:
mov r11, [rel kernel_base]
lea r11, [r11+0x3ab7d0]
jmp r11

global if_attach
if_attach:
mov r11, [rel kernel_base]
lea r11, [r11+0x3a6d80]
jmp r11

global m_pullup
m_pullup:
mov r11, [rel kernel_base]
lea r11, [r11+0x2ac7b0]
jmp r11

global m_freem
m_freem:
mov r11, [rel kernel_base]
lea r11, [r11+0x2aa8a0]
jmp r11

global m_length
m_length:
mov r11, [rel kernel_base]
lea r11, [r11+0x2ad750]
jmp r11

global netisr_queue
netisr_queue:
mov r11, [rel kernel_base]
lea r11, [r11+0x4887c0]
jmp r11

global m_devget
m_devget:
mov r11, [rel kernel_base]
lea r11, [r11+0x2acf50]
jmp r11

global malloc
malloc:
mov r11, [rel kernel_base]
lea r11, [r11+0xd7a0]
jmp r11

global socreate
socreate:
mov r11, [rel kernel_base]
lea r11, [r11+0x3cfa70]
jmp r11

global sosend
sosend:
mov r11, [rel kernel_base]
lea r11, [r11+0x3d1770]
jmp r11

global soconnect2
soconnect2:
mov r11, [rel kernel_base]
lea r11, [r11+0x3d0da0]
jmp r11

global if_detach
if_detach:
mov r11, [rel kernel_base]
lea r11, [r11+0x3a7500]
jmp r11

global if_free
if_free:
mov r11, [rel kernel_base]
lea r11, [r11+0x3a6d40]
jmp r11

global soclose
soclose:
mov r11, [rel kernel_base]
lea r11, [r11+0x3d0850]
jmp r11

global free
free:
mov r11, [rel kernel_base]
lea r11, [r11+0xd9a0]
jmp r11

global make_dev_p
make_dev_p:
mov r11, [rel kernel_base]
lea r11, [r11+0x3bd770]
jmp r11

global devfs_get_cdevpriv
devfs_get_cdevpriv:
mov r11, [rel kernel_base]
lea r11, [r11+0x252120]
jmp r11

global soreceive
soreceive:
mov r11, [rel kernel_base]
lea r11, [r11+0x3d33f0]
jmp r11

global devfs_set_cdevpriv
devfs_set_cdevpriv:
mov r11, [rel kernel_base]
lea r11, [r11+0x252160]
jmp r11

global uiomove
uiomove:
mov r11, [rel kernel_base]
lea r11, [r11+0x2d2a70]
jmp r11

global if_down
if_down:
mov r11, [rel kernel_base]
lea r11, [r11+0x3a9020]
jmp r11

global mtx_init
mtx_init:
mov r11, [rel kernel_base]
lea r11, [r11+0x496fe0]
jmp r11

global mtx_destroy
mtx_destroy:
mov r11, [rel kernel_base]
lea r11, [r11+0x497050]
jmp r11

global _mtx_lock_sleep
_mtx_lock_sleep:
mov r11, [rel kernel_base]
lea r11, [r11+0x4965e0]
jmp r11

global _mtx_unlock_sleep
_mtx_unlock_sleep:
mov r11, [rel kernel_base]
lea r11, [r11+0x496910]
jmp r11

global soshutdown
soshutdown:
mov r11, [rel kernel_base]
lea r11, [r11+0x3d3410]
jmp r11

global m_copydata
m_copydata:
mov r11, [rel kernel_base]
lea r11, [r11+0x2ac250]
jmp r11

global sopoll
sopoll:
mov r11, [rel kernel_base]
lea r11, [r11+0x3d5380]
jmp r11

global copyin
copyin:
mov r11, [rel kernel_base]
lea r11, [r11+0x3c17a0]
jmp r11

global m_dup
m_dup:
mov r11, [rel kernel_base]
lea r11, [r11+0x2ac300]
jmp r11

global get_M_TEMP
get_M_TEMP:
mov rax, [rel kernel_base]
add rax, 0x1540eb0
ret 

global get_sysent
get_sysent:
mov rax, [rel kernel_base]
add rax, 0x111e000
ret

global get_curthread
get_curthread:
mov rax, [fs:0]
ret

global get_udp_usrreqs
get_udp_usrreqs:
mov rax, [rel kernel_base]
add rax, 0x1a5dfa8
ret

; ...
