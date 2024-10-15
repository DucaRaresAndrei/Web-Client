#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

int main(int argc, char *argv[])
{
    char *message;
    char *response;

    int sockfd;

	char *cookie = NULL;
    char *token = NULL;

    while (1) {
		char command[LINELEN];
    	fgets(command, LINELEN, stdin);
		command[strlen(command) - 1] = '\0';

		sockfd = open_connection(SV_HOST, SV_PORT, AF_INET, SOCK_STREAM, 0);

		if (!strcmp(command, "register")) {
			register_function(message, response, sockfd, cookie);

		} else if (!strcmp(command, "login")) {
			login_function(message, response, sockfd, &cookie);

		} else if (!strcmp(command, "enter_library")) {
			enter_library_function(message, response, sockfd, cookie, &token);

		} else if (!strcmp(command, "get_books")) {
			get_books_function(message, response, sockfd, cookie, token);

		} else if (!strcmp(command, "get_book")) {
			get_book_function(message, response, sockfd, cookie, token);

		} else if (!strcmp(command, "add_book")) {
			add_book_function(message, response, sockfd, cookie, token);

		} else if (!strcmp(command, "delete_book")) {
			delete_book_function(message, response, sockfd, cookie, token);

		} else if (!strcmp(command, "logout")) {
			logout_function(message, response, sockfd, &cookie, &token);

		} else if (!strcmp(command, "exit")) {
			close_connection(sockfd);
			break;

		} else {
			printf("EROARE: Comanda introdusa este invalida!\n");
		}

		close_connection(sockfd);
	}

    return 0;
}
