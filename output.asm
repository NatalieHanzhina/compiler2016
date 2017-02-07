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
b: resd 1
i: resd 1
a: resd 1
section .text
global main

f:
push ebp
mov ebp, esp
push dword 3
pop eax
mov [ebp + 24], eax
push dword 0
pop eax
mov [ebp + 20], eax
L6:
push dword [ebp + 12]
push dword 3
pop ebx
pop eax
imul ebx
push eax
pop eax
mov [ebp + 12], eax
push dword [ebp + 20]
push dword 1
pop ebx
pop edx
add edx, ebx
mov eax, edx
push eax
pop eax
mov [ebp + 20], eax
push dword [ebp + 20]
push int_format_p_n
call printf
add esp, 8
push dword [ebp + 20]
push dword 3
pop ebx
pop edx
cmp edx, ebx
jae L17
mov eax, 0
jmp L18
L17:
mov eax, 1
L18:
push eax
pop eax
cmp eax, 1
jne L6
push dword [ebp + 12]
pop eax
push dword 4
pop eax
sub esp, 12
mov esp, ebp
pop ebp
ret 8

main:
push ebp
mov ebp, esp
sub esp, 20
pusha
lea ebx, [ebp - 4]
push ebx
push int_format_s
call scanf
add esp, 8
popa
push dword 0
pop eax
mov [ebp - 8], eax
push dword [ebp - 4]
push dword [ebp - 12]
call f
push eax
pop eax
mov [ebp - 4], eax
push dword [ebp - 4]
push int_format_p_n
call printf
add esp, 8
mov esp, ebp
pop ebp
ret
