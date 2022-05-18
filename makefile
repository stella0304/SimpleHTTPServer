server: server.o http.o
	gcc  -Wall -o server http.o server.o

clean:
	rm -f *.o server

server.o: server.c http.h
	gcc -Wall -c -o server.o server.c

http.o: http.c http.h
	gcc -Wall -c -o http.o http.c
