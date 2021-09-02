section .text
use64

extern main

global _start
_start:
mov rcx, 0xc0000082
rdmsr
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
lea r11, [r11+0x26f740]
jmp r11

global if_alloc
if_alloc:
mov r11, [rel kernel_base]
lea r11, [r11+0x2ad790]
jmp r11

global if_initname
if_initname:
mov r11, [rel kernel_base]
lea r11, [r11+0x2b2a60]
jmp r11

global if_attach
if_attach:
mov r11, [rel kernel_base]
lea r11, [r11+0x2ae020]
jmp r11

global m_pullup
m_pullup:
mov r11, [rel kernel_base]
lea r11, [r11+0xb4730]
jmp r11

global m_freem
m_freem:
mov r11, [rel kernel_base]
lea r11, [r11+0xb28b0]
jmp r11

global m_length
m_length:
mov r11, [rel kernel_base]
lea r11, [r11+0xb5670]
jmp r11

global netisr_queue
netisr_queue:
mov r11, [rel kernel_base]
lea r11, [r11+0x4777d0]
jmp r11

global m_devget
m_devget:
mov r11, [rel kernel_base]
lea r11, [r11+0xb4ec0]
jmp r11

global malloc
malloc:
mov r11, [rel kernel_base]
lea r11, [r11+0x1d6680]
jmp r11

global socreate
socreate:
mov r11, [rel kernel_base]
lea r11, [r11+0x3b950]
jmp r11

global sosend
sosend:
mov r11, [rel kernel_base]
lea r11, [r11+0x3d6a0]
jmp r11

global soconnect2
soconnect2:
mov r11, [rel kernel_base]
lea r11, [r11+0x3ccc0]
jmp r11

global if_detach
if_detach:
mov r11, [rel kernel_base]
lea r11, [r11+0x2ae780]
jmp r11

global if_free
if_free:
mov r11, [rel kernel_base]
lea r11, [r11+0x2adfe0] ; actually if_free_type, second arg ignored
jmp r11

global soclose
soclose:
mov r11, [rel kernel_base]
lea r11, [r11+0x3c750]
jmp r11

global free
free:
mov r11, [rel kernel_base]
lea r11, [r11+0x1d6870]
jmp r11

global make_dev_p
make_dev_p:
mov r11, [rel kernel_base]
lea r11, [r11+0x9f150]
jmp r11

global devfs_get_cdevpriv
devfs_get_cdevpriv:
mov r11, [rel kernel_base]
lea r11, [r11+0x2bb1f0]
jmp r11

global soreceive
soreceive:
mov r11, [rel kernel_base]
lea r11, [r11+0x3f2d0]
jmp r11

global devfs_set_cdevpriv
devfs_set_cdevpriv:
mov r11, [rel kernel_base]
lea r11, [r11+0x2bb230]
jmp r11

global uiomove
uiomove:
mov r11, [rel kernel_base]
lea r11, [r11+0x388690]
jmp r11

global if_down
if_down:
mov r11, [rel kernel_base]
lea r11, [r11+0x2b0250]
jmp r11

global mtx_init
mtx_init:
mov r11, [rel kernel_base]
lea r11, [r11+0x3105b0]
jmp r11

global mtx_destroy
mtx_destroy:
mov r11, [rel kernel_base]
lea r11, [r11+0x310620]
jmp r11

global _mtx_lock_sleep
_mtx_lock_sleep:
mov r11, [rel kernel_base]
lea r11, [r11+0x30fbb0]
jmp r11

global _mtx_unlock_sleep
_mtx_unlock_sleep:
mov r11, [rel kernel_base]
lea r11, [r11+0x30fed0]
jmp r11

global soshutdown
soshutdown:
mov r11, [rel kernel_base]
lea r11, [r11+0x3f2f0]
jmp r11

global m_copydata
m_copydata:
mov r11, [rel kernel_base]
lea r11, [r11+0xb4210]
jmp r11

global sopoll
sopoll:
mov r11, [rel kernel_base]
lea r11, [r11+0x412a0]
jmp r11

global copyin
copyin:
mov r11, [rel kernel_base]
lea r11, [r11+0x28f9f0]
jmp r11

global m_dup
m_dup:
mov r11, [rel kernel_base]
lea r11, [r11+0xb42c0]
jmp r11

global get_M_TEMP
get_M_TEMP:
mov rax, [rel kernel_base]
add rax, 0x1556da0
ret 

global get_sysent
get_sysent:
mov rax, [rel kernel_base]
add rax, 0x1122340
ret

global get_curthread
get_curthread:
mov rax, [fs:0]
ret

global get_udp_usrreqs
get_udp_usrreqs:
mov rax, [rel kernel_base]
add rax, 0x159aa48
ret

; ...
