
SECTION .data

align 32
zero_avx:
db      0x0000000000000000000000000000000000000000000000000000000000000000

align 32
four_doubles:
dq      0, 0, 0, 0, 0

mxcsr:
db      0, 0, 0, 0

SECTION .text


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Dense to sparse dot product, stable,
; quick version,
; - SSE2
; - 64 bit
; - linux
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _denseToSparseDotProduct_stable_SSE2_64_linux
_denseToSparseDotProduct_stable_SSE2_64_linux:

push    rbp
mov     rbp, rsp

push    rax
push    rbx

; rdi: dense address
; rsi: sparse address
; rdx: sparse index address
; rcx: sparse count
; r8: negPtr
; rbx: sparse count

mov     rbx,  rcx

; positive results
xorpd   xmm4, xmm4
; negative results
xorpd   xmm5, xmm5
; zero
xorpd   xmm6, xmm6


shr     rcx, 3
and     rbx, 0x7

cmp     rcx, 1
jl      _denseToSparseDotProduct_stable_SSE2_64_linux_loop_aa_after_loop1

xor     rax, rax

align 16
_denseToSparseDotProduct_stable_SSE2_64_linux_loop_aa:
movapd  xmm0, [rsi]
mov     eax, [rdx]
movlpd  xmm8, [rdi+rax*8]
mov     eax, [rdx+4]
movhpd  xmm8, [rdi+rax*8]

mulpd   xmm0, xmm8
; xmm0-t szetvalasztani xmm4-re vagy xmm5-re
movapd  xmm7, xmm0
movapd  xmm12, xmm0
cmppd   xmm7, xmm6, 1 ; xmm7-ben lesz a maszk
andpd   xmm0, xmm7  ; a maszkal osszeeseljuk az eredmenyt
haddpd  xmm0, xmm0
addsd   xmm4, xmm0
andnpd  xmm7, xmm12 ; xmm7-ben meg mindig maszk van, amit most felulirunk
haddpd  xmm7, xmm7
addsd   xmm5, xmm7


movapd  xmm1, [rsi+16]
mov     eax, [rdx+8]
movlpd  xmm9, [rdi+rax*8]
mov     eax, [rdx+12]
movhpd  xmm9, [rdi+rax*8]

mulpd   xmm1, xmm9
; xmm1
movapd  xmm7, xmm1
movapd  xmm12, xmm1
cmppd   xmm7, xmm6, 1 ; xmm7-ben lesz a maszk
andpd   xmm1, xmm7  ; a maszkal osszeeseljuk az eredmenyt
haddpd  xmm1, xmm1
addsd   xmm4, xmm1
andnpd  xmm7, xmm12 ; xmm7-ben meg mindig maszk van, amit most felulirunk
haddpd  xmm7, xmm7
addsd   xmm5, xmm7

movapd  xmm2, [rsi+32]
mov     eax, [rdx+16]
movlpd  xmm10, [rdi+rax*8]
mov     eax, [rdx+20]
movhpd  xmm10, [rdi+rax*8]

mulpd   xmm2, xmm10
; xmm2
movapd  xmm7, xmm2
movapd  xmm12, xmm2
cmppd   xmm7, xmm6, 1 ; xmm7-ben lesz a maszk
andpd   xmm2, xmm7  ; a maszkal osszeeseljuk az eredmenyt
haddpd  xmm2, xmm2
addsd   xmm4, xmm2
andnpd  xmm7, xmm12 ; xmm7-ben meg mindig maszk van, amit most felulirunk
haddpd  xmm7, xmm7
addsd   xmm5, xmm7


movapd  xmm3, [rsi+48]
mov     eax, [rdx+24]
movlpd  xmm11, [rdi+rax*8]
mov     eax, [rdx+28]
movhpd  xmm11, [rdi+rax*8]

mulpd   xmm3, xmm11
; xmm3
movapd  xmm7, xmm3
movapd  xmm12, xmm3
cmppd   xmm7, xmm6, 1 ; xmm7-ben lesz a maszk
andpd   xmm3, xmm7  ; a maszkal osszeeseljuk az eredmenyt
haddpd  xmm3, xmm3
addsd   xmm4, xmm3
andnpd  xmm7, xmm12 ; xmm7-ben meg mindig maszk van, amit most felulirunk
haddpd  xmm7, xmm7
addsd   xmm5, xmm7


;mulpd   xmm0, xmm8
;mulpd   xmm1, xmm9
;mulpd   xmm2, xmm10
;mulpd   xmm3, xmm11


;addpd   xmm4, xmm0
;addpd   xmm4, xmm1
;addpd   xmm4, xmm2
;addpd   xmm4, xmm3

add     rsi, 64
add     rdx, 32

