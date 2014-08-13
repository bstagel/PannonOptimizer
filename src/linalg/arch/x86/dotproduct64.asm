
SECTION .data

align 32
zero_avx:
db      0x0000000000000000000000000000000000000000000000000000000000000000

align 32
four_doubles:
dq      0, 0, 0, 0, 0

SECTION .text


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Dense to sparse dot product, unstable,
; quick version,
; - AVX
; - 64 bit
; - linux
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _denseToSparseDotProduct_unstable_AVX_64_linux
_denseToSparseDotProduct_unstable_AVX_64_linux:

push    rbp
mov     rbp, rsp

push    rax
push    rbx

vzeroall

movapd xmm0, [rsi]
mov     eax, [rdx]
movlpd xmm8, [rdi+rax*8]
mov     eax, [rdx+4]
movhpd xmm8, [rdi+rax*8]

mov     eax, [rdx+8]
movlpd xmm8, [rdi+rax*8]
mov     eax, [rdx+12]
movhpd xmm8, [rdi+rax*8]

;vinsertf128 ymm8, ymm8, xmm12, 1

mulpd   xmm0, xmm8
addpd   xmm4, xmm0


pop     rbx
pop     rax

mov     rsp, rbp
pop     rbp
ret

; rdi: dense address
; rsi: sparse address
; rdx: sparse index address
; rcx: sparse count
; rbx: sparse count

;vzeroall

mov     rbx,  rcx

;vmovapd     ymm4, [zero_avx]
;vxorpd   ymm4, ymm4
;vxorpd   ymm5, ymm5
;vxorpd   ymm6, ymm6
;vxorpd   ymm7, ymm7
;vxorpd   ymm15, ymm15

cmp     rcx, 16
jge     _denseToSparseDotProduct_unstable_AVX_64_linux_more_data

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

shr     rcx, 2
and     rbx, 0x03

cmp     rcx, 1
jl      _denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_after_loop_short_1

align 16
_denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_loop_short_1:

;vmovapd ymm0, [rsi]
;mov     eax, [rdx]
;vmovlpd xmm8, [rdi+rax*8]
;mov     eax, [rdx+4]
;vmovhpd xmm8, [rdi+rax*8]
;mov     eax, [rdx+8]
;vmovlpd xmm12, [rdi+rax*8]
;mov     eax, [rdx+12]
;vmovhpd xmm12, [rdi+rax*8]
;vinsertf128 ymm8, ymm8, xmm12, 1

;vmulpd   ymm0, ymm8
;vaddpd   ymm4, ymm0


add     rsi, 32
add     rdx, 16

loop    _denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_loop_short_1

_denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_after_loop_short_1:

cmp     rbx, 0
je      _denseToSparseDotProduct_unstable_AVX_64_linux_short_end

mov     rcx, rbx

;vxorpd  xmm15, xmm15

_denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_short_loop_2:

