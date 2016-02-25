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
#include "lib/user.h"

#define EXIT 0
#define DEFAULT_PORT "3494"

void* perform();

user_store_t user_store;

char* NICKS[] = {
    "Rocket Raccoon",
    "Groot",
    "Drax",
    "Calypso",
    "Bishop",
    "Cypher",
    "Devos",
    "Dazzler",
    "Dormammu",
    "Debrii",
    "Fenris",
    "Galia",
    "Havok",
    "Jackal",
    "Jubilee",
    "Kabuki",
    "Kylun",
    "Landau",
    "Yondu",
    "Ozymandias"
};


int
main(int argc, char* argv[])
{
    /* Check if user is setting up a port */
    char* port;
    if (argc > 1 &&  strcmp(argv[1], "-p") == 0) {
        if (argv[2] != NULL)
            port = argv[2];
    } else {
        port = DEFAULT_PORT;
    }

    user_store = create_user_store();
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // fill my ip for me

    getaddrinfo(NULL, port, &hints, &res);

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
    if(listen(sockfd, MAX_USERS) == -1) {
        print_error("failed to listen on socket");
        return errno;
    }

    // Show server message
    printf("Listening on localhost:%s, CTRL+C to stop\n", port);

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
    }
    return EXIT;
}

void *
perform(int *sockfd)
{
    int new_sockfd = *sockfd;

    /* Set user */
    user_t user;
    memset(&user, '0', sizeof user);
    user.sockfd = sockfd;
    user.nick = NICKS[rand() % ARR_SIZE(NICKS)];

    us_add(&user_store, &user);

    /* Send stuff to the client socket */
    char* msg = "Connection stablished...\n";
    int msg_len = strlen(msg);
    if (send(new_sockfd, msg, msg_len, 0) == -1) {
        print_error("failed when sending message");
        exit(errno);
    }

    char msg_buffer[1024];
    while (recv(new_sockfd, &msg_buffer, 1024, 0)) {
        printf("%s: %s\n", user.nick, msg_buffer);
    }
    return NULL;
}

