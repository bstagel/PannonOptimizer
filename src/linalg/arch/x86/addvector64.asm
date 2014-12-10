
;extern _abs_tolerance
;extern _rel_tolerance

SECTION .data

abs_mask:
dq  0x7fffffffffffffff, 0x7fffffffffffffff, 0x7fffffffffffffff, 0x7fffffffffffffff

stableadd_mul:
dq  1e-5, 1e-5, 1e-5, 1e-5

SECTION .text

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Dense to dense
;; Uses cache
;; SSE2
;; Absolute and relative tolerances
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _denseToDenseAddAbsRelSSE2_cache_64_linux
_denseToDenseAddAbsRelSSE2_cache_64_linux:

push    rbp
mov     rbp, rsp

; rdi: address1
; rsi: address2
; rdx: address3
; rcx: count
; xmm0: lambda
; xmm1: absTolerance
; xmm2: relTolerance

mov     rax, rcx
shr     rcx, 1
and     rax, 0x01

movupd  xmm5, [abs_mask]
shufpd  xmm1, xmm1, 00b
shufpd  xmm2, xmm2, 00b
movupd  xmm4, xmm2
movupd  xmm6, xmm1
shufpd  xmm0, xmm0, 00b
movupd  xmm7, xmm0

cmp     rcx, 0
je      after_loop_denseToDenseAddAbsRelSSE2_cache_64_linux

start_denseToDenseAddAbsRelSSE2_cache_64_linux:

movapd  xmm0, [rdi] ; a
movapd  xmm1, [rsi] ; b
mulpd   xmm1, xmm7  ; b = lambda * b
movapd  xmm2, xmm0
addpd   xmm2, xmm1  ; xmm2 = c = a + b
movapd  xmm3, xmm2  ; xmm3 = c
andpd   xmm3, xmm5  ; xmm3 = |c|
andpd   xmm0, xmm5  ; xmm0 = |a|
andpd   xmm1, xmm5  ; xmm1 = |b|
addpd   xmm0, xmm1  ; xmm0 = |a| + |b|
mulpd   xmm0, xmm4
cmppd   xmm0, xmm3, 1
; |a| + |b| * mul < |c|
;    c is unchanged
; else
;    c = 0.0
andpd   xmm2, xmm0
; abs tolerance
movapd  xmm0, xmm2  ; result in xmm2
andpd   xmm0, xmm5  ; absolute value of the result in xmm0
cmppd   xmm0, xmm6, 0xD ; absolute tolerance
andpd   xmm2, xmm0
movapd  [rdx], xmm2

add     rdi, 16
add     rsi, 16
add     rdx, 16

loop    start_denseToDenseAddAbsRelSSE2_cache_64_linux

after_loop_denseToDenseAddAbsRelSSE2_cache_64_linux:

cmp     rax, 0
je      end_denseToDenseAddAbsRelSSE2_cache_64_linux

movlpd  xmm0, [rdi] ; a
movlpd  xmm1, [rsi] ; b
mulsd   xmm1, xmm7  ; b = lambda * b
movapd  xmm2, xmm0
addsd   xmm2, xmm1  ; xmm2 = c = a + b
movapd  xmm3, xmm2  ; xmm3 = c
andpd   xmm3, xmm5  ; xmm3 = |c|
andpd   xmm0, xmm5  ; xmm0 = |a|
andpd   xmm1, xmm5  ; xmm1 = |b|
addsd   xmm0, xmm1  ; xmm0 = |a| + |b|
mulsd   xmm0, xmm4
cmpsd   xmm0, xmm3, 1
; |a| + |b| * mul < |c|
;    c is unchanged
; else
;    c = 0.0
andpd   xmm2, xmm0
; abs tolerance
movapd  xmm0, xmm2  ; result in xmm2
andpd   xmm0, xmm5  ; absolute value of the result in xmm0
cmpsd   xmm0, xmm6, 0xD ; absolute tolerance
andpd   xmm2, xmm0
movlpd  [rdx], xmm2

end_denseToDenseAddAbsRelSSE2_cache_64_linux:

mov     rsp, rbp
pop     rbp
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Dense to dense
;; Without cache (for storing the result)
;; SSE2
;; Absolute and relative tolerances
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _denseToDenseAddAbsRelSSE2_nocache_64_linux
_denseToDenseAddAbsRelSSE2_nocache_64_linux:

push    rbp
mov     rbp, rsp

; rdi: address1
; rsi: address2
; rdx: address3
; rcx: count
; xmm0: lambda
; xmm1: absTolerance
; xmm2: relTolerance

mov     rax, rcx
shr     rcx, 1
and     rax, 0x01

