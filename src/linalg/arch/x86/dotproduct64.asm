

SECTION .data

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Dense to sparse dot product, unstable,
; quick version,
; - SSE2
; - 64 bit
; - linux
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _denseToSparseDotProduct_unstable_SSE2_64_linux
_denseToSparseDotProduct_unstable_SSE2_64_linux:

push    rbp
mov     rbp, rsp

push    rcx

; rdi: dense address
; rsi: sparse address
; rdx: sparse index address
;

xorpd   xmm4, xmm4
xorpd   xmm5, xmm5
xorpd   xmm6, xmm6
xorpd   xmm7, xmm7

mov     rcx, rdx
shr     rcx, 3
and     rdx, 0x7

cmp     rcx, 1
jl      _denseToSparseDotProduct_unstable_SSE2_64_linux_loop_aa_after_loop1

align 16
_denseToSparseDotProduct_unstable_SSE2_64_linux_loop_aa:

movapd  xmm0, [rdi]
movapd  xmm1, [rdi+16]
movapd  xmm2, [rdi+32]
movapd  xmm3, [rdi+48]

mulpd   xmm0, [rsi]
mulpd   xmm1, [rsi+16]
mulpd   xmm2, [rsi+32]
mulpd   xmm3, [rsi+48]


addpd   xmm4, xmm0
addpd   xmm5, xmm1
addpd   xmm6, xmm2
addpd   xmm7, xmm3

add     rdi, 64
add     rsi, 64

loop    _denseToSparseDotProduct_unstable_SSE2_64_linux_loop_aa

_denseToSparseDotProduct_unstable_SSE2_64_linux_loop_aa_after_loop1:

mov     rcx, rdx
shr     rcx, 1
and     rdx, 1

cmp     rcx, 1
jl      _denseToSparseDotProduct_unstable_SSE2_64_linux_loop_aa_after_loop2

_denseToSparseDotProduct_unstable_SSE2_64_linux_loop_aa_loop2:

movapd  xmm0, [rdi]
mulpd   xmm0, [rsi]
addpd   xmm4, xmm0

add     rdi, 16
add     rsi, 16

loop    _denseToSparseDotProduct_unstable_SSE2_64_linux_loop_aa_loop2

_denseToSparseDotProduct_unstable_SSE2_64_linux_loop_aa_after_loop2:

cmp     rdx, 0
je      _denseToSparseDotProduct_unstable_SSE2_64_linux_end

movsd   xmm0, [rdi]
mulsd   xmm0, [rsi]
addsd   xmm4, xmm0

_denseToSparseDotProduct_unstable_SSE2_64_linux_end:

addpd   xmm4, xmm5
addpd   xmm4, xmm6
addpd   xmm4, xmm7

haddpd  xmm4, xmm4

; result
movsd   xmm0, xmm4

pop     rcx

mov     rsp, rbp
pop     rbp
ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Dense to dense dot product, unstable,
; quick version,
; - SSE2
; - 64 bit
; - linux
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _denseToDenseDotProduct_unstable_SSE2_64_linux
_denseToDenseDotProduct_unstable_SSE2_64_linux:

push    rbp
mov     rbp, rsp

push    rcx

; rdi: address1
; rsi: address2
; rdx: count

xorpd   xmm4, xmm4
xorpd   xmm5, xmm5
xorpd   xmm6, xmm6
xorpd   xmm7, xmm7

mov     rcx, rdx
shr     rcx, 3
and     rdx, 0x7

cmp     rcx, 1
jl      _denseToDenseDotProduct_unstable_SSE2_64_linux_loop_aa_after_loop1

align 16
_denseToDenseDotProduct_unstable_SSE2_64_linux_loop_aa:

movapd  xmm0, [rdi]
movapd  xmm1, [rdi+16]
movapd  xmm2, [rdi+32]
movapd  xmm3, [rdi+48]

mulpd   xmm0, [rsi]
mulpd   xmm1, [rsi+16]
mulpd   xmm2, [rsi+32]
mulpd   xmm3, [rsi+48]


addpd   xmm4, xmm0
addpd   xmm5, xmm1
addpd   xmm6, xmm2
addpd   xmm7, xmm3

