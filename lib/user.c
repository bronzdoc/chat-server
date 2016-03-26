#include "stdio.h"
#include "string.h"
#include "user.h"

/* Create a new user store */
user_store_t
create_user_store() {
    user_store_t store;
    memset(&store, '\0', sizeof store);
    store.size = 0;
    return store;
}

/* Search user in user store */
user_t*
us_search(user_store_t* store, char* username)
{
    int key =_us_generate_key_(username);
    return store->index[key];
}

/* Add user to the user store */
int
us_add(user_store_t* store, user_t* user)
{
    int key =_us_generate_key_(user);
    if (store->index[key] == '\0') {
        store->index[key] = user;
        store->bag[store->size++] = user;
        return 1;
    } else {
        return 0;
    }
}

/* Remove user from user store */
int
us_remove(user_store_t* store, user_t* user)
{
    int key =_us_generate_key_(user);
    if (store->index[key] != NULL) {
        store->index[key] = NULL;
        return 1;
    } else {
        return 0;
    }
}

/* Crappy hash function */
int
_us_generate_key_(user_t* user)
{
    // TODO use a proper algorithm to generate hash later...
    /* Generate key */
    int key = 0;
    int i;
    int len = strlen(user->nick);

    for (i = 0; i <= len ; i++) {
        key += user->nick[i];
    }
    return key % MAX_USERS;
}

