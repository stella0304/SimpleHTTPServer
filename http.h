/* Yunzhi Li (880371) Project 2, COMP300023 */
#ifndef HTTP_H
	#define HTTP_H

typedef struct {
    int response_code;
    char *file_type;
} response_header_t;

void processHttpRequest(int newsockfd, char *root_path);

#endif