add     rdi, 64
add     rsi, 64

loop    _denseToDenseDotProduct_unstable_SSE2_64_linux_loop_aa

_denseToDenseDotProduct_unstable_SSE2_64_linux_loop_aa_after_loop1:

mov     rcx, rdx
shr     rcx, 1
and     rdx, 1

cmp     rcx, 1
jl      _denseToDenseDotProduct_unstable_SSE2_64_linux_loop_aa_after_loop2

_denseToDenseDotProduct_unstable_SSE2_64_linux_loop_aa_loop2:

movapd  xmm0, [rdi]
mulpd   xmm0, [rsi]
addpd   xmm4, xmm0

add     rdi, 16
add     rsi, 16

loop    _denseToDenseDotProduct_unstable_SSE2_64_linux_loop_aa_loop2

_denseToDenseDotProduct_unstable_SSE2_64_linux_loop_aa_after_loop2:

cmp     rdx, 0
je      _denseToDenseDotProduct_unstable_SSE2_64_linux_end

movsd   xmm0, [rdi]
mulsd   xmm0, [rsi]
addsd   xmm4, xmm0

_denseToDenseDotProduct_unstable_SSE2_64_linux_end:

addpd   xmm4, xmm5
addpd   xmm4, xmm6
addpd   xmm4, xmm7

haddpd  xmm4, xmm4

; result
movsd   xmm0, xmm4

pop     rcx

mov     rsp, rbp
pop     rbp
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Dense to dense dot product, unstable,
; quick version,
; - AVX
; - 64 bit
; - linux
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _denseToDenseDotProduct_unstable_AVX_64_linux
_denseToDenseDotProduct_unstable_AVX_64_linux:

push    rbp
mov     rbp, rsp

push    rcx

; rdi: address1
; rsi: address2
; rdx: count

vxorpd   ymm4, ymm4
vxorpd   ymm5, ymm5
vxorpd   ymm6, ymm6
vxorpd   ymm7, ymm7

mov     rcx, rdx
shr     rcx, 4
and     rdx, 0xF

cmp     rcx, 1
jl      _denseToDenseDotProduct_unstable_AVX_64_linux_loop_aa_after_loop1

align 16
_denseToDenseDotProduct_unstable_AVX_64_linux_loop_aa:

vmovapd  ymm0, [rdi]
vmovapd  ymm1, [rdi+32]
vmovapd  ymm2, [rdi+64]
vmovapd  ymm3, [rdi+96]

vmulpd   ymm0, [rsi]
vmulpd   ymm1, [rsi+32]
vmulpd   ymm2, [rsi+64]
vmulpd   ymm3, [rsi+96]

vaddpd   ymm4, ymm0
vaddpd   ymm5, ymm1
vaddpd   ymm6, ymm2
vaddpd   ymm7, ymm3

add     rdi, 128
add     rsi, 128

loop    _denseToDenseDotProduct_unstable_AVX_64_linux_loop_aa

_denseToDenseDotProduct_unstable_AVX_64_linux_loop_aa_after_loop1:

mov     rcx, rdx
shr     rcx, 2
and     rdx, 0x3

cmp     rcx, 1
jl      _denseToDenseDotProduct_unstable_AVX_64_linux_loop_aa_after_loop2

_denseToDenseDotProduct_unstable_AVX_64_linux_loop_aa_loop2:

vmovapd  ymm0, [rdi]
vmulpd   ymm0, [rsi]
vaddpd   ymm4, ymm0

add     rdi, 32
add     rsi, 32

loop    _denseToDenseDotProduct_unstable_AVX_64_linux_loop_aa_loop2

_denseToDenseDotProduct_unstable_AVX_64_linux_loop_aa_after_loop2:

cmp     rdx, 0
je      _denseToDenseDotProduct_unstable_AVX_64_linux_end

mov     rcx, rdx

_denseToDenseDotProduct_unstable_AVX_64_linux_loop_aa_loop3:

movsd   xmm0, [rdi]
mulsd   xmm0, [rsi]
addsd   xmm4, xmm0

add     rdi, 8
add     rsi, 8