movupd  xmm5, [abs_mask]
shufpd  xmm1, xmm1, 00b
shufpd  xmm2, xmm2, 00b
movupd  xmm4, xmm2
movupd  xmm6, xmm1
shufpd  xmm0, xmm0, 00b
movupd  xmm7, xmm0

cmp     rcx, 0
je      after_loop_denseToDenseAddAbsRelSSE2_nocache_64_linux

start_denseToDenseAddAbsRelSSE2_nocache_64_linux:

movapd  xmm0, [rdi] ; a
movapd  xmm1, [rsi] ; b
mulpd   xmm1, xmm7  ; b = lambda * b
movapd  xmm2, xmm0
addpd   xmm2, xmm1  ; xmm2 = c = a + b
movapd  xmm3, xmm2  ; xmm3 = c
andpd   xmm3, xmm5  ; xmm3 = |c|
andpd   xmm0, xmm5  ; xmm0 = |a|
andpd   xmm1, xmm5  ; xmm1 = |b|
addpd   xmm0, xmm1  ; xmm0 = |a| + |b|
mulpd   xmm0, xmm4
cmppd   xmm0, xmm3, 1
; |a| + |b| * mul < |c|
;    c is unchanged
; else
;    c = 0.0
andpd   xmm2, xmm0
; abs tolerance
movapd  xmm0, xmm2  ; result in xmm2
andpd   xmm0, xmm5  ; absolute value of the result in xmm0
cmppd   xmm0, xmm6, 0xD ; absolute tolerance
andpd   xmm2, xmm0
movntpd  [rdx], xmm2 ; writes the result without the cache

add     rdi, 16
add     rsi, 16
add     rdx, 16

loop    start_denseToDenseAddAbsRelSSE2_nocache_64_linux

after_loop_denseToDenseAddAbsRelSSE2_nocache_64_linux:

; todo: majd a maradekot lekezelni
cmp     rax, 0
je      end_denseToDenseAddAbsRelSSE2_nocache_64_linux

movlpd  xmm0, [rdi] ; a
movlpd  xmm1, [rsi] ; b
mulsd   xmm1, xmm7  ; b = lambda * b
movapd  xmm2, xmm0
addsd   xmm2, xmm1  ; xmm2 = c = a + b
movapd  xmm3, xmm2  ; xmm3 = c
andpd   xmm3, xmm5  ; xmm3 = |c|
andpd   xmm0, xmm5  ; xmm0 = |a|
andpd   xmm1, xmm5  ; xmm1 = |b|
addsd   xmm0, xmm1  ; xmm0 = |a| + |b|
mulsd   xmm0, xmm4
cmpsd   xmm0, xmm3, 1
; |a| + |b| * mul < |c|
;    c is unchanged
; else
;    c = 0.0
andpd   xmm2, xmm0
; abs tolerance
movapd  xmm0, xmm2  ; result in xmm2
andpd   xmm0, xmm5  ; absolute value of the result in xmm0
cmpsd   xmm0, xmm6, 0xD ; absolute tolerance
andpd   xmm2, xmm0
movapd  [rdx], xmm2

end_denseToDenseAddAbsRelSSE2_nocache_64_linux:

mov     rsp, rbp
pop     rbp
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Dense to dense
;; Uses cache
;; SSE2
;; Absolute tolerance
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _denseToDenseAddAbsSSE2_cache_64_linux
_denseToDenseAddAbsSSE2_cache_64_linux:

push    rbp
mov     rbp, rsp

; rdi: address1
; rsi: address2
; rdx: address3
; rcx: count
; xmm0: lambda
; xmm1: absTolerance

mov     rax, rcx
shr     rcx, 1
and     rax, 0x01

movupd  xmm5, [abs_mask]
shufpd  xmm1, xmm1, 00b
movupd  xmm6, xmm1
shufpd  xmm0, xmm0, 00b
movupd  xmm7, xmm0

cmp     rcx, 0
je      after_loop_denseToDenseAddAbsSSE2_cache_64_linux

start_denseToDenseAddAbsSSE2_cache_64_linux:

movapd  xmm0, [rdi] ; a
movapd  xmm1, [rsi] ; b
mulpd   xmm1, xmm7  ; b = lambda * b
addpd   xmm0, xmm1  ; xmm0 = c = a + b
movapd  xmm2, xmm0
andpd   xmm2, xmm5  ; xmm2 = |c|
cmppd   xmm2, xmm6, 0xD
andpd   xmm2, xmm0
movapd  [rdx], xmm2

add     rdi, 16
add     rsi, 16
add     rdx, 16

loop    start_denseToDenseAddAbsSSE2_cache_64_linux

