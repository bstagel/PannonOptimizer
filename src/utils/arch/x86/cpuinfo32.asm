
SECTION .data

global _cpuinfo_32

_cpuinfo_32:
push    ebp
mov     ebp, esp

; epb + 8:  eax
; ebp + 12: ebx
; ebp + 16: ecx
; ebp + 20: edx
; ebp + 24: result address

push    eax
push    ebx
push    ecx
push    edx
push    edi

mov     eax, [ebp + 8]
mov     ebx, [ebp + 12]
mov     ecx, [ebp + 16]
mov     edx, [ebp + 20]
mov     edi, [ebp + 24]

cpuid

mov     [edi], eax
mov     [edi + 4], ebx
mov     [edi + 8], ecx
mov     [edi + 12], edx

pop     edi
pop     edx
pop     ecx
pop     ebx
pop     eax

mov     esp, ebp
pop     ebp
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _cpuinfo_supported_32
_cpuinfo_supported_32:

push    ebp
mov     ebp, esp

push    ecx

pushf
pop     eax
mov     ecx, eax
xor     eax, 0x200000
push    eax
popf
pushf
pop     eax
xor     eax, ecx
push    ecx
popf

pop     ecx


mov     esp, ebp
pop     ebp
ret
