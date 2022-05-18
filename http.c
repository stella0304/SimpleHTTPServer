/* Yunzhi Li (880371) Project 2, COMP300023 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "http.h"

#define BUFFER_SIZE 256
#define MAX_REQUEST_SIZE 3000 // plenty for 2kB
#define METHOD_SIZE 10
#define MAX_TYPE_SIZE 10
#define MAX_MINE_SIZE 30
#define OK_RESPONSE "HTTP/1.0 200 OK\r\nContent-Type: "
#define NOT_FOUND_RESPONSE "HTTP/1.0 404 Not Found\r\n\r\n"
#define END_OF_RESPONSE "\r\n\r\n"

static int read_request(char *request_str, int newsockfd);
static int get_path(char *request_str, char *path_buffer);
static int check_valid_request_path(char *path);
static void get_full_path(char *root_path, char *requested_path, char *full_path);
static int check_valid_full_path(char *path);
static void send_404(int newsockfd);
static void send_200(int newsockfd, char *path);
static void get_file_type(char *path, char *type_buffer);
static void send_response(int newsockfd, char *response);

// processes one single http request and respond
void processHttpRequest(int newsockfd, char *root_path) {

    // read the full request
    char request_str[MAX_REQUEST_SIZE + 1];
    if (!read_request(request_str, newsockfd)) {
        printf("invalid request received\n");
        return;
    }

    // separate out the path to file
    char path_to_file[PATH_MAX + 1];
    if (!get_path(request_str, path_to_file)) {
        printf("invalid request received\n");
        return;
    }

    if (!check_valid_request_path(path_to_file)) {
        send_404(newsockfd);
        return;
    }

    // get the full path of file
    char full_path[PATH_MAX + 1];
    get_full_path(root_path, path_to_file, full_path);

    // check if what's at full path exists and is a file
    if (!check_valid_full_path(full_path)) {
        send_404(newsockfd);
        return;
    }

    send_200(newsockfd, full_path);
}

// read the full http request
static int read_request(char *request_str, int newsockfd) {
    request_str[0] = '\0';
    char buffer[BUFFER_SIZE];
    int char_read = 0, n, valid_request = 0;

    // Read characters from the connection, then process
    while (char_read < MAX_REQUEST_SIZE - BUFFER_SIZE) {
        
        n = read(newsockfd, buffer, BUFFER_SIZE-1); // n is number of characters read
        if (n < 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        buffer[n] = '\0';
        strcat(request_str, buffer);
        char_read += n;

        // check if we have read in the full request (reached 2 CRLF)
        int end_reached = 0;
        for (int i=3; request_str[i] != '\0'; i++) {
            if (request_str[i] == '\n' && request_str[i-1] == '\r' &&
             request_str[i-2] == '\n' && request_str[i-3] == '\r') {
                end_reached = 1;
                break;
            }
        }

        if (end_reached) {
            valid_request = 1;
            break;
        }
    }

    return valid_request;
}

// go through the request and find the path to requested file
static int get_path(char *request_str, char *path_buffer) {
    char method[METHOD_SIZE];
    int i, j=0;

    // check if request is a get request
    for (i=0; request_str[i] != ' ' && request_str[i] != '\0'; i++) {
        method[i] = request_str[i];
    }
    method[i] = '\0';
    i++;

    if (strcmp(method, "GET") != 0) {
        printf("Not a get request\n");
        return 0;
    }

    // parse in path to requested file
    while (request_str[i] != ' ' && request_str[i] != '\0') {
        path_buffer[j] = request_str[i];
        i++;
        j++;
    }
    path_buffer[j] = '\0';

    return 1;
}

// check that the path doesn't escape out of the root folder
static int check_valid_request_path(char *path) {
    int valid = 0;
    for (int i=2; path[i] != '\0'; i++) {
        if (path[i] == '/') {
            if (path[i-1] == '.' && path[i-2] == '.') {
                valid--;
                if (valid < 0) { // escaped out of the root folder
                    return 0;
                }
            } else {
                valid++;
            }
        }
    }

    return 1;
}

// make full path with the root path and the requested path
static void get_full_path(char *root_path, char *requested_path, char *full_path_buffer) {
    full_path_buffer[0] = '\0';
    strcat(full_path_buffer, root_path);
    strcat(full_path_buffer, requested_path);
}


// check if path exist and if the file is a regular file
static int check_valid_full_path(char *path) {
    struct stat stat_buf;
    if (stat(path, &stat_buf) != 0 || !S_ISREG(stat_buf.st_mode)) {
		return 0;
	}
    return 1;
}

// send 404 response
static void send_404(int newsockfd) {
    printf("invalid path received\n");
    char *response = NOT_FOUND_RESPONSE;
    send_response(newsockfd, response);
    close(newsockfd);
}

// send a 200 response, the file type, and the file
static void send_200(int newsockfd, char *path) {
    printf("valid request received\n");
    char *response = OK_RESPONSE;
    send_response(newsockfd, response);

    // send file mime type
    char type[MAX_TYPE_SIZE];
    get_file_type(path, type);
    char mine_type[MAX_MINE_SIZE];
    if (strcmp(type, "jpg") == 0) {
        strcpy(mine_type, "image/jpeg");
    } else if (strcmp(type, "html") == 0) {
        strcpy(mine_type, "text/html");
    } else if (strcmp(type, "css") == 0) {
        strcpy(mine_type, "text/css");
    } else if (strcmp(type, "js") == 0) {
        strcpy(mine_type, "text/javascript");
    } else {
        strcpy(mine_type, "application/octet-stream");
    }
    
    send_response(newsockfd, mine_type);

    // send end of response
    char *end_response = END_OF_RESPONSE;
    send_response(newsockfd, end_response);

    // send file


    close(newsockfd);
}

static void get_file_type(char *path, char *type_buffer) {
    type_buffer[0] = '\0';
    int type_index = -1;

    // find the . in the path
    for (int i = strlen(path) - 1; path[i] >= 0; i--) {
        if (path[i] == '.') {
            type_index = i + 1;
            break;
        } else if (path[i] == '/') {
            return;
        }
    }

    // copy over file type
    int j;
    for (j = type_index; path[j] != '\0'; j++) {
        type_buffer[j-type_index] = path[j];
    }
    type_buffer[j-type_index] = '\0';
}

// send a string to newsockfd
static void send_response(int newsockfd, char *response) {
    int n = write(newsockfd, response, (size_t) strlen(response));
    if (n < 0) {
        perror("write");
        exit(EXIT_FAILURE);
    }
}