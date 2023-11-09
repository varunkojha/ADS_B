#! /bin/bash

adsB: main.o generateMessage.o parseMessage.o
	gcc -o adsB main.o generateMessage.o parseMessage.o

main.o: main.c headers.h declarations.h
	gcc -c main.c 
generateMessage.o: generateMessage.c headers.h declarations.h
	gcc -c generateMessage.c 
parseMessage.o: parseMessage.c headers.h declarations.h
	gcc -c parseMessage.c 

clean:
	rm -rf *.o adsB *.gch 
	sync
