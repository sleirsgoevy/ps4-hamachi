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

global if_alloc
if_alloc:
jmp qword [rel .addr]
.addr:
dq 0xffffffff808e38f0

global if_initname
if_initname:
jmp qword [rel .addr]
.addr:
dq 0xffffffff808df470

global if_attach
if_attach:
jmp qword [rel .addr]
.addr:
dq 0xffffffff808e2190

global m_pullup
m_pullup:
jmp qword [rel .addr]
.addr:
dq 0xffffffff80894940

global m_freem
m_freem:
jmp qword [rel .addr]
.addr:
dq 0xffffffff808927e0

global m_length
m_length:
jmp qword [rel .addr]
.addr:
dq 0xffffffff80891dc0

global netisr_queue
netisr_queue:
jmp qword [rel .addr]
.addr:
dq 0xffffffff808f1e80

global m_devget
m_devget:
jmp qword [rel .addr]
.addr:
dq 0xffffffff80892840

global malloc
malloc:
jmp qword [rel .addr]
.addr:
dq 0xffffffff8081fc50

global socreate
socreate:
jmp qword [rel .addr]
.addr:
dq 0xffffffff80899e30

global sosend
sosend:
jmp qword [rel .addr]
.addr:
dq 0xffffffff80898720

global soconnect2
soconnect2:
jmp qword [rel .addr]
.addr:
dq 0xffffffff80898640

global if_detach
if_detach:
jmp qword [rel .addr]
.addr:
dq 0xffffffff808e1610

global if_free
if_free:
jmp qword [rel .addr]
.addr:
dq 0xffffffff808e27e0

global soclose
soclose:
jmp qword [rel .addr]
.addr:
dq 0xffffffff8089a260

global free
free:
jmp qword [rel .addr]
.addr:
dq 0xffffffff8081fae0

global make_dev_p
make_dev_p:
jmp qword [rel .addr]
.addr:
dq 0xffffffff807ed4f0

global devfs_get_cdevpriv
devfs_get_cdevpriv:
jmp qword [rel .addr]
.addr:
dq 0xffffffff80753090

global soreceive
soreceive:
jmp qword [rel .addr]
.addr:
dq 0xffffffff80898740

global devfs_set_cdevpriv
devfs_set_cdevpriv:
jmp qword [rel .addr]
.addr:
dq 0xffffffff80753330

global uiomove
uiomove:
jmp qword [rel .addr]
.addr:
dq 0xffffffff80877770

global if_down
if_down:
jmp qword [rel .addr]
.addr:
dq 0xffffffff808e0eb0

global mtx_init
mtx_init:
jmp qword [rel .addr]
.addr:
dq 0xffffffff80822c50

global mtx_destroy
mtx_destroy:
jmp qword [rel .addr]
.addr:
dq 0xffffffff80823040

global _mtx_lock_sleep
_mtx_lock_sleep:
jmp qword [rel .addr]
.addr:
dq 0xffffffff80822e60

global _mtx_unlock_sleep
_mtx_unlock_sleep:
jmp qword [rel .addr]
.addr:
dq 0xffffffff80822d00

global soshutdown
soshutdown:
jmp qword [rel .addr]
.addr:
dq 0xffffffff808997f0

global sopoll
sopoll:
jmp qword [rel .addr]
.addr:
dq 0xffffffff80898760

global bpfattach
bpfattach:
jmp qword [rel .addr]
.addr:
dq 0xffffffff808da070

global bpfdetach
bpfdetach:
jmp qword [rel .addr]
.addr:
dq 0xffffffff808dd8f0

global bpf_mtap2
bpf_mtap2:
jmp qword [rel .addr]
.addr:
dq 0xffffffff808db3a0

global m_copydata
m_copydata:
jmp qword [rel .addr]
.addr:
dq 0xffffffff80892200

global copyin
copyin:
jmp qword [rel .addr]
.addr:
dq 0xffffffff80b16200

global get_M_TEMP
get_M_TEMP:
mov rax, 0xffffffff8107cc00
ret 

global get_sysent
get_sysent:
mov rax, 0xffffffff81072280
ret
; ...