;movsd  xmm13, [rsi]
;mov     eax, [rdx]
;movlpd xmm14, [rdi+rax*8]
;mulsd  xmm13, xmm14
;addsd  xmm15, xmm13 ; vaddsd xmm4, xmm0 zeroes the highest 128 bits :(


add     rsi, 8
add     rdx, 4

loop _denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_short_loop_2

_denseToSparseDotProduct_unstable_AVX_64_linux_short_end:

;vmovdqa  [four_doubles], ymm4

;sub     rsp, 40 ; reserve space for 5*8 bytes
;vmovdqu   [rsp], ymm4
;movsd    [rsp+32], xmm15

vaddpd   ymm0, ymm4, ymm15
;vaddpd   ymm4, ymm5
;vaddpd   ymm4, ymm6
;vaddpd   ymm4, ymm7

;vhaddpd  ymm4, ymm4
;vextractf128 xmm3, ymm4, 1
;vaddsd   xmm4, xmm3

; result:D
;vmovsd   xmm0, xmm4

;add     rsp, 40

pop     rbx
pop     rax

mov     rsp, rbp
pop     rbp
ret



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;




_denseToSparseDotProduct_unstable_AVX_64_linux_more_data:

shr     rcx, 4
and     rbx, 0xF

xor     rax, rax

cmp     rcx, 1
jl      _denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_after_loop1

align 16
_denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa:
vmovapd ymm0, [rsi]
mov     eax, [rdx]
vmovlpd xmm8, [rdi+rax*8]
mov     eax, [rdx+4]
vmovhpd xmm8, [rdi+rax*8]
mov     eax, [rdx+8]
vmovlpd xmm12, [rdi+rax*8]
mov     eax, [rdx+12]
vmovhpd xmm12, [rdi+rax*8]
vinsertf128 ymm8, ymm8, xmm12, 1

vmovapd ymm1, [rsi+32]
mov     eax, [rdx+16]
vmovlpd xmm9, [rdi+rax*8]
mov     eax, [rdx+20]
vmovhpd xmm9, [rdi+rax*8]
mov     eax, [rdx+24]
vmovlpd xmm12, [rdi+rax*8]
mov     eax, [rdx+28]
vmovhpd xmm12, [rdi+rax*8]
vinsertf128 ymm9, ymm9, xmm12, 1

vmovapd ymm2, [rsi+64]
mov     eax, [rdx+32]
vmovlpd xmm10, [rdi+rax*8]
mov     eax, [rdx+36]
vmovhpd xmm10, [rdi+rax*8]
mov     eax, [rdx+40]
vmovlpd xmm12, [rdi+rax*8]
mov     eax, [rdx+44]
vmovhpd xmm12, [rdi+rax*8]
vinsertf128 ymm10, ymm10, xmm12, 1

vmovapd ymm3, [rsi+96]
mov     eax, [rdx+48]
vmovlpd xmm11, [rdi+rax*8]
mov     eax, [rdx+52]
vmovhpd xmm11, [rdi+rax*8]
mov     eax, [rdx+56]
vmovlpd xmm12, [rdi+rax*8]
mov     eax, [rdx+60]
vmovhpd xmm12, [rdi+rax*8]
vinsertf128 ymm11, ymm11, xmm12, 1

vmulpd   ymm0, ymm8
vmulpd   ymm1, ymm9
vmulpd   ymm2, ymm10
vmulpd   ymm3, ymm11

vaddpd   ymm4, ymm0
vaddpd   ymm4, ymm1
vaddpd   ymm4, ymm2
vaddpd   ymm4, ymm3

add     rsi, 128
add     rdx, 64

dec     ecx
jnz    _denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa

_denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_after_loop1:

mov     rcx, rbx
shr     rcx, 2
and     rbx, 0x03

cmp     rcx, 1
jl      _denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_after_loop2

align 16
_denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_loop2:

vmovapd ymm0, [rsi]
mov     eax, [rdx]
vmovlpd xmm8, [rdi+rax*8]
mov     eax, [rdx+4]
vmovhpd xmm8, [rdi+rax*8]
mov     eax, [rdx+8]
vmovlpd xmm12, [rdi+rax*8]
mov     eax, [rdx+12]
vmovhpd xmm12, [rdi+rax*8]
vinsertf128 ymm8, ymm8, xmm12, 1

vmulpd   ymm0, ymm8
vaddpd   ymm4, ymm0

add     rsi, 32
add     rdx, 16

loop    _denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_loop2

_denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_after_loop2:

cmp     rbx, 0
je      _denseToSparseDotProduct_unstable_AVX_64_linux_end

mov     rcx, rbx

vxorpd  xmm15, xmm15

_denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_loop3:

vmovsd  xmm13, [rsi]
mov     eax, [rdx]
vmovlpd xmm14, [rdi+rax*8]
vmulsd  xmm13, xmm14
vaddsd  xmm15, xmm13 ; vaddsd xmm4, xmm0 zeroes the highest 128 bits :(

;movsd  xmm0, [rsi]
;mov     eax, [rdx]
;movlpd xmm8, [rdi+rax*8]
;mulsd  xmm0, xmm8
;addsd  xmm4, xmm0 ; vaddsd xmm4, xmm0 zeroes the highest 128 bits :(


add     rsi, 8
add     rdx, 4

loop _denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_loop3

_denseToSparseDotProduct_unstable_AVX_64_linux_end:

vaddpd   ymm4, ymm15
vaddpd   ymm4, ymm5
vaddpd   ymm4, ymm6
vaddpd   ymm4, ymm7

vhaddpd  ymm4, ymm4
vextractf128 xmm3, ymm4, 1
vaddsd   xmm4, xmm3

; result
vmovsd   xmm0, xmm4


pop     rbx
pop     rax

mov     rsp, rbp
pop     rbp
ret



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

push    rax
push    rbx

; rdi: dense address
; rsi: sparse address
; rdx: sparse index address
; rcx: sparse count
; rbx: sparse count

mov     rbx,  rcx

xorpd   xmm4, xmm4
xorpd   xmm5, xmm5
xorpd   xmm6, xmm6
xorpd   xmm7, xmm7

shr     rcx, 3
and     rbx, 0x7

cmp     rcx, 1
jl      _denseToSparseDotProduct_unstable_SSE2_64_linux_loop_aa_after_loop1

xor     rax, rax

align 16
_denseToSparseDotProduct_unstable_SSE2_64_linux_loop_aa:
movapd  xmm0, [rsi]
mov     eax, [rdx]
movlpd  xmm8, [rdi+rax*8]
mov     eax, [rdx+4]
movhpd  xmm8, [rdi+rax*8]


movapd  xmm1, [rsi+16]
mov     eax, [rdx+8]
movlpd  xmm9, [rdi+rax*8]
mov     eax, [rdx+12]
movhpd  xmm9, [rdi+rax*8]

movapd  xmm2, [rsi+32]
mov     eax, [rdx+16]
movlpd  xmm10, [rdi+rax*8]
mov     eax, [rdx+20]
movhpd  xmm10, [rdi+rax*8]

movapd  xmm3, [rsi+48]
mov     eax, [rdx+24]
movlpd  xmm11, [rdi+rax*8]
mov     eax, [rdx+28]
movhpd  xmm11, [rdi+rax*8]

mulpd   xmm0, xmm8
mulpd   xmm1, xmm9
mulpd   xmm2, xmm10
mulpd   xmm3, xmm11

addpd   xmm4, xmm0
addpd   xmm4, xmm1
addpd   xmm4, xmm2
addpd   xmm4, xmm3

add     rsi, 64
add     rdx, 32

dec     ecx
jnz    _denseToSparseDotProduct_unstable_SSE2_64_linux_loop_aa

_denseToSparseDotProduct_unstable_SSE2_64_linux_loop_aa_after_loop1:


mov     rcx, rbx
shr     rcx, 1
and     rbx, 1

cmp     rcx, 1
jl      _denseToSparseDotProduct_unstable_SSE2_64_linux_loop_aa_after_loop2

_denseToSparseDotProduct_unstable_SSE2_64_linux_loop_aa_loop2:

movapd  xmm0, [rsi]
mov     eax, [rdx]
movlpd  xmm8, [rdi+rax*8]
mov     eax, [rdx+4]
movhpd  xmm8, [rdi+rax*8]

mulpd   xmm0, xmm8
addpd   xmm4, xmm0

add     rsi, 16
add     rdx, 8

loop    _denseToSparseDotProduct_unstable_SSE2_64_linux_loop_aa_loop2

_denseToSparseDotProduct_unstable_SSE2_64_linux_loop_aa_after_loop2:

cmp     rbx, 0
je      _denseToSparseDotProduct_unstable_SSE2_64_linux_end

movsd   xmm0, [rsi]
mov     eax, [rdx]
movlpd  xmm8, [rdi+rax*8]
mulsd   xmm0, xmm8
addsd   xmm4, xmm0

_denseToSparseDotProduct_unstable_SSE2_64_linux_end:

addpd   xmm4, xmm5
addpd   xmm4, xmm6
addpd   xmm4, xmm7

haddpd  xmm4, xmm4

; result
movsd   xmm0, xmm4

pop     rbx
pop     rax

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
vaddsd   xmm4, xmm3


; result
vmovsd   xmm0, xmm4

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