after_loop_denseToDenseAddAbsSSE2_cache_64_linux:

; todo: majd a maradekot lekezelni
cmp     rax, 0
je      end_denseToDenseAddAbsSSE2_cache_64_linux

movlpd  xmm0, [rdi] ; a
movlpd  xmm1, [rsi] ; b
mulsd   xmm1, xmm7  ; b = lambda * b
addsd   xmm0, xmm1  ; xmm0 = c = a + b
movapd  xmm2, xmm0
andpd   xmm2, xmm5  ; xmm2 = |c|
cmpsd   xmm2, xmm6, 0xD
andpd   xmm2, xmm0
movlpd  [rdx], xmm2

end_denseToDenseAddAbsSSE2_cache_64_linux:

mov     rsp, rbp
pop     rbp
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Dense to dense
;; Without cache (for storing the result)
;; SSE2
;; Absolute tolerance
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _denseToDenseAddAbsSSE2_nocache_64_linux
_denseToDenseAddAbsSSE2_nocache_64_linux:

push    rbp
mov     rbp, rsp

; rdi: address1
; rsi: address2
; rdx: address3
; rcx: count
; xmm0: lambda
; xmm1: absTolerance

mov     rax, rcx
shr     rcx, 1
and     rax, 0x01

movupd  xmm5, [abs_mask]
shufpd  xmm1, xmm1, 00b
movupd  xmm6, xmm1
shufpd  xmm0, xmm0, 00b
movupd  xmm7, xmm0

cmp     rcx, 0
je      after_loop_denseToDenseAddAbsSSE2_nocache_64_linux

start_denseToDenseAddAbsSSE2_nocache_64_linux:

movapd  xmm0, [rdi] ; a
movapd  xmm1, [rsi] ; b
mulpd   xmm1, xmm7  ; b = lambda * b
addpd   xmm0, xmm1  ; xmm0 = c = a + b
movapd  xmm2, xmm0
andpd   xmm2, xmm5  ; xmm2 = |c|
cmppd   xmm2, xmm6, 0xD
andpd   xmm2, xmm0
movntpd  [rdx], xmm2

add     rdi, 16
add     rsi, 16
add     rdx, 16

loop    start_denseToDenseAddAbsSSE2_nocache_64_linux

after_loop_denseToDenseAddAbsSSE2_nocache_64_linux:

; todo: majd a maradekot lekezelni
cmp     rax, 0
je      end_denseToDenseAddAbsSSE2_nocache_64_linux

movlpd  xmm0, [rdi] ; a
movlpd  xmm1, [rsi] ; b
mulsd   xmm1, xmm7  ; b = lambda * b
addsd   xmm0, xmm1  ; xmm0 = c = a + b
movapd  xmm2, xmm0
andpd   xmm2, xmm5  ; xmm2 = |c|
cmpsd   xmm2, xmm6, 0xD
andpd   xmm2, xmm0
movlpd  [rdx], xmm2

end_denseToDenseAddAbsSSE2_nocache_64_linux:

mov     rsp, rbp
pop     rbp
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Dense to dense
;; Uses cache
;; SSE2
;; Without tolerances
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _denseToDenseAddSSE2_cache_64_linux
_denseToDenseAddSSE2_cache_64_linux:

push    rbp
mov     rbp, rsp

; rdi: address1
; rsi: address2
; rdx: address3
; rcx: count
; xmm0: lambda

mov     rax, rcx
shr     rcx, 1
and     rax, 0x01

movupd  xmm5, [abs_mask]
shufpd  xmm0, xmm0, 00b
movupd  xmm7, xmm0

cmp     rcx, 0
je      after_loop_denseToDenseAddSSE2_cache_64_linux

start_denseToDenseAddSSE2_cache_64_linux:

movapd  xmm0, [rdi] ; a
movapd  xmm1, xmm7
mulpd   xmm1, [rsi] ; b
addpd   xmm0, xmm1  ; xmm0 = c = a + b
movapd  [rdx], xmm0

add     rdi, 16
add     rsi, 16
add     rdx, 16

loop    start_denseToDenseAddSSE2_cache_64_linux

after_loop_denseToDenseAddSSE2_cache_64_linux:

; todo: majd a maradekot lekezelni
cmp     rax, 0
je      end_denseToDenseAddSSE2_cache_64_linux

movlpd  xmm0, [rdi] ; a
movapd  xmm1, xmm7
mulsd   xmm1, [rsi] ; b
addsd   xmm0, xmm1  ; xmm0 = c = a + b
movlpd  [rdx], xmm0


end_denseToDenseAddSSE2_cache_64_linux:

