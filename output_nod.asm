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
n: resd 1
d: resd 1
c: resd 1
section .text
global main

nod:
push ebp
mov ebp, esp
L3:
push dword [ebp + 12]
push dword [ebp + 8]
pop ebx
pop eax
imul ebx
push eax
push dword 0
pop ebx
pop edx
cmp edx, ebx
jnz L5
mov eax, 0
jmp L6
L5:
mov eax, 1
L6:
push eax
pop eax
cmp eax, 1
jne L2
push dword [ebp + 12]
push dword [ebp + 8]
pop ebx
pop edx
cmp edx, ebx
ja L11
mov eax, 0
jmp L12
L11:
mov eax, 1
L12:
push eax
pop eax
cmp eax, 1
jne L8
push dword [ebp + 12]
push dword [ebp + 8]
pop ebx
pop eax
mov edx, 0
idiv ebx
mov eax, edx
push eax
pop eax
mov [ebp + 12], eax
jmp L9
L8:
push dword [ebp + 8]
push dword [ebp + 12]
pop ebx
pop eax
mov edx, 0
idiv ebx
mov eax, edx
push eax
pop eax
mov [ebp + 8], eax
L9:
jmp L3
L2:
push dword [ebp + 12]
push dword [ebp + 8]
pop ebx
pop edx
add edx, ebx
mov eax, edx
push eax
pop eax
sub esp, 4
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
push int_format_s_n
call scanf
add esp, 8
popa
pusha
lea ebx, [ebp - 8]
push ebx
push int_format_s_n
call scanf
add esp, 8
popa
push dword [ebp - 4]
push dword [ebp - 8]
call nod
push eax
pop eax
mov [ebp - 12], eax
push dword [ebp - 12]
push int_format_p_n
call printf
add esp, 8
mov esp, ebp
pop ebp
ret
