assembler: main.o first_read_mcro.o first_loop.o function_for_loop1.o second_loop.o function_for_make_files.o
	gcc -ansi -pedantic -g -Wall main.o first_read_mcro.o first_loop.o function_for_loop1.o second_loop.o function_for_make_files.o -o assembler


main.o: main.c asembler.h
	gcc -c -ansi -pedantic -g -Wall main.c -o main.o

first_read_mcro.o: first_read_mcro.c asembler.h
	gcc -c -ansi -pedantic -g -Wall first_read_mcro.c -o first_read_mcro.o

first_loop.o: first_loop.c asembler.h
	gcc -c -ansi -pedantic -g -Wall first_loop.c -o first_loop.o

function_for_loop1.o: function_for_loop1.c asembler.h
	gcc -c -ansi -pedantic -g -Wall function_for_loop1.c -o function_for_loop1.o

second_loop.o: second_loop.c asembler.h
	gcc -c -ansi -pedantic -g -Wall second_loop.c -o second_loop.o

function_for_make_files.o: function_for_make_files.c asembler.h
	gcc -c -ansi -pedantic -g -Wall function_for_make_files.c -o function_for_make_files.o
