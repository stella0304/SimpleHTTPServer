/* Yunzhi Li (880371) Project 2, COMP300023 */
#define IMPLEMENTS_IPV6

#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netdb.h>
#include <limits.h>
#include "http.h"


void check_args(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, re, s;
	struct addrinfo hints, *res, *p;
	struct sockaddr_storage client_addr;
	socklen_t client_addr_size;
    
    // deal with command line arguments
	check_args(argc, argv);
	int ipv = atoi(argv[1]);
	char *port = argv[2];
    char *root_path = argv[3];

    // Create address we're going to listen on 
	memset(&hints, 0, sizeof hints);
	if (ipv == 4) {
		hints.ai_family = AF_INET;
	} else {
		hints.ai_family = AF_INET6;
	}
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = AI_PASSIVE; 
	s = getaddrinfo(NULL, port, &hints, &res);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	// Create socket
	for (p = res; p != NULL; p = p->ai_next) {
		if (p->ai_family == hints.ai_family) {
			sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		}
	}
	if (sockfd < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	// Reuse port if possible
	re = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &re, sizeof(int)) < 0) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	// Bind address to the socket
	if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(res);

	// Listen on socket
	if (listen(sockfd, 5) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	while (1) {
		client_addr_size = sizeof client_addr;
		newsockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_addr_size);
		if (newsockfd < 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}

		processHttpRequest(newsockfd, root_path);

	}

	close(sockfd);
	return 0;
}


void check_args(int argc, char *argv[]) {
	// checks if command line argumentss are valid
	if (argc != 4) {
        printf("Wrong number of arguments\n");
        exit(EXIT_FAILURE);
    } 

	// check protocol
	int ipv = atoi(argv[1]);//, port = atoi(argv[2]);
	if (ipv != 4 && ipv != 6) {
		printf("invalid protocol number\n");
		exit(EXIT_FAILURE);
	}
	
	// check root path
	
	/*
	char curr_path[PATH_MAX];
	if (getcwd(curr_path, sizeof(curr_path)) == NULL) {
		perror("getcwd error");
		exit(EXIT_FAILURE);
	}
	char *full_path = strcat(curr_path, argv[3]);
	printf("%s\n", full_path);
	*/
	
	struct stat stat_buf;
	if (stat(argv[3], &stat_buf) != 0 || !S_ISDIR(stat_buf.st_mode)) {
		printf("invalid root path\n");
		exit(EXIT_FAILURE);
	}
}
