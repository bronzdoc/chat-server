#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define EXIT 0

void print_error(char*);

void print_info(char* msg);

int
main(int argc, char* argv[])
{
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // fill my ip for me

    getaddrinfo(NULL, "3490", &hints, &res);

    /* Create socket */
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1)
        print_error("failed to create a socket");

    /* Bind port to socket */
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1)
        print_error("failed to bind socket port");

    /* Listen for connection in the bound socket */
    int connections_allowed = 5;
    if(listen(sockfd, connections_allowed) == -1)
        print_error("failed to listen on socket");

    /* Accept inconming connections */
    struct sockaddr_storage client_addr;
    socklen_t addr_size;
    int new_sockfd;
    if ((new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size)) == -1)
        print_error("failed when accepting socket connection");

    /* Do some cool shit! */

    // Send stuff to the client socket
    char* msg = "Connection stablished...";
    int msg_len = strlen(msg);
    if (send(new_sockfd, msg, msg_len, 0) == -1)
        print_error("failed when sending message");

    char msg_buffer[1024];
    recv(new_sockfd, &msg_buffer, 1024, 0);
    printf("msg:%s\n", msg_buffer);
    return EXIT;
}

void
print_error(char* msg)
{
    printf("ERROR: %s\n", msg);
}

void
print_info(char* msg) {
    printf("INFO: %s\n", msg);
}
