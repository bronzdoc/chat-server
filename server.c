#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include "lib/util.h"

#define EXIT 0
#define SERVER_PORT "3490"

int
main(int argc, char* argv[])
{
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // fill my ip for me

    getaddrinfo(NULL, SERVER_PORT , &hints, &res);

    /* Create socket */
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        print_error("failed to create a socket");
        return errno;
    }

    /* Bind port to socket */
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        print_error("failed to bind socket port");
        return errno;
    }

    /* Listen for connection in the bound socket */
    int connections_allowed = 5;
    if(listen(sockfd, connections_allowed) == -1) {
        print_error("failed to listen on socket");
        return errno;
    }

    // Show server message
    printf("Listening on localhost:%s, CTRL+C to stop\n", SERVER_PORT);

    /* Accept inconming connections */
    struct sockaddr_storage client_addr;
    socklen_t addr_size;
    int new_sockfd;
    if ((new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size)) == -1) {
        print_error("failed when accepting socket connection");
        return errno;
    }

    /* Do some cool shit! */

    // Send stuff to the client socket
    char* msg = "Connection stablished...";
    int msg_len = strlen(msg);
    if (send(new_sockfd, msg, msg_len, 0) == -1) {
        print_error("failed when sending message");
        return errno;
    }

    char msg_buffer[1024];
    recv(new_sockfd, &msg_buffer, 1024, 0);
    printf("msg:%s\n", msg_buffer);

    return EXIT;
}
