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
lea r11, [r11+0xb7a30]
jmp r11

global if_alloc
if_alloc:
mov r11, [rel kernel_base]
lea r11, [r11+0x20f210]
jmp r11

global if_initname
if_initname:
mov r11, [rel kernel_base]
lea r11, [r11+0x214410]
jmp r11

global if_attach
if_attach:
mov r11, [rel kernel_base]
lea r11, [r11+0x20fa90]
jmp r11

global m_pullup
m_pullup:
mov r11, [rel kernel_base]
lea r11, [r11+0x260130]
jmp r11

global m_freem
m_freem:
mov r11, [rel kernel_base]
lea r11, [r11+0x25e300]
jmp r11

global m_length
m_length:
mov r11, [rel kernel_base]
lea r11, [r11+0x261090]
jmp r11

global netisr_queue
netisr_queue:
mov r11, [rel kernel_base]
lea r11, [r11+0x1b1010]
jmp r11

global m_devget
m_devget:
mov r11, [rel kernel_base]
lea r11, [r11+0x2608c0]
jmp r11

global malloc
malloc:
mov r11, [rel kernel_base]
lea r11, [r11+0x301b20]
jmp r11

global socreate
socreate:
mov r11, [rel kernel_base]
lea r11, [r11+0x3f19d0]
jmp r11

global sosend
sosend:
mov r11, [rel kernel_base]
lea r11, [r11+0x3f3700]
jmp r11

global soconnect2
soconnect2:
mov r11, [rel kernel_base]
lea r11, [r11+0x3f2d30]
jmp r11

global if_detach
if_detach:
mov r11, [rel kernel_base]
lea r11, [r11+0x2101f0]
jmp r11

global if_free
if_free:
mov r11, [rel kernel_base]
lea r11, [r11+0x20f8e0] ; actually if_free_type, second arg ignored
jmp r11

global soclose
soclose:
mov r11, [rel kernel_base]
lea r11, [r11+0x3f27c0]
jmp r11

global free
free:
mov r11, [rel kernel_base]
lea r11, [r11+0x301ce0]
jmp r11

global make_dev_p
make_dev_p:
mov r11, [rel kernel_base]
lea r11, [r11+0x1ef590]
jmp r11

global devfs_get_cdevpriv
devfs_get_cdevpriv:
mov r11, [rel kernel_base]
lea r11, [r11+0x1610b0]
jmp r11

global soreceive
soreceive:
mov r11, [rel kernel_base]
lea r11, [r11+0x3f52f0]
jmp r11

global devfs_set_cdevpriv
devfs_set_cdevpriv:
mov r11, [rel kernel_base]
lea r11, [r11+0x1610f0]
jmp r11

global uiomove
uiomove:
mov r11, [rel kernel_base]
lea r11, [r11+0x44e200]
jmp r11

global if_down
if_down:
mov r11, [rel kernel_base]
lea r11, [r11+0x211c70]
jmp r11

global mtx_init
mtx_init:
mov r11, [rel kernel_base]
lea r11, [r11+0x2ef960]
jmp r11

global mtx_destroy
mtx_destroy:
mov r11, [rel kernel_base]
lea r11, [r11+0x2ef9d0]
jmp r11

global _mtx_lock_sleep
_mtx_lock_sleep:
mov r11, [rel kernel_base]
lea r11, [r11+0x2eef50]
jmp r11

global _mtx_unlock_sleep
_mtx_unlock_sleep:
mov r11, [rel kernel_base]
lea r11, [r11+0x2ef270]
jmp r11

global soshutdown
soshutdown:
mov r11, [rel kernel_base]
lea r11, [r11+0x3f5310]
jmp r11

global m_copydata
m_copydata:
mov r11, [rel kernel_base]
lea r11, [r11+0x25fc10]
jmp r11

global sopoll
sopoll:
mov r11, [rel kernel_base]
lea r11, [r11+0x3f7290]
jmp r11

global copyin
copyin:
mov r11, [rel kernel_base]
lea r11, [r11+0x2716a0]
jmp r11

global m_dup
m_dup:
mov r11, [rel kernel_base]
lea r11, [r11+0x25fcc0]
jmp r11

global get_M_TEMP
get_M_TEMP:
mov rax, [rel kernel_base]
add rax, 0x15621e0
ret 

global get_curthread
get_curthread:
mov rax, [fs:0]
ret

global get_udp_usrreqs
get_udp_usrreqs:
mov rax, [rel kernel_base]
add rax, 0x1a7dbe8
ret

; ...
