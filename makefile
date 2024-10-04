default: main
main: main.c test.c mes_fonctions.c mes_signatures.h mes_strucures.h
	gcc -g -Wall main.c test.c mes_fonctions.c -o main
	./main