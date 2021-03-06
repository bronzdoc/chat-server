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

void* handle_connection(user_t*);

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
    /* get my ip */
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, port, &hints, &res);

    /* Create socket */
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        print_error("failed to create a socket");
        return errno;
    }

    /* Bind port to socket */
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        print_error("failed to bind socket port");
        return errno;
    }

    /* Listen for connection in the bound socket */
    if(listen(sockfd, MAX_USERS) < 0) {
        print_error("failed to listen on socket");
        return errno;
    }

    /* Show server message */
    printf("Listening on localhost:%s, CTRL+C to stop\n", port);

    /* Accept inconming connections */
    struct sockaddr_storage client_addr;
    socklen_t addr_size;
    int new_sockfd;
    while(1) {
        user_t* user = (user_t*)malloc(sizeof(user_t));

        if ((new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size)) < 0) {
            print_error("failed when accepting socket connection");
            return errno;
        }

        /* Set user */
        user->sockfd = new_sockfd;
        user->nick = NICKS[rand() % ARR_SIZE(NICKS)];
        printf("user addr: %p - sockfd addr:%p\n", user, &user->sockfd);

        pthread_t thread;
        int err = pthread_create(&thread, NULL, (void*)&handle_connection, user);
        if (err < 0)
            print_error("can't create thread");
    }
    return EXIT;
}

void *
handle_connection(user_t* user)
{
    /* Store user in the user store */
    us_add(&user_store, user);

    char* msg = "Connection stablished...\n";
    int msg_len = strlen(msg);
    if (send(user->sockfd, msg, msg_len, 0) < 0) {
        print_error("failed when sending message");
        exit(errno);
    }

    char welcome_buffer[64];
    strcpy(welcome_buffer, "Welcome ");
    strcat(welcome_buffer, user->nick);
    strcat(welcome_buffer, "\n");
    send(user->sockfd, welcome_buffer, strlen(welcome_buffer), 0);
    send(user->sockfd, "> ", 2, 0);

    char msg_buffer[1024];
    while (recv(user->sockfd, &msg_buffer, 1024, 0)) {
        char res_buffer[1024];
        strcpy(res_buffer, user->nick);
        strcat(res_buffer, ": ");
        strcat(res_buffer, msg_buffer);

        int i;
        for (i = 0; i < user_store.size; i++)
            send(user_store.bag[i]->sockfd, res_buffer, strlen(res_buffer), 0);

        send(user->sockfd, "> ", 2, 0);
    }
    return NULL;
}

