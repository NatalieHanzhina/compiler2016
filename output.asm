extern printf
extern scanf
extern strcmp
extern strcat
extern strcpy

extern malloc
extern memcpy
extern free

section .data
int_format dd "%d", 10, 0
str_format dd "%s", 10, 0
section .bss
l: resd 1
b: resd 1
a: resd 1
section .text
global main

f:
push ebp
mov ebp, esp
push dword c
pop ebx
pop eax
push dword 1
pop ebx
pop eax
pop ebx
pop edx
add edx, ebx
mov eax, edx
push eax
pop eax
mov f, eax
sub esp, 8
mov esp, ebp
pop ebp
ret
main:
push ebp
mov ebp, esp
sub esp, 20
pusha
lea ebx, [ebp - 4]
push ebx
push int_format
call scanf
add esp, 8
popa
push dword a
pop ebx
pop eax
push dword 10
pop ebx
pop eax
pop ebx
pop edx
cmp edx, ebx
jb L8
mov eax, 0
jmp L9
L8:
mov eax, 1
L9:
push eax
pop eax
cmp eax, 1
jne L7
L8:
push dword a
pop ebx
pop eax
call f
push eax
pop ebx
pop eax
pop eax
mov a, eax
pop eax
cmp eax, 1
je L8
L7:
push dword a
pop ebx
pop eax
push dword 10
pop ebx
pop eax
pop ebx
pop edx
cmp edx, ebx
ja L9
mov eax, 0
jmp L10
L9:
mov eax, 1
L10:
push eax
pop eax
cmp eax, 1
jne L8
push dword a
pop ebx
pop eax
push int_format
call printf
add esp, 8
L8:
mov esp, ebp
pop ebp
ret
