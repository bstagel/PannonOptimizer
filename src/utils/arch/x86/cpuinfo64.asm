
SECTION .data

global _cpuinfo_64_linux

_cpuinfo_64_linux:
push    rbp
mov     rbp, rsp

push    rbx

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
