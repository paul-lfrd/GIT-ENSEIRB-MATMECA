objects = main.o ecriture.o huffman.o

program: $(objects)
	gcc -Wall -o program $(objects)
	./program
main.o : main.c ecriture.h huffman.h
	gcc -Wall -c main.c
ecriture.o : ecriture.c ecriture.h
	gcc -Wall -c ecriture.c
huffman.o : huffman.c huffman.h
	gcc -Wall -c huffman.c 

clean:
	rm $(objects)