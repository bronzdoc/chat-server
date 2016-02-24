

default:
	gcc -g -Wall server.c lib/user.c -lpthread -o server
