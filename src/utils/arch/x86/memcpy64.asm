
; TODO: elejen beolvasni annyi bajtot, hogy 16 bajtos hataron
; menjen az olvasas es iras
; vegen a maradekot kulon kezelni

SECTION .text

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Use cache for read and write
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _memcpy_sse2_64_linux_cread_cwrite
_memcpy_sse2_64_linux_cread_cwrite:
push    rbp
mov     rbp, rsp

push    rax
push    rbx
push    rcx

; rdi: destination
; rsi: source
; rdx: count



; 16 bajtos hataron van?
mov     rax, rsi
and     rax, 0x0F

mov     rbx, rdi
and     rbx, 0x0F

or      rax, rbx

mov     rcx, rdx
and     rcx, 0x3F
shr     rdx, 6

cmp     rdx, 0
je      _memcpy_sse2_64_linux_cread_cwrite_end

cmp     rax, 0
jne     _memcpy_sse2_64_linux_cread_cwrite_uu



align 16
_memcpy_sse2_64_linux_cread_cwrite_aa_loop:

movdqa  xmm0, [rsi]
movdqa  xmm1, [rsi+16]
movdqa  xmm2, [rsi+32]
movdqa  xmm3, [rsi+48]

movdqa [rdi], xmm0
movdqa [rdi+16], xmm1
movdqa [rdi+32], xmm2
movdqa [rdi+48], xmm3

add     rsi, 64
add     rdi, 64
sub     rdx, 1

jnz _memcpy_sse2_64_linux_cread_cwrite_aa_loop

; a vegen meg a maradekot is atmasoljuk
jmp _memcpy_sse2_64_linux_cread_cwrite_end

_memcpy_sse2_64_linux_cread_cwrite_uu:


align 16
_memcpy_sse2_64_linux_cread_cwrite_uu_loop:

movdqu  xmm0, [rsi]
movdqu  xmm1, [rsi+16]
movdqu  xmm2, [rsi+32]
movdqu  xmm3, [rsi+48]

movdqu [rdi], xmm0
movdqu [rdi+16], xmm1
movdqu [rdi+32], xmm2
movdqu [rdi+48], xmm3

add     rsi, 64
add     rdi, 64
sub     rdx, 1

jnz _memcpy_sse2_64_linux_cread_cwrite_uu_loop

_memcpy_sse2_64_linux_cread_cwrite_end:


rep     movsb

pop     rcx
pop     rbx
pop     rax

mov     rsp, rbp
pop     rbp
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Non temporal write, non temporal read
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _memcpy_sse4_1_64_linux_ntread_ntwrite
_memcpy_sse4_1_64_linux_ntread_ntwrite:
push    rbp
mov     rbp, rsp

push    rax
push    rbx
push    rcx

; rdi: destination
; rsi: source
; rdx: count



; 16 bajtos hataron van?
mov     rax, rsi
and     rax, 0x0F

mov     rbx, rdi
and     rbx, 0x0F

or      rax, rbx

mov     rcx, rdx
and     rcx, 0x3F
shr     rdx, 6

cmp     rdx, 0
je      _memcpy_sse4_1_64_linux_ntread_ntwrite_end

cmp     rax, 0
jne     _memcpy_sse4_1_64_linux_ntread_ntwrite_uu


align 16
_memcpy_sse4_1_64_linux_ntread_ntwrite_aa_loop:

movntdqa xmm0, [rsi]
movntdqa  xmm1, [rsi+16]
movntdqa  xmm2, [rsi+32]
movntdqa  xmm3, [rsi+48]

movntdq   [rdi], xmm0
movntdq [rdi+16], xmm1
movntdq [rdi+32], xmm2
movntdq [rdi+48], xmm3

add     rsi, 64
add     rdi, 64
sub     rdx, 1

jnz _memcpy_sse4_1_64_linux_ntread_ntwrite_aa_loop

sfence

; a vegen meg a maradekot is atmasoljuk
jmp _memcpy_sse2_64_linux_cread_cwrite_end

_memcpy_sse4_1_64_linux_ntread_ntwrite_uu:


align 16
_memcpy_sse4_1_64_linux_ntread_ntwrite_uu_loop:

movdqu xmm0, [rsi]
movdqu  xmm1, [rsi+16]
movdqu  xmm2, [rsi+32]
movdqu  xmm3, [rsi+48]

movdqu   [rdi], xmm0
movdqu [rdi+16], xmm1
movdqu [rdi+32], xmm2
movdqu [rdi+48], xmm3

add     rsi, 64
add     rdi, 64
sub     rdx, 1

jnz _memcpy_sse4_1_64_linux_ntread_ntwrite_uu_loop

sfence

_memcpy_sse4_1_64_linux_ntread_ntwrite_end:

rep     movsb

pop     rcx
pop     rbx
pop     rax

mov     rsp, rbp
pop     rbp
ret