mov     rsp, rbp
pop     rbp
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Dense to dense
;; Without cache (for storing the result)
;; SSE2
;; Without tolerances
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _denseToDenseAddSSE2_nocache_64_linux
_denseToDenseAddSSE2_nocache_64_linux:

push    rbp
mov     rbp, rsp

; rdi: address1
; rsi: address2
; rdx: address3
; rcx: count
; xmm0: lambda

mov     rax, rcx
shr     rcx, 1
and     rax, 0x01

movupd  xmm5, [abs_mask]
shufpd  xmm0, xmm0, 00b
movupd  xmm7, xmm0

cmp     rcx, 0
je      after_loop_denseToDenseAddSSE2_nocache_64_linux

start_denseToDenseAddSSE2_nocache_64_linux:

movapd  xmm0, [rdi] ; a
movapd  xmm1, xmm7
mulpd   xmm1, [rsi] ; b
addpd   xmm0, xmm1  ; xmm0 = c = a + b
movntpd  [rdx], xmm0

add     rdi, 16
add     rsi, 16
add     rdx, 16

loop    start_denseToDenseAddSSE2_nocache_64_linux

after_loop_denseToDenseAddSSE2_nocache_64_linux:

; todo: majd a maradekot lekezelni
cmp     rax, 0
je      end_denseToDenseAddSSE2_nocache_64_linux

movlpd  xmm0, [rdi] ; a
movapd  xmm1, xmm7
mulsd   xmm1, [rsi] ; b
addsd   xmm0, xmm1  ; xmm0 = c = a + b
movlpd  [rdx], xmm0


end_denseToDenseAddSSE2_nocache_64_linux:

mov     rsp, rbp
pop     rbp
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Dense to dense
;; Uses cache
;; AVX
;; Absolute and relative tolerances
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _denseToDenseAddAbsRelAVX_cache_64_linux
_denseToDenseAddAbsRelAVX_cache_64_linux:

push    rbp
mov     rbp, rsp

; rdi: address1
; rsi: address2
; rdx: address3
; rcx: count
; xmm0: lambda
; xmm1: absTolerance
; xmm2: relTolerance

mov     rax, rcx
shr     rcx, 2
and     rax, 0x03

vmovupd  ymm5, [abs_mask]

sub     rsp, 24 ; make space for 3 doubles

movlpd  [rbp - 8], xmm1
movlpd  [rbp - 16], xmm2
movlpd  [rbp - 24], xmm0

vbroadcastsd ymm1, [rbp - 8]
vbroadcastsd ymm2, [rbp - 16]

vmovupd  ymm4, ymm2
vmovupd  ymm6, ymm1

vbroadcastsd ymm7, [rbp - 24]


cmp     rcx, 0
je      after_loop_denseToDenseAddAbsRelAVX_cache_64_linux

start_denseToDenseAddAbsRelAVX_cache_64_linux:

vmovapd  ymm0, [rdi] ; a
vmovapd  ymm1, [rsi] ; b
vmulpd   ymm1, ymm7  ; b = lambda * b
vmovapd  ymm2, ymm0
vaddpd   ymm2, ymm1  ; xmm2 = c = a + b
vmovapd  ymm3, ymm2  ; xmm3 = c
vandpd   ymm3, ymm5  ; xmm3 = |c|
vandpd   ymm0, ymm5  ; xmm0 = |a|
vandpd   ymm1, ymm5  ; xmm1 = |b|
vaddpd   ymm0, ymm1  ; xmm0 = |a| + |b|
vmulpd   ymm0, ymm4
vcmppd   ymm0, ymm3, 1
; |a| + |b| * mul < |c|
;    c is unchanged
; else
;    c = 0.0
vandpd   ymm2, ymm0
; abs tolerance
vmovapd  ymm0, ymm2  ; result in xmm2
vandpd   ymm0, ymm5  ; absolute value of the result in xmm0
vcmppd   ymm0, ymm6, 0xD ; absolute tolerance
vandpd   ymm2, ymm0
vmovapd  [rdx], ymm2

add     rdi, 32
add     rsi, 32
add     rdx, 32

loop    start_denseToDenseAddAbsRelAVX_cache_64_linux

after_loop_denseToDenseAddAbsRelAVX_cache_64_linux:

cmp     rax, 0
je      end_denseToDenseAddAbsRelAVX_cache_64_linux

mov     rcx, rax

last_loop_denseToDenseAddAbsRelAVX_cache_64_linux:

