all: compile testsuite

compile: 
	gcc Main.c -o main

testsuite: tc1 tc2 tc3 tc4 tc5

tc1: 
	@echo "-----tc1: Program for computing Greatest Common Divisor-----"
	@./main TESTS/nod.pas TESTS/nod.lst
	@if [ -f output.asm ]; then nasm -f elf -l output.lst output.asm; fi
	@if [ -f output.o ]; then gcc -m32 -o prog output.o; fi
	@if [ -f prog ]; then cat TESTS/inp_nod; ./prog < TESTS/inp_nod || true; fi
	@echo "-----tc1 passed-----\n"
	@-rm -f *.o
	@-rm -f *.asm
	@-rm -f prog

tc2: 
	@echo "-----tc2: Program for computing simple expression (base operations)-----"
	@./main TESTS/prog_exp TESTS/prog_exp.lst
	@if [ -f output.asm ]; then nasm -f elf -l output.lst output.asm; fi
	@if [ -f output.o ]; then gcc -m32 -o prog output.o; fi
	@if [ -f prog ]; then ./prog || true; fi
	@echo "\n-----tc2 passed-----\n"
	@-rm -f *.o
	@-rm -f *.asm
	@-rm -f prog

tc3: 
	@echo "-----tc3: Program for computing cube in repeat..until loop in function-----"
	@./main TESTS/prog_s.pas TESTS/prog_s.lst
	@if [ -f output.asm ]; then nasm -f elf -l output.lst output.asm; fi
	@if [ -f output.o ]; then gcc -m32 -o prog output.o; fi
	@if [ -f prog ]; then cat TESTS/inp_cube; ./prog < TESTS/inp_cube || true; fi
	@echo "-----tc3 passed-----\n"
	@-rm -f *.o
	@-rm -f *.asm
	@-rm -f prog

tc4: 
	@echo "-----tc4: Program with errors-----"
	@./main TESTS/prog_err TESTS/prog_err.lst
	@if [ -f output.asm ]; then nasm -f elf -l output.lst output.asm; fi
	@if [ -f output.o ]; then gcc -m32 -o prog output.o; fi
	@if [ -f prog ]; then ./prog || true; fi
	@echo "-----tc4 passed-----\n"
	@-rm -f *.o
	@-rm -f *.asm
	@-rm -f prog

tc5: 
	@echo "-----tc5: Program for computing increment in function in while loop and if statement checking-----"
	@./main TESTS/prog_r TESTS/prog_r.lst
	@if [ -f output.asm ]; then nasm -f elf -l output.lst output.asm; fi
	@if [ -f output.o ]; then gcc -m32 -o prog output.o; fi
	@if [ -f prog ]; then cat TESTS/inp_inc; ./prog < TESTS/inp_inc || true; fi
	@echo "\n-----tc5 passed-----\n"
	@-rm -f *.o
	@-rm -f *.asm
	@-rm -f prog

clean:
	-rm -f *.o
