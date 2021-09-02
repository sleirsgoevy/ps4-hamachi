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

global if_alloc
if_alloc:
mov r11, [rel kernel_base]
lea r11, [r11+0x3b040]
jmp r11

global if_initname
if_initname:
mov r11, [rel kernel_base]
lea r11, [r11+0x402c0]
jmp r11

global if_attach
if_attach:
mov r11, [rel kernel_base]
lea r11, [r11+0x3b8d0]
jmp r11

global m_pullup
m_pullup:
mov r11, [rel kernel_base]
lea r11, [r11+0x38afe0]
jmp r11

global m_freem
m_freem:
mov r11, [rel kernel_base]
lea r11, [r11+0x389130]
jmp r11

global m_length
m_length:
mov r11, [rel kernel_base]
lea r11, [r11+0x38bf90]
jmp r11

global netisr_queue
netisr_queue:
mov r11, [rel kernel_base]
lea r11, [r11+0x29b3b0]
jmp r11

global m_devget
m_devget:
mov r11, [rel kernel_base]
lea r11, [r11+0x38b780]
jmp r11

global malloc
malloc:
mov r11, [rel kernel_base]
lea r11, [r11+0x301840]
jmp r11

global socreate
socreate:
mov r11, [rel kernel_base]
lea r11, [r11+0x3c4210]
jmp r11

global sosend
sosend:
mov r11, [rel kernel_base]
lea r11, [r11+0x3c5f40]
jmp r11

global soconnect2
soconnect2:
mov r11, [rel kernel_base]
lea r11, [r11+0x3c5560]
jmp r11

global if_detach
if_detach:
mov r11, [rel kernel_base]
lea r11, [r11+0x3c040]
jmp r11

global if_free
if_free:
mov r11, [rel kernel_base]
lea r11, [r11+0x3b720] ; actually if_free_type, second arg ignored
jmp r11

global soclose
soclose:
mov r11, [rel kernel_base]
lea r11, [r11+0x3c5000]
jmp r11

global free
free:
mov r11, [rel kernel_base]
lea r11, [r11+0x301a40]
jmp r11

global make_dev_p
make_dev_p:
mov r11, [rel kernel_base]
lea r11, [r11+0x4221e0]
jmp r11

global devfs_get_cdevpriv
devfs_get_cdevpriv:
mov r11, [rel kernel_base]
lea r11, [r11+0x467d10]
jmp r11

global soreceive
soreceive:
mov r11, [rel kernel_base]
lea r11, [r11+0x3c7b70]
jmp r11

global devfs_set_cdevpriv
devfs_set_cdevpriv:
mov r11, [rel kernel_base]
lea r11, [r11+0x467d50]
jmp r11

global uiomove
uiomove:
mov r11, [rel kernel_base]
lea r11, [r11+0x20e1e0]
jmp r11

global if_down
if_down:
mov r11, [rel kernel_base]
lea r11, [r11+0x3db20]
jmp r11

global mtx_init
mtx_init:
mov r11, [rel kernel_base]
lea r11, [r11+0xbbb10]
jmp r11

global mtx_destroy
mtx_destroy:
mov r11, [rel kernel_base]
lea r11, [r11+0xbbb80]
jmp r11

global _mtx_lock_sleep
_mtx_lock_sleep:
mov r11, [rel kernel_base]
lea r11, [r11+0xbb100]
jmp r11

global _mtx_unlock_sleep
_mtx_unlock_sleep:
mov r11, [rel kernel_base]
lea r11, [r11+0xbb430]
jmp r11

global soshutdown
soshutdown:
mov r11, [rel kernel_base]
lea r11, [r11+0x3c7b90]
jmp r11

global m_copydata
m_copydata:
mov r11, [rel kernel_base]
lea r11, [r11+0x38aa80]
jmp r11

global sopoll
sopoll:
mov r11, [rel kernel_base]
lea r11, [r11+0x3c9b60]
jmp r11

global copyin
copyin:
mov r11, [rel kernel_base]
lea r11, [r11+0x2f230]
jmp r11

global m_dup
m_dup:
mov r11, [rel kernel_base]
lea r11, [r11+0x38ab30]
jmp r11

global get_M_TEMP
get_M_TEMP:
mov rax, [rel kernel_base]
add rax, 0x1a7ae50
ret 

global get_sysent
get_sysent:
mov rax, [rel kernel_base]
add rax, 0x1125660
ret

global get_curthread
get_curthread:
mov rax, [fs:0]
ret

global get_udp_usrreqs
get_udp_usrreqs:
mov rax, [rel kernel_base]
add rax, 0x1a88348
ret

; ...
