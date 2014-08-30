
SECTION .data

mxcsr:
db  0, 0, 0, 0

global _cpuinfo_64_linux

_cpuinfo_64_linux:
push    rbp
mov     rbp, rsp

push    rbx

;stmxcsr [mxcsr]
;mov     ebx, [mxcsr]
;or      ebx, 0x8940
;mov     [mxcsr], ebx
;ldmxcsr [mxcsr]

; RDI, RSI, RDX, RCX, R8
mov     rax, rdi
mov     rbx, rsi

mov     rdi, rcx
mov     rcx, rdx
mov     rdx, rdi

cpuid

mov     [r8], eax
mov     [r8 + 4], ebx
mov     [r8 + 8], ecx
mov     [r8 + 12], edx

pop     rbx

mov     rsp, rbp
pop     rbp
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _avx_enabled_by_64
_avx_enabled_by_64:
push    rbp
mov     rbp, rsp

mov     rcx, 0
xgetbv
and     rax, 0x06

mov     rsp, rbp
pop     rbp
ret
