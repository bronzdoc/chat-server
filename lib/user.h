#ifndef __USER_H__
#define __USER_H_

#define MAX_USERS 20

/* User type */
typedef struct user_t {
    int sockfd;
    char* nick;
} user;

typedef struct ser_store_t {
    user_t data[MAX_USERS];
} user_stroe;


/* user_store functions */
user_t us_search(*user_store store, user* user);
void us_add(*user_store store, user* user);
void us_remove(*user_store store, user* user);
void _us_generate_key_();

#endif
