#ifndef __SERVER_UTIL_H__
#define __SERVER_UTIL_H__
#define MAX_CONNECTIONS 5

void print_error(char* msg);
void print_info(char* msg);

int store_connection(int [MAX_CONNECTIONS], int);
void* perform();

#endif
