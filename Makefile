

default:
	gcc -g -Wall server.c lib/user.c lib/util.c -lpthread -o server