movlpd  xmm0, [rdi] ; a
movlpd  xmm1, [rsi] ; b
mulsd   xmm1, xmm7  ; b = lambda * b
movapd  xmm2, xmm0
addsd   xmm2, xmm1  ; xmm2 = c = a + b
movapd  xmm3, xmm2  ; xmm3 = c
andpd   xmm3, xmm5  ; xmm3 = |c|
andpd   xmm0, xmm5  ; xmm0 = |a|
andpd   xmm1, xmm5  ; xmm1 = |b|
addsd   xmm0, xmm1  ; xmm0 = |a| + |b|
mulsd   xmm0, xmm4
cmpsd   xmm0, xmm3, 1
; |a| + |b| * mul < |c|
;    c is unchanged
; else
;    c = 0.0
andpd   xmm2, xmm0
; abs tolerance
movapd  xmm0, xmm2  ; result in xmm2
andpd   xmm0, xmm5  ; absolute value of the result in xmm0
cmpsd   xmm0, xmm6, 0xD ; absolute tolerance
andpd   xmm2, xmm0
movlpd  [rdx], xmm2

add     rdi, 8
add     rsi, 8
add     rdx, 8

loop    last_loop_denseToDenseAddAbsRelAVX_cache_64_linux

end_denseToDenseAddAbsRelAVX_cache_64_linux:

mov     rsp, rbp
pop     rbp
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Dense to dense
;; Without cache (for storing the result)
;; AVX
;; Absolute and relative tolerances
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _denseToDenseAddAbsRelAVX_nocache_64_linux
_denseToDenseAddAbsRelAVX_nocache_64_linux:

push    rbp
mov     rbp, rsp

; rdi: address1
; rsi: address2
; rdx: address3
; rcx: count
; xmm0: lambda
; xmm1: absTolerance
; xmm2: relTolerance

mov     rax, rcx
shr     rcx, 2
and     rax, 0x03

vmovupd  ymm5, [abs_mask]

sub     rsp, 24 ; make space for 3 doubles

movlpd  [rbp - 8], xmm1
movlpd  [rbp - 16], xmm2
movlpd  [rbp - 24], xmm0

vbroadcastsd ymm6, [rbp - 8]
vbroadcastsd ymm4, [rbp - 16]
vbroadcastsd ymm7, [rbp - 24]


cmp     rcx, 0
je      after_loop_denseToDenseAddAbsRelAVX_nocache_64_linux

start_denseToDenseAddAbsRelAVX_nocache_64_linux:

vmovapd  ymm0, [rdi] ; a
vmovapd  ymm1, [rsi] ; b
vmulpd   ymm1, ymm7  ; b = lambda * b
vmovapd  ymm2, ymm0
vaddpd   ymm2, ymm1  ; xmm2 = c = a + b
vmovapd  ymm3, ymm2  ; xmm3 = c
vandpd   ymm3, ymm5  ; xmm3 = |c|
vandpd   ymm0, ymm5  ; xmm0 = |a|
vandpd   ymm1, ymm5  ; xmm1 = |b|
vaddpd   ymm0, ymm1  ; xmm0 = |a| + |b|
vmulpd   ymm0, ymm4
vcmppd   ymm0, ymm3, 1
; |a| + |b| * mul < |c|
;    c is unchanged
; else
;    c = 0.0
vandpd   ymm2, ymm0
; abs tolerance
vmovapd  ymm0, ymm2  ; result in xmm2
vandpd   ymm0, ymm5  ; absolute value of the result in xmm0
vcmppd   ymm0, ymm6, 0xD ; absolute tolerance
vandpd   ymm2, ymm0
vmovntpd  [rdx], ymm2

add     rdi, 32
add     rsi, 32
add     rdx, 32

loop    start_denseToDenseAddAbsRelAVX_nocache_64_linux

after_loop_denseToDenseAddAbsRelAVX_nocache_64_linux:

cmp     rax, 0
je      end_denseToDenseAddAbsRelAVX_nocache_64_linux

mov     rcx, rax

last_loop_denseToDenseAddAbsRelAVX_nocache_64_linux:

movlpd  xmm0, [rdi] ; a
movlpd  xmm1, [rsi] ; b
mulsd   xmm1, xmm7  ; b = lambda * b
movapd  xmm2, xmm0
addsd   xmm2, xmm1  ; xmm2 = c = a + b
movapd  xmm3, xmm2  ; xmm3 = c
andpd   xmm3, xmm5  ; xmm3 = |c|
andpd   xmm0, xmm5  ; xmm0 = |a|
andpd   xmm1, xmm5  ; xmm1 = |b|
addsd   xmm0, xmm1  ; xmm0 = |a| + |b|
mulsd   xmm0, xmm4
cmpsd   xmm0, xmm3, 1
; |a| + |b| * mul < |c|
;    c is unchanged
; else
;    c = 0.0
andpd   xmm2, xmm0
; abs tolerance
movapd  xmm0, xmm2  ; result in xmm2
andpd   xmm0, xmm5  ; absolute value of the result in xmm0
cmpsd   xmm0, xmm6, 0xD ; absolute tolerance
andpd   xmm2, xmm0
movlpd  [rdx], xmm2