dec     ecx
jnz    _denseToSparseDotProduct_stable_SSE2_64_linux_loop_aa

_denseToSparseDotProduct_stable_SSE2_64_linux_loop_aa_after_loop1:


mov     rcx, rbx
shr     rcx, 1
and     rbx, 1

cmp     rcx, 1
jl      _denseToSparseDotProduct_stable_SSE2_64_linux_loop_aa_after_loop2

_denseToSparseDotProduct_stable_SSE2_64_linux_loop_aa_loop2:

movapd  xmm0, [rsi]
mov     eax, [rdx]
movlpd  xmm8, [rdi+rax*8]
mov     eax, [rdx+4]
movhpd  xmm8, [rdi+rax*8]

mulpd   xmm0, xmm8
; xmm0-t szetvalasztani xmm4-re vagy xmm5-re
movapd  xmm7, xmm0
movapd  xmm12, xmm0
cmppd   xmm7, xmm6, 1 ; xmm7-ben lesz a maszk
andpd   xmm0, xmm7  ; a maszkal osszeeseljuk az eredmenyt
haddpd  xmm0, xmm0
addsd   xmm4, xmm0
andnpd  xmm7, xmm12 ; xmm7-ben meg mindig maszk van, amit most felulirunk
haddpd  xmm7, xmm7
addsd   xmm5, xmm7

;mulpd   xmm0, xmm8
;addpd   xmm4, xmm0

add     rsi, 16
add     rdx, 8

loop    _denseToSparseDotProduct_stable_SSE2_64_linux_loop_aa_loop2

_denseToSparseDotProduct_stable_SSE2_64_linux_loop_aa_after_loop2:

cmp     rbx, 0
je      _denseToSparseDotProduct_stable_SSE2_64_linux_end

movsd   xmm0, [rsi]
mov     eax, [rdx]
movlpd  xmm8, [rdi+rax*8]
;mulsd   xmm0, xmm8
;addsd   xmm4, xmm0

mulsd   xmm0, xmm8
; xmm0-t szetvalasztani xmm4-re vagy xmm5-re
movsd  xmm7, xmm0
movsd  xmm12, xmm0
cmpsd   xmm7, xmm6, 1 ; xmm7-ben lesz a maszk
andpd   xmm0, xmm7  ; a maszkal osszeeseljuk az eredmenyt
addsd   xmm4, xmm0
andnpd  xmm7, xmm12 ; xmm7-ben meg mindig maszk van, amit most felulirunk
addsd   xmm5, xmm7


_denseToSparseDotProduct_stable_SSE2_64_linux_end:

;addpd   xmm4, xmm5
;addpd   xmm4, xmm6
;addpd   xmm4, xmm7

;haddpd  xmm4, xmm4
;haddpd  xmm5, xmm5

movsd   [r8], xmm5
;addsd   xmm4, xmm5
; result
movsd   xmm0, xmm4

pop     rbx
pop     rax

mov     rsp, rbp
pop     rbp
ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Dense to sparse dot product, stable,
; quick version,
; - AVX
; - 64 bit
; - linux
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;1000 1000 0000 0000
global _denseToSparseDotProduct_stable_AVX_64_linux
_denseToSparseDotProduct_stable_AVX_64_linux:

push    rbp
mov     rbp, rsp

push    rax
push    rbx


; rdi: dense address
; rsi: sparse address
; rdx: sparse index address
; rcx: sparse count
; r8: neg ptr
; rbx: sparse count

vzeroall

mov     rbx,  rcx

cmp     rcx, 16
jge     _denseToSparseDotProduct_stable_AVX_64_linux_more_data

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

shr     rcx, 2
and     rbx, 0x03

cmp     rcx, 1
jl      _denseToSparseDotProduct_stable_AVX_64_linux_loop_aa_after_loop_short_1

align 16
_denseToSparseDotProduct_stable_AVX_64_linux_loop_aa_loop_short_1:

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
vcmppd   ymm7, ymm6, ymm0, 1
vandpd   ymm8, ymm7, ymm0
vandnpd  ymm7, ymm0
vaddpd   ymm4, ymm8
vaddpd   ymm5, ymm7
;vhaddpd  ymm8, ymm8
;vhaddpd  ymm7, ymm7
;vaddpd   ymm4, ymm0

add     rsi, 32
add     rdx, 16

loop    _denseToSparseDotProduct_stable_AVX_64_linux_loop_aa_loop_short_1

_denseToSparseDotProduct_stable_AVX_64_linux_loop_aa_after_loop_short_1:

cmp     rbx, 0
je      _denseToSparseDotProduct_stable_AVX_64_linux_short_end

mov     rcx, rbx

_denseToSparseDotProduct_stable_AVX_64_linux_loop_aa_short_loop_2:

