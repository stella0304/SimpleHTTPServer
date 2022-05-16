/* Yunzhi Li (880371) Project 2, COMP300023 */
#ifndef RAGRAPH_H
	#define RAGRAPH_H

typedef struct {
    int response_code;
    char *file_type;
} response_header_t;

void processHttpRequest(int newsockfd, char *root_path);

#endif