add     rdi, 8
add     rsi, 8
add     rdx, 8

loop    last_loop_denseToDenseAddAbsRelAVX_nocache_64_linux

end_denseToDenseAddAbsRelAVX_nocache_64_linux:

mov     rsp, rbp
pop     rbp
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Dense to dense
;; Uses cache
;; AVX
;; Absolute tolerance
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _denseToDenseAddAbsAVX_cache_64_linux
_denseToDenseAddAbsAVX_cache_64_linux:

push    rbp
mov     rbp, rsp

; rdi: address1
; rsi: address2
; rdx: address3
; rcx: count
; xmm0: lambda
; xmm1: absTolerance

mov     rax, rcx
shr     rcx, 2
and     rax, 0x03

vmovupd  ymm5, [abs_mask]

sub     rsp, 16

movlpd  [rbp - 8], xmm1
movlpd  [rbp - 16], xmm0

vbroadcastsd    ymm6, [rbp - 8]
vbroadcastsd    ymm7, [rbp - 16]

cmp     rcx, 0
je      after_loop_denseToDenseAddAbsAVX_cache_64_linux

start_denseToDenseAddAbsAVX_cache_64_linux:

vmovapd  ymm0, [rdi] ; a
vmovapd  ymm1, [rsi] ; b
vmulpd   ymm1, ymm7  ; b = lambda * b
vaddpd   ymm0, ymm1  ; xmm0 = c = a + b
vmovapd  ymm2, ymm0
vandpd   ymm2, ymm5  ; xmm2 = |c|
vcmppd   ymm2, ymm6, 0xD
vandpd   ymm2, ymm0
vmovapd  [rdx], ymm2

add     rdi, 32
add     rsi, 32
add     rdx, 32

loop    start_denseToDenseAddAbsAVX_cache_64_linux

after_loop_denseToDenseAddAbsAVX_cache_64_linux:

cmp     rax, 0
je      end_denseToDenseAddAbsAVX_cache_64_linux

mov     rcx, rax

last_loop_denseToDenseAddAbsAVX_cache_64_linux:

movlpd  xmm0, [rdi] ; a
movlpd  xmm1, [rsi] ; b
mulsd   xmm1, xmm7  ; b = lambda * b
addsd   xmm0, xmm1  ; xmm0 = c = a + b
movapd  xmm2, xmm0
andpd   xmm2, xmm5  ; xmm2 = |c|
cmpsd   xmm2, xmm6, 0xD
andpd   xmm2, xmm0
movlpd  [rdx], xmm2

add     rdi, 8
add     rsi, 8
add     rdx, 8

loop    last_loop_denseToDenseAddAbsAVX_cache_64_linux

end_denseToDenseAddAbsAVX_cache_64_linux:

mov     rsp, rbp
pop     rbp
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Dense to dense
;; Without cache (for storing the result)
;; AVX
;; Absolute tolerance
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _denseToDenseAddAbsAVX_nocache_64_linux
_denseToDenseAddAbsAVX_nocache_64_linux:

push    rbp
mov     rbp, rsp

; rdi: address1
; rsi: address2
; rdx: address3
; rcx: count
; xmm0: lambda
; xmm1: absTolerance

mov     rax, rcx
shr     rcx, 2
and     rax, 0x03

vmovupd  ymm5, [abs_mask]

sub     rsp, 16

movlpd  [rbp - 8], xmm1
movlpd  [rbp - 16], xmm0

vbroadcastsd    ymm6, [rbp - 8]
vbroadcastsd    ymm7, [rbp - 16]

cmp     rcx, 0
je      after_loop_denseToDenseAddAbsAVX_nocache_64_linux

start_denseToDenseAddAbsAVX_nocache_64_linux:

vmovapd  ymm0, [rdi] ; a
vmovapd  ymm1, [rsi] ; b
vmulpd   ymm1, ymm7  ; b = lambda * b
vaddpd   ymm0, ymm1  ; xmm0 = c = a + b
vmovapd  ymm2, ymm0
vandpd   ymm2, ymm5  ; xmm2 = |c|
vcmppd   ymm2, ymm6, 0xD
vandpd   ymm2, ymm0
vmovntpd  [rdx], ymm2

add     rdi, 32
add     rsi, 32
add     rdx, 32

loop    start_denseToDenseAddAbsAVX_nocache_64_linux

after_loop_denseToDenseAddAbsAVX_nocache_64_linux:

