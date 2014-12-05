
SECTION .data

abs_mask:
dq  0x7fffffffffffffff, 0x7fffffffffffffff, 0x7fffffffffffffff, 0x7fffffffffffffff

stableadd_mul:
dq  1e-5, 1e-5, 1e-5, 1e-5

SECTION .text

global _denseToDenseAddSSE2_cache_64_linux
_denseToDenseAddSSE2_cache_64_linux:

push    rbp
mov     rbp, rsp

; rdi: address1
; rsi: address2
; rdx: address3
; rcx: count

mov     rax, rcx
shr     rcx, 1
and     rax, 0x01

movupd  xmm5, [abs_mask]
movupd  xmm4, [stableadd_mul]

start_denseToDenseAddSSE2_cache_64_linux:

movapd  xmm0, [rdi] ; a
movapd  xmm1, [rsi] ; b
movapd  xmm2, xmm0
addpd   xmm2, xmm1  ; xmm2 = c = a + b
movapd  xmm3, xmm2  ; xmm3 = c
andpd   xmm3, xmm5  ; xmm3 = |c|
andpd   xmm0, xmm5  ; xmm0 = |a|
andpd   xmm1, xmm5  ; xmm1 = |b|
addpd   xmm0, xmm1  ; xmm0 = |a| + |b|
mulpd   xmm0, xmm4
cmppd   xmm0, xmm3, 1
andpd   xmm2, xmm0
movapd  [rdx], xmm2

add     rdi, 16
add     rsi, 16
add     rdx, 16

loop    start_denseToDenseAddSSE2_cache_64_linux

; todo: majd a maradekot lekezelni
cmp     rax, 0
je      end_denseToDenseAddSSE2_cache_64_linux



end_denseToDenseAddSSE2_cache_64_linux:

mov     rsp, rbp
pop     rbp
ret

; -------------------------------------------

global _denseToDenseAddAVX_64_linux
_denseToDenseAddAVX_64_linux:

push    rbp
mov     rbp, rsp

; rdi: address1
; rsi: address2
; rdx: address3
; rcx: count

mov     rax, rcx
shr     rcx, 2
and     rax, 0x03

vmovupd  ymm5, [abs_mask]
vmovupd  ymm4, [stableadd_mul]

sub     rcx, 1

vmovapd  ymm0, [rdi] ; a
vmovapd  ymm1, [rsi] ; b

start_denseToDenseAddAVX_64_linux:

add     rdi, 32
add     rsi, 32
add     rdx, 32


;vaddpd   ymm0, [rsi]
vmovapd  ymm2, ymm0
vaddpd   ymm2, ymm1  ; xmm2 = c = a + b
vmovapd  ymm3, ymm2  ; xmm3 = c
vandpd   ymm3, ymm5  ; xmm3 = |c|
vandpd   ymm0, ymm5  ; xmm0 = |a]
vandpd   ymm1, ymm5  ; xmm1 = |b|
vaddpd   ymm0, ymm1  ; xmm0 = |a] + |b|

;;;;;;;;;;;;;;
vmovapd  ymm8, [rdi] ; a
vmovapd  ymm9, [rsi] ; b
;;;;;;;;;;;;;;

vmulpd   ymm0, ymm4
vcmppd   ymm0, ymm3, 1
vandpd   ymm2, ymm0
vmovapd  [rdx], ymm2

vmovapd  ymm0, ymm8
vmovapd  ymm1, ymm9


loop    start_denseToDenseAddAVX_64_linux

; todo: majd a maradekot lekezelni

mov     rsp, rbp
pop     rbp
ret
