/* Yunzhi Li (880371) Project 2, COMP300023 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include "http.h"

#define BUFFER_SIZE 256

void processHttpRequest(int newsockfd, char *root_path) {
    char buffer[BUFFER_SIZE];

    // Read characters from the connection, then process
	int n = read(newsockfd, buffer, 255); // n is number of characters read
	if (n < 0) {
		perror("read");
		exit(EXIT_FAILURE);
	}
	// Null-terminate string
	buffer[n] = '\0';

	// Write message back
	printf("Here is the message: %s\n", buffer);
	n = write(newsockfd, "I got your message", 18);
	if (n < 0) {
		perror("write");
		exit(EXIT_FAILURE);
	}


    close(newsockfd);
}