cmp     rax, 0
je      end_denseToDenseAddAbsAVX_nocache_64_linux

mov     rcx, rax

last_loop_denseToDenseAddAbsAVX_nocache_64_linux:

movlpd  xmm0, [rdi] ; a
movlpd  xmm1, [rsi] ; b
mulsd   xmm1, xmm7  ; b = lambda * b
addsd   xmm0, xmm1  ; xmm0 = c = a + b
movapd  xmm2, xmm0
andpd   xmm2, xmm5  ; xmm2 = |c|
cmpsd   xmm2, xmm6, 0xD
andpd   xmm2, xmm0
movlpd  [rdx], xmm2

add     rdi, 8
add     rsi, 8
add     rdx, 8

loop    last_loop_denseToDenseAddAbsAVX_nocache_64_linux

end_denseToDenseAddAbsAVX_nocache_64_linux:

mov     rsp, rbp
pop     rbp
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Dense to dense
;; Uses cache
;; AVX
;; Without tolerances
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _denseToDenseAddAVX_cache_64_linux
_denseToDenseAddAVX_cache_64_linux:

push    rbp
mov     rbp, rsp

; rdi: address1
; rsi: address2
; rdx: address3
; rcx: count
; xmm0: lambda

mov     rax, rcx
shr     rcx, 2
and     rax, 0x03

vmovupd  ymm5, [abs_mask]

sub     rsp, 8
movlpd  [rbp - 8], xmm0
vbroadcastsd    ymm7, [rbp - 8]

;shufpd  xmm0, xmm0, 00b
;movupd  xmm7, xmm0

cmp     rcx, 0
je      after_loop_denseToDenseAddAVX_cache_64_linux

start_denseToDenseAddAVX_cache_64_linux:

vmovapd  ymm0, [rdi] ; a
vmovapd  ymm1, ymm7
vmulpd   ymm1, [rsi] ; b
vaddpd   ymm0, ymm1  ; xmm0 = c = a + b
vmovapd  [rdx], ymm0

add     rdi, 32
add     rsi, 32
add     rdx, 32

loop    start_denseToDenseAddAVX_cache_64_linux

after_loop_denseToDenseAddAVX_cache_64_linux:

; todo: majd a maradekot lekezelni
cmp     rax, 0
je      end_denseToDenseAddAVX_cache_64_linux

mov     rcx, rax

last_loop_denseToDenseAddAVX_cache_64_linux:

movlpd  xmm0, [rdi] ; a
movapd  xmm1, xmm7
mulsd   xmm1, [rsi] ; b
addsd   xmm0, xmm1  ; xmm0 = c = a + b
movlpd  [rdx], xmm0

add     rdi, 8
add     rsi, 8
add     rdx, 8

loop    last_loop_denseToDenseAddAVX_cache_64_linux

end_denseToDenseAddAVX_cache_64_linux:

mov     rsp, rbp
pop     rbp
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Dense to dense
;; Without cache (for storing the result)
;; AVX
;; Without tolerances
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _denseToDenseAddAVX_nocache_64_linux
_denseToDenseAddAVX_nocache_64_linux:

push    rbp
mov     rbp, rsp

; rdi: address1
; rsi: address2
; rdx: address3
; rcx: count
; xmm0: lambda

mov     rax, rcx
shr     rcx, 2
and     rax, 0x03

vmovupd  ymm5, [abs_mask]

sub     rsp, 8
movlpd  [rbp - 8], xmm0
vbroadcastsd    ymm7, [rbp - 8]

;shufpd  xmm0, xmm0, 00b
;movupd  xmm7, xmm0

cmp     rcx, 0
je      after_loop_denseToDenseAddAVX_nocache_64_linux

start_denseToDenseAddAVX_nocache_64_linux:

vmovapd  ymm0, [rdi] ; a
vmovapd  ymm1, ymm7
vmulpd   ymm1, [rsi] ; b
vaddpd   ymm0, ymm1  ; xmm0 = c = a + b
vmovntpd  [rdx], ymm0

add     rdi, 32
add     rsi, 32
add     rdx, 32

loop    start_denseToDenseAddAVX_nocache_64_linux

after_loop_denseToDenseAddAVX_nocache_64_linux:

; todo: majd a maradekot lekezelni
cmp     rax, 0
je      end_denseToDenseAddAVX_nocache_64_linux

mov     rcx, rax

last_loop_denseToDenseAddAVX_nocache_64_linux:

movlpd  xmm0, [rdi] ; a
movapd  xmm1, xmm7
mulsd   xmm1, [rsi] ; b
addsd   xmm0, xmm1  ; xmm0 = c = a + b
movlpd  [rdx], xmm0

