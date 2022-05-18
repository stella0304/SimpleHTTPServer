server: server.o http.o queue.o
	gcc  -Wall -pthread -o server http.o queue.o server.o

clean:
	rm -f *.o server

server.o: server.c http.h queue.h
	gcc -Wall -c -o server.o server.c

http.o: http.c http.h
	gcc -Wall -c -o http.o http.c

queue.o: queue.c queue.h
	gcc -Wall -c -o queue.o queue.c