loop    _denseToDenseDotProduct_unstable_AVX_64_linux_loop_aa_loop3

_denseToDenseDotProduct_unstable_AVX_64_linux_end:

vaddpd   ymm4, ymm5
vaddpd   ymm4, ymm6
vaddpd   ymm4, ymm7

vhaddpd  ymm4, ymm4
vextractf128 xmm3, ymm4, 1
addsd   xmm4, xmm3


; result
movsd   xmm0, xmm4

pop     rcx

mov     rsp, rbp
pop     rbp
ret



global _denseToDenseDotProductSSE2_64_linux
_denseToDenseDotProductSSE2_64_linux:

push    rbp
mov     rbp, rsp

; rdi: address1
; rsi: address2
; rdx: count

; 2 doubles at the same time

mov     rax, rdx
shr     rdx, 1
and     rax, 0x01
mov     rcx, rdx

xorpd   xmm4, xmm4
xorpd   xmm1, xmm1
xorpd   xmm2, xmm2

start_denseToDenseDotProductSSE2_64_linux_loop:

movapd  xmm0, [rdi]
mulpd   xmm0, [rsi]
movapd  xmm5, xmm0
movapd  xmm3, xmm0
cmppd   xmm3, xmm4, 1
andpd   xmm5, xmm3
addpd   xmm1, xmm5
andnpd  xmm3, xmm0
addpd   xmm2, xmm3

add     rdi, 16
add     rsi, 16

loop    start_denseToDenseDotProductSSE2_64_linux_loop

cmp     rax, 0
jz      denseToDenseDotProductSSE2_64_linux_loop_end

xorpd   xmm0, xmm0
xorpd   xmm3, xmm3
movsd   xmm0, [rdi]
mulsd   xmm0, [rsi]
movsd   xmm5, xmm0
movsd   xmm3, xmm0
cmpsd   xmm3, xmm4, 1
andpd   xmm5, xmm3
addsd   xmm1, xmm5
andnpd  xmm3, xmm0
addsd   xmm2, xmm3

denseToDenseDotProductSSE2_64_linux_loop_end:

haddpd  xmm1, xmm1
haddpd  xmm2, xmm2
addsd   xmm1, xmm2

movsd   xmm0, xmm1


mov     rsp, rbp
pop     rbp
ret

; ---------------------------------------------------

global _denseToDenseDotProductSSE2_64_windows
_denseToDenseDotProductSSE2_64_windows:

push    rbp
mov     rbp, rsp

; rcx: address1 -> rbx
; rdx: address2
; r8:  count

; 2 doubles at the same time

mov     rbx, rcx
mov     rax, r8
shr     r8, 1
and     rax, 0x01
mov     rcx, r8

xorpd   xmm4, xmm4
xorpd   xmm1, xmm1
xorpd   xmm2, xmm2

start_denseToDenseDotProductSSE2_64_windows_loop:

movapd  xmm0, [rbx]
mulpd   xmm0, [rdx]
movapd  xmm5, xmm0
movapd  xmm3, xmm0
cmppd   xmm3, xmm4, 1
andpd   xmm5, xmm3
addpd   xmm1, xmm5
andnpd  xmm3, xmm0
addpd   xmm2, xmm3

add     rbx, 16
add     rdx, 16

loop    start_denseToDenseDotProductSSE2_64_windows_loop

cmp     rax, 0
jz      denseToDenseDotProductSSE2_64_windows_loop_end

xorpd   xmm0, xmm0
xorpd   xmm3, xmm3
movsd   xmm0, [rbx]
mulsd   xmm0, [rdx]
movsd   xmm5, xmm0
movsd   xmm3, xmm0
cmpsd   xmm3, xmm4, 1
andpd   xmm5, xmm3
addsd   xmm1, xmm5
andnpd  xmm3, xmm0
addsd   xmm2, xmm3

denseToDenseDotProductSSE2_64_windows_loop_end:

haddpd  xmm1, xmm1
haddpd  xmm2, xmm2
addsd   xmm1, xmm2

movsd   xmm0, xmm1

mov     rsp, rbp
pop     rbp
ret