add     rdi, 8
add     rsi, 8
add     rdx, 8

loop    last_loop_denseToDenseAddAVX_nocache_64_linux

end_denseToDenseAddAVX_nocache_64_linux:

mov     rsp, rbp
pop     rbp
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Sparse to dense
;; Uses cache
;; SSE2
;; Absolute and relative tolerances
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _sparseToDenseAddAbsRelSSE2_cache_64_linux
_sparseToDenseAddAbsRelSSE2_cache_64_linux:

push    rbp
mov     rbp, rsp
push    rbx
push    r9

mov     rbx, 0

; rdi: address1 (dense)
; rsi: address2 (sparse)
; rdx: result address (dense)
; rcx: sparse count
; r8:  sparse index address
; xmm0: lambda
; xmm1: absTolerance
; xmm2: relTolerance

mov     rax, rcx
shr     rcx, 1
and     rax, 0x01

movupd  xmm5, [abs_mask]
shufpd  xmm1, xmm1, 00b
shufpd  xmm2, xmm2, 00b
movupd  xmm4, xmm2
movupd  xmm6, xmm1
shufpd  xmm0, xmm0, 00b
movupd  xmm7, xmm0

cmp     rcx, 0
je      after_loop_sparseToDenseAddAbsRelSSE2_cache_64_linux

start_sparseToDenseAddAbsRelSSE2_cache_64_linux:

;movapd  xmm0, [rdi] ; a

mov     ebx,  [r8]
movlpd  xmm0, [rdi + rbx * 8]
mov     r9,   rbx
mov     ebx,  [r8 + 4]
movhpd  xmm0, [rdi + rbx * 8]

movapd  xmm1, [rsi] ; b
mulpd   xmm1, xmm7  ; b = lambda * b
movapd  xmm2, xmm0
addpd   xmm2, xmm1  ; xmm2 = c = a + b
movapd  xmm3, xmm2  ; xmm3 = c
andpd   xmm3, xmm5  ; xmm3 = |c|
andpd   xmm0, xmm5  ; xmm0 = |a|
andpd   xmm1, xmm5  ; xmm1 = |b|
addpd   xmm0, xmm1  ; xmm0 = |a| + |b|
mulpd   xmm0, xmm4
cmppd   xmm0, xmm3, 1
; |a| + |b| * mul < |c|
;    c is unchanged
; else
;    c = 0.0
andpd   xmm2, xmm0
; abs tolerance
movapd  xmm0, xmm2  ; result in xmm2
andpd   xmm0, xmm5  ; absolute value of the result in xmm0
cmppd   xmm0, xmm6, 0xD ; absolute tolerance
andpd   xmm2, xmm0

movhpd  [rdx + rbx * 8], xmm0
mov     rbx, r9
movlpd  [rdx + rbx * 8], xmm0

;movapd  [rdx], xmm2

add     rdi, 16
add     rsi, 16
add     rdx, 16
add     r8, 8

loop    start_sparseToDenseAddAbsRelSSE2_cache_64_linux

after_loop_sparseToDenseAddAbsRelSSE2_cache_64_linux:

cmp     rax, 0
je      end_sparseToDenseAddAbsRelSSE2_cache_64_linux

;movlpd  xmm0, [rdi] ; a
mov     ebx,  [r8]
movlpd  xmm0, [rdi + rbx * 8]

movlpd  xmm1, [rsi] ; b
mulsd   xmm1, xmm7  ; b = lambda * b
movapd  xmm2, xmm0
addsd   xmm2, xmm1  ; xmm2 = c = a + b
movapd  xmm3, xmm2  ; xmm3 = c
andpd   xmm3, xmm5  ; xmm3 = |c|
andpd   xmm0, xmm5  ; xmm0 = |a|
andpd   xmm1, xmm5  ; xmm1 = |b|
addsd   xmm0, xmm1  ; xmm0 = |a| + |b|
mulsd   xmm0, xmm4
cmpsd   xmm0, xmm3, 1
; |a| + |b| * mul < |c|
;    c is unchanged
; else
;    c = 0.0
andpd   xmm2, xmm0
; abs tolerance
movapd  xmm0, xmm2  ; result in xmm2
andpd   xmm0, xmm5  ; absolute value of the result in xmm0
cmpsd   xmm0, xmm6, 0xD ; absolute tolerance
andpd   xmm2, xmm0
;movlpd  [rdx], xmm2
movlpd  [rdx + rbx * 8], xmm0

end_sparseToDenseAddAbsRelSSE2_cache_64_linux:

pop     r9
pop     rbx
mov     rsp, rbp
pop     rbp
ret
