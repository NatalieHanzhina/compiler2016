extern printf
extern scanf
extern strcmp
extern strcat
extern strcpy

extern malloc
extern memcpy
extern free

section .data
int_format_s_n dd "%d", 10, 0
int_format_p_n db "%d", 10, 0
int_format_s dd "%d", 0
int_format_p db "%d", 0
section .bss
a: resd 1
section .text
global main

main:
push ebp
mov ebp, esp
sub esp, 8
push dword 1
pop eax
mov [ebp - 4], eax
push dword [ebp - 4]
push dword 7
pop ebx
pop eax
imul ebx
push eax
push dword 3
pop ebx
pop eax
mov edx, 0
idiv ebx
push eax
push dword 1
pop ebx
pop edx
add edx, ebx
mov eax, edx
push eax
pop eax
mov [ebp - 4], eax
push dword [ebp - 4]
push int_format_p
call printf
add esp, 8
mov esp, ebp
pop ebp
ret
