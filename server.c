#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
#include "lib/util.h"

#define EXIT 0
#define SERVER_PORT "3494"

int connection_pool[MAX_CONNECTIONS];

int
main(int argc, char* argv[])
{
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // fill my ip for me

    getaddrinfo(NULL, SERVER_PORT, &hints, &res);

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
    if(listen(sockfd, MAX_CONNECTIONS) == -1) {
        print_error("failed to listen on socket");
        return errno;
    }

    // Show server message
    printf("Listening on localhost:%s, CTRL+C to stop\n", SERVER_PORT);

    /* Accept inconming connections */
    struct sockaddr_storage client_addr;
    socklen_t addr_size;
    int new_sockfd;
    while(1) {
        if ((new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size)) == -1) {
            print_error("failed when accepting socket connection");
            return errno;
        }

        pthread_t thread;
        int err = pthread_create(&thread, NULL, (void*)&perform, &new_sockfd);
        if (err == -1)
            print_error("can't create thread");
        else
            print_info("Thread created successfully");
    }
    return EXIT;
}

void *
perform(int *sockfd)
{
    int new_sockfd = *sockfd;
    int stored = store_connection(connection_pool, new_sockfd);
    if (stored)
        print_info("Connection stored");
    else
        print_info("Couldn't store connection, max connections reached...");

    // Send stuff to the client socket
    char* msg = "Connection stablished...\n";
    int msg_len = strlen(msg);
    if (send(new_sockfd, msg, msg_len, 0) == -1) {
        print_error("failed when sending message");
        exit(errno);
    }

    char msg_buffer[1024];
    while (recv(new_sockfd, &msg_buffer, 1024, 0)) {
        printf("msg: %s\n", msg_buffer);
    }
    return NULL;
}

int
store_connection(int pool[MAX_CONNECTIONS], int sock_fd)
{
    static int connection_counter = 0;
    int index = 0;
    if (connection_counter == 0)
        index = connection_counter;
    else
        index = (connection_counter += 1);

    if (connection_counter <= MAX_CONNECTIONS) {
        print_info("got here");
        pool[index] = sock_fd;
        return 1;
    }
    return 0;
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
