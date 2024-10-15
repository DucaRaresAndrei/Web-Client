#ifndef _HELPERS_
#define _HELPERS_

#include "parson.h"
#include "requests.h"

#define BUFLEN 4096
#define LINELEN 1000

#define SV_HOST "34.246.184.49"
#define SV_PORT 8080

#define TYPE_OF_PAYLOAD "application/json"
#define REGISTER_PATH "/api/v1/tema/auth/register"
#define LOGIN_PATH "/api/v1/tema/auth/login"
#define LOGOUT_PATH "/api/v1/tema/auth/logout"
#define ACCESS_PATH "/api/v1/tema/library/access"
#define BOOKS_PATH "/api/v1/tema/library/books"

int verify_string(char *string, int type);

char *read_user_info();

char *set_cookie(char *response);

char *set_token(char *response);

void print_books(char *books);

int verify_number(char *id);

char *read_id();

void print_book(char *book, int id);

int verify_null_book(char *element);

void register_function(char *message, char *response, int sockfd, char *cookie);

void login_function(char *message, char *response, int sockfd, char **cookie);

void enter_library_function(char *message, char *response, int sockfd, char *cookie, char **token);

void get_books_function(char *message, char *response, int sockfd, char *cookie, char *token);

void get_book_function(char *message, char *response, int sockfd, char *cookie, char *token);

void add_book_function(char *message, char *response, int sockfd, char *cookie, char *token);

void delete_book_function(char *message, char *response, int sockfd, char *cookie, char *token);

void logout_function(char *message, char *response, int sockfd, char **cookie, char **token);

// shows the current error
void error(const char *msg);

// adds a line to a string message
void compute_message(char *message, const char *line);

// opens a connection with server host_ip on port portno, returns a socket
int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag);

// closes a server connection on socket sockfd
void close_connection(int sockfd);

// send a message to a server
void send_to_server(int sockfd, char *message);

// receives and returns the message from a server
char *receive_from_server(int sockfd);

// extracts and returns a JSON from a server response
char *basic_extract_json_response(char *str);

#endif
