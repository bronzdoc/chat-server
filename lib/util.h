#ifndef __SERVER_UTIL_H__
#define __SERVER_UTIL_H__

#define ARR_SIZE(arr) ( sizeof((arr)) / sizeof((arr[0])) )

void print_error(char* msg);
void print_info(char* msg);

#endif
