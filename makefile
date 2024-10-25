all: main run

main: main.o mes_fonctions.o
	gcc -o main main.o mes_fonctions.o

main.o: main.c mes_structures.h mes_signatures.h
	gcc -c main.c

mes_fonctions.o: mes_fonctions.c mes_structures.h mes_signatures.h
	gcc -c mes_fonctions.c

run: main
	chcp 65001 > nul
	./main.exe
clean:
	del /Q *.o main.exe
	@if exist Captures rmdir /S /Q Captures