#ifndef __USER_H__
#define __USER_H_

#define MAX_USERS 20

typedef struct { int sockfd; char* nick; } user_t;
typedef struct { user_t* index[MAX_USERS]; user_t* bag[MAX_USERS]; int size;} user_store_t;

user_store_t create_user_store();
user_t* us_search(user_store_t* store, char* username);
int us_add(user_store_t* store, user_t* user);
int us_remove(user_store_t* store, user_t* user);
int _us_generate_key_();

#endif