vmovsd  xmm13, [rsi]
mov     eax, [rdx]
vmovlpd xmm1, [rdi+rax*8]
vmulsd  xmm13, xmm1

vcmpsd   xmm7, xmm6, xmm13, 1
vandpd   xmm8, xmm7, xmm13
vandnpd  xmm7, xmm13
vaddsd   xmm14, xmm8
vaddsd   xmm15, xmm7


;vaddsd  xmm15, xmm13 ; vaddsd xmm4, xmm0 zeroes the highest 128 bits :(

add     rsi, 8
add     rdx, 4

loop _denseToSparseDotProduct_stable_AVX_64_linux_loop_aa_short_loop_2

_denseToSparseDotProduct_stable_AVX_64_linux_short_end:


vaddpd   ymm4, ymm14

vhaddpd  ymm4, ymm4
vextractf128 xmm3, ymm4, 1
vaddsd   xmm4, xmm3

vaddpd   ymm5, ymm15

vhaddpd  ymm5, ymm5
vextractf128 xmm3, ymm5, 1
vaddsd   xmm5, xmm3
vmovsd   [r8], xmm5
vmovsd   xmm0, xmm4


pop     rbx
pop     rax

vzeroupper

mov     rsp, rbp
pop     rbp
ret

; TODO: stabilla tenni a kesobbi reszt is
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


_denseToSparseDotProduct_stable_AVX_64_linux_more_data:

shr     rcx, 4
and     rbx, 0xF

xor     rax, rax

cmp     rcx, 1
jl      _denseToSparseDotProduct_stable_AVX_64_linux_loop_aa_after_loop1

align 16
_denseToSparseDotProduct_stable_AVX_64_linux_loop_aa:
vmovapd ymm0, [rsi]
vmovapd ymm1, [rsi+32]
vmovapd ymm2, [rsi+64]
vmovapd ymm3, [rsi+96]

mov     eax, [rdx]
vmovlpd xmm8, [rdi+rax*8]
mov     eax, [rdx+4]
vmovhpd xmm8, [rdi+rax*8]
mov     eax, [rdx+8]
vmovlpd xmm12, [rdi+rax*8]
mov     eax, [rdx+12]
vmovhpd xmm12, [rdi+rax*8]
vinsertf128 ymm8, ymm8, xmm12, 1

mov     eax, [rdx+16]
vmovlpd xmm9, [rdi+rax*8]
mov     eax, [rdx+20]
vmovhpd xmm9, [rdi+rax*8]
mov     eax, [rdx+24]
vmovlpd xmm12, [rdi+rax*8]
mov     eax, [rdx+28]
vmovhpd xmm12, [rdi+rax*8]
vinsertf128 ymm9, ymm9, xmm12, 1

mov     eax, [rdx+32]
vmovlpd xmm10, [rdi+rax*8]
mov     eax, [rdx+36]
vmovhpd xmm10, [rdi+rax*8]
mov     eax, [rdx+40]
vmovlpd xmm12, [rdi+rax*8]
mov     eax, [rdx+44]
vmovhpd xmm12, [rdi+rax*8]
vinsertf128 ymm10, ymm10, xmm12, 1

mov     eax, [rdx+48]
vmovlpd xmm11, [rdi+rax*8]
mov     eax, [rdx+52]
vmovhpd xmm11, [rdi+rax*8]
mov     eax, [rdx+56]
vmovlpd xmm12, [rdi+rax*8]
mov     eax, [rdx+60]
vmovhpd xmm12, [rdi+rax*8]
vinsertf128 ymm11, ymm11, xmm12, 1

prefetcht0  [rsi+256]
prefetcht0  [rdx+128]

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
jnz    _denseToSparseDotProduct_stable_AVX_64_linux_loop_aa

_denseToSparseDotProduct_stable_AVX_64_linux_loop_aa_after_loop1:

mov     rcx, rbx
shr     rcx, 2
and     rbx, 0x03

cmp     rcx, 1
jl      _denseToSparseDotProduct_stable_AVX_64_linux_loop_aa_after_loop2

align 16
_denseToSparseDotProduct_stable_AVX_64_linux_loop_aa_loop2:

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

loop    _denseToSparseDotProduct_stable_AVX_64_linux_loop_aa_loop2

_denseToSparseDotProduct_stable_AVX_64_linux_loop_aa_after_loop2:

cmp     rbx, 0
je      _denseToSparseDotProduct_stable_AVX_64_linux_end

mov     rcx, rbx

vxorpd  xmm15, xmm15

_denseToSparseDotProduct_stable_AVX_64_linux_loop_aa_loop3:

vmovsd  xmm13, [rsi]
mov     eax, [rdx]
vmovlpd xmm14, [rdi+rax*8]
vmulsd  xmm13, xmm14
vaddsd  xmm15, xmm13 ; vaddsd xmm4, xmm0 zeroes the highest 128 bits :(

add     rsi, 8
add     rdx, 4

loop _denseToSparseDotProduct_stable_AVX_64_linux_loop_aa_loop3

_denseToSparseDotProduct_stable_AVX_64_linux_end:

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

vzeroupper

mov     rsp, rbp
pop     rbp
ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Dense to sparse dot product, unstable,
; quick version,
; - AVX
; - 64 bit
; - linux
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;1000 1000 0000 0000
global _denseToSparseDotProduct_unstable_AVX_64_linux
_denseToSparseDotProduct_unstable_AVX_64_linux:

push    rbp
mov     rbp, rsp

push    rax
push    rbx


; rdi: dense address
; rsi: sparse address
; rdx: sparse index address
; rcx: sparse count
; rbx: sparse count

vzeroall

mov     rbx,  rcx

cmp     rcx, 16
jge     _denseToSparseDotProduct_unstable_AVX_64_linux_more_data

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

shr     rcx, 2
and     rbx, 0x03

cmp     rcx, 1
jl      _denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_after_loop_short_1

align 16
_denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_loop_short_1:

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

loop    _denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_loop_short_1

_denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_after_loop_short_1:

cmp     rbx, 0
je      _denseToSparseDotProduct_unstable_AVX_64_linux_short_end

mov     rcx, rbx

_denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_short_loop_2:

vmovsd  xmm13, [rsi]
mov     eax, [rdx]
vmovlpd xmm14, [rdi+rax*8]
vmulsd  xmm13, xmm14
vaddsd  xmm15, xmm13 ; vaddsd xmm4, xmm0 zeroes the highest 128 bits :(

add     rsi, 8
add     rdx, 4

loop _denseToSparseDotProduct_unstable_AVX_64_linux_loop_aa_short_loop_2

_denseToSparseDotProduct_unstable_AVX_64_linux_short_end:


vaddpd   ymm4, ymm15

vhaddpd  ymm4, ymm4
vextractf128 xmm3, ymm4, 1
vaddsd   xmm4, xmm3

vmovsd   xmm0, xmm4


pop     rbx
pop     rax

vzeroupper

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
vmovapd ymm1, [rsi+32]
vmovapd ymm2, [rsi+64]
vmovapd ymm3, [rsi+96]

mov     eax, [rdx]
vmovlpd xmm8, [rdi+rax*8]
mov     eax, [rdx+4]
vmovhpd xmm8, [rdi+rax*8]
mov     eax, [rdx+8]
vmovlpd xmm12, [rdi+rax*8]
mov     eax, [rdx+12]
vmovhpd xmm12, [rdi+rax*8]
vinsertf128 ymm8, ymm8, xmm12, 1

mov     eax, [rdx+16]
vmovlpd xmm9, [rdi+rax*8]
mov     eax, [rdx+20]
vmovhpd xmm9, [rdi+rax*8]
mov     eax, [rdx+24]
vmovlpd xmm12, [rdi+rax*8]
mov     eax, [rdx+28]
vmovhpd xmm12, [rdi+rax*8]
vinsertf128 ymm9, ymm9, xmm12, 1

mov     eax, [rdx+32]
vmovlpd xmm10, [rdi+rax*8]
mov     eax, [rdx+36]
vmovhpd xmm10, [rdi+rax*8]
mov     eax, [rdx+40]
vmovlpd xmm12, [rdi+rax*8]
mov     eax, [rdx+44]
vmovhpd xmm12, [rdi+rax*8]
vinsertf128 ymm10, ymm10, xmm12, 1

mov     eax, [rdx+48]
vmovlpd xmm11, [rdi+rax*8]
mov     eax, [rdx+52]
vmovhpd xmm11, [rdi+rax*8]
mov     eax, [rdx+56]
vmovlpd xmm12, [rdi+rax*8]
mov     eax, [rdx+60]
vmovhpd xmm12, [rdi+rax*8]
vinsertf128 ymm11, ymm11, xmm12, 1

prefetcht0  [rsi+256]
prefetcht0  [rdx+128]

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

vzeroupper

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

;dppd    xmm0, xmm8, 0x33
;dppd    xmm1, xmm9, 0x33
;dppd    xmm2, xmm10, 0x33
;dppd    xmm3, xmm11, 0x33

mulpd   xmm0, xmm8
mulpd   xmm1, xmm9
mulpd   xmm2, xmm10
mulpd   xmm3, xmm11

addpd   xmm4, xmm0
addpd   xmm4, xmm1
addpd   xmm4, xmm2
addpd   xmm4, xmm3

;addsd   xmm4, xmm0
;addsd   xmm4, xmm1
;addsd   xmm4, xmm2
;addsd   xmm4, xmm3

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
