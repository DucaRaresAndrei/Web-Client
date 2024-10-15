#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "buffer.h"

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)


int verify_string(char *string, int type)
{
	if (!strcmp(string, "") || !strcmp(string, "\n") || (strlen(string) == 0)) {
		if (type == 0) {
			printf("ERROR: Username must contain at least one character!\n");
			return 0;

		} else {
			printf("ERROR: Password must contain at least one character!\n");
			return 0;
		}
	}

    for (int i = 0; i < strlen(string); i++)
    {
        if (string[i] == ' ')
        {
			if (type == 0) {
				printf("ERROR: Username cannot contain spaces!\n");
				return 0;

			} else {
				printf("ERROR: Password cannot contain spaces!\n");
				return 0;
			}
        }
    }

	return 1;
}

char *read_user_info()
{
	char username[LINELEN];
	char password[LINELEN];

	printf("username=");
	fgets(username, LINELEN, stdin);
	username[strlen(username) - 1] = '\0';

	printf("password=");
	fgets(password, LINELEN, stdin);
	password[strlen(password) - 1] = '\0';

	//check if the information respects the format, or if it displays an error
	if (verify_string(username, 0)) {
		if (verify_string(password, 1) == 0)
			return NULL;
	} else {
		return NULL;
	}

	//creates the JSON object with the user's information
	JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);

	return json_serialize_to_string_pretty(root_value);
}

//extract the cookie from the server response
char *set_cookie(char *response)
{
	char *cookie = strtok(strstr(response, "Set-Cookie:"), " ");

	return strtok(NULL, ";");
}

//extract the token from the server response
char *set_token(char *response)
{
	char *token = strstr(response, ":\"");
	token = strtok(token, "\"");

	return strtok(NULL, "\"");
}

//display the list of books in a JSON format
void print_books(char *books)
{
	JSON_Value *main_root_value = json_value_init_array();
    JSON_Array *main_books_array = json_value_get_array(main_root_value);

	JSON_Value *root_value = json_parse_string(books);
    JSON_Array *books_array = json_value_get_array(root_value);

	for (int i = 0; i < json_array_get_count(books_array); i++) {
		JSON_Object *root_object = json_array_get_object(books_array, i);

        const int id = json_object_get_number(root_object, "id");
        const char *title = json_object_get_string(root_object, "title");

		JSON_Value *root_book_value = json_value_init_object();
    	JSON_Object *book_object = json_value_get_object(root_book_value);

    	json_object_set_number(book_object, "id", id);
    	json_object_set_string(book_object, "title", title);

		json_array_append_value(main_books_array, root_book_value);
	}

	printf("%s\n", json_serialize_to_string_pretty(main_root_value));
}

int verify_number(char *id)
{
	if (!strcmp(id, "") || !strcmp(id, "\n") || (strlen(id) == 0)) {
		printf("ERROR: You have not entered any id!\n");
		return 0;
	}

	for (int i = 0; i < strlen(id); i++)
    {
        if (!isdigit(id[i])) {
			printf("ERROR: The id entered is not valid!\n");
			return 0;
		}
	}

	return 1;
}

char *read_id()
{
	char *id;
	id = calloc(LINELEN, sizeof(char));

	printf("id=");
	fgets(id, LINELEN, stdin);
	id[strlen(id) - 1] = '\0';

	if (verify_number(id))
		return id;

	return NULL;
}

void print_book(char *book, int id)
{
	JSON_Value *root_value = json_parse_string(book);
    JSON_Object *book_object = json_value_get_object(root_value);

	const char *title = json_object_get_string(book_object, "title");
    const char *author = json_object_get_string(book_object, "author");
	const char *publisher = json_object_get_string(book_object, "publisher");
    const char *genre = json_object_get_string(book_object, "genre");
    const int page_count = (int)json_object_get_number(book_object, "page_count");

	JSON_Value *print_book_value = json_value_init_object();
    JSON_Object *print_book_object = json_value_get_object(print_book_value);

	json_object_set_number(print_book_object, "id", id);
    json_object_set_string(print_book_object, "title", title);
	json_object_set_string(print_book_object, "author", author);
	json_object_set_string(print_book_object, "publisher", publisher);
	json_object_set_string(print_book_object, "genre", genre);
	json_object_set_number(print_book_object, "page_count", page_count);

	printf("%s\n", json_serialize_to_string_pretty(print_book_value));
}

//checks if an element of the book is empty
int verify_null_book(char *element)
{
	if (!strcmp(element, "\n") || (strlen(element) == 0))
		return 0;
	
	return 1;
}

void register_function(char *message, char *response, int sockfd, char *cookie)
{
	char *json_user = read_user_info();

	if (json_user == NULL)
		return;

	if (cookie) {
        printf("ERROR: You cannot register, you are already logged in!\n");
        return;
    }

	message = compute_post_request(SV_HOST, REGISTER_PATH, TYPE_OF_PAYLOAD,
									&json_user, 0, NULL, NULL);

	send_to_server(sockfd, message);

	response = receive_from_server(sockfd);

	if (basic_extract_json_response(response)) {
		printf("ERROR: Username has already been taken by someone else!\n");

	} else {
		printf("SUCCESS: The user has been registered!\n");
	}
}

void login_function(char *message, char *response, int sockfd, char **cookie)
{
	char *json_user = read_user_info();

	if (json_user == NULL)
		return;

	if (*cookie) {
        printf("ERROR: To be able to log in, you must first log out!\n");
        return;
    }

	message = compute_post_request(SV_HOST, LOGIN_PATH, TYPE_OF_PAYLOAD,
									&json_user, 0, NULL, NULL);

	send_to_server(sockfd, message);

	response = receive_from_server(sockfd);

	if (basic_extract_json_response(response)) {
		printf("ERROR: Username or Password do not match!\n");

	} else {
		*cookie = set_cookie(response);

		printf("SUCCESS: User has been logged in!\n");
	}
}

void enter_library_function(char *message, char *response, int sockfd, char *cookie,
							char **token)
{
	if (!cookie) {
        printf("ERROR: You must be logged in first!\n");
        return;
    }

	message = compute_get_request(SV_HOST, ACCESS_PATH, NULL, cookie , NULL);

	send_to_server(sockfd, message);

	response = basic_extract_json_response(receive_from_server(sockfd));

	if (strstr(response, "token")) {
		*token = set_token(response);

		printf("SUCCESS: You have received access to the library!\n");

	} else {
		printf("ERROR!\n");
	}
}

void get_books_function(char *message, char *response, int sockfd,
						char *cookie, char *token)
{
	if (!token) {
        printf("ERROR: You must first have access to the library!\n");
        return;
    }

	message = compute_get_request(SV_HOST, BOOKS_PATH, NULL,
									cookie, token);

	send_to_server(sockfd, message);

	response = receive_from_server(sockfd);

	char *check_error = basic_extract_json_response(response);

	if (check_error) {
		if (strstr(check_error, "error")) {
			printf("ERROR!\n");
			return;
		}

		char *books = strstr(response, "[");
		print_books(books);

	} else {
		printf("There are no books in the library!\n");
	}
}

void get_book_function(char *message, char *response, int sockfd,
						char *cookie, char *token)
{
	if (!token) {
        printf("ERROR: You must first have access to the library!\n");
        return;
    }

	char *check_id = read_id();

	if (!check_id)
		return;

	int id = atoi(check_id);

	char book_path[LINELEN];
	sprintf(book_path, "/api/v1/tema/library/books/%d", id);

	message = compute_get_request(SV_HOST, book_path, NULL,
									cookie, token);

	send_to_server(sockfd, message);

	response = receive_from_server(sockfd);

	char *check_error = basic_extract_json_response(response);

	if (strstr(check_error, "error")) {
		printf("ERROR: No book corresponding to this id was found!\n");
		return;
	}

	char *book = strstr(response, "{");
	print_book(book, id);
}

void add_book_function(char *message, char *response, int sockfd,
					   char *cookie, char *token)
{
	if (!token) {
        printf("ERROR: You must first have access to the library!\n");
        return;
    }

	char *title = calloc(LINELEN, sizeof(char));
	printf("title=");
    fgets(title, LINELEN, stdin);
	title[strlen(title) - 1] = '\0';

    char *author = calloc(LINELEN, sizeof(char));
	printf("author=");
    fgets(author, LINELEN, stdin);
	author[strlen(author) - 1] = '\0';

	char *genre = calloc(LINELEN, sizeof(char));
	printf("genre=");
    fgets(genre, LINELEN, stdin);
	genre[strlen(genre) - 1] = '\0';

	char *publisher = calloc(LINELEN, sizeof(char));
	printf("publisher=");
    fgets(publisher, LINELEN, stdin);
	publisher[strlen(publisher) - 1] = '\0';

    char *page_count_check = calloc(LINELEN, sizeof(char));
	printf("page_count=");
    fgets(page_count_check, LINELEN, stdin);
	page_count_check[strlen(page_count_check) - 1] = '\0';

	if (!(verify_null_book(title) && verify_null_book(author) && verify_null_book(genre) &&
		verify_null_book(publisher) && verify_null_book(page_count_check))) {
		printf("ERROR: The entered data is incomplete!\n");
		return;
	}

	for (int i = 0; i < strlen(page_count_check); i++) {
        if (!isdigit(page_count_check[i])) {
			printf("ERROR: The entered data does not respect the formatting!\n");
			return;
		}
	}

	int page_count = atoi(page_count_check);

	JSON_Value *book_value = json_value_init_object();
    JSON_Object *book_object = json_value_get_object(book_value);

    json_object_set_string(book_object, "title", title);
    json_object_set_string(book_object, "author", author);
    json_object_set_string(book_object, "publisher", publisher);
    json_object_set_string(book_object, "genre", genre);
    json_object_set_number(book_object, "page_count", page_count);

    char *json_book = json_serialize_to_string_pretty(book_value);

	message = compute_post_request(SV_HOST, BOOKS_PATH, TYPE_OF_PAYLOAD,
									&json_book, 0, cookie, token);

	send_to_server(sockfd, message);

	response = receive_from_server(sockfd);

	if (basic_extract_json_response(response)) {
		printf("ERROR!\n");

	} else {
		printf("SUCCESS: The book has been added to the library!\n");
	}
}

void delete_book_function(char *message, char *response, int sockfd,
						  char *cookie, char *token)
{
	if (!token) {
        printf("ERROR: You must first have access to the library!\n");
        return;
    }

	char *check_id = read_id();

	if (!check_id)
		return;

	int id = atoi(check_id);

	char book_path[LINELEN];
	sprintf(book_path, "/api/v1/tema/library/books/%d", id);

	message = compute_delete_request(SV_HOST, book_path, cookie, token);

	send_to_server(sockfd, message);

	response = receive_from_server(sockfd);

	if (basic_extract_json_response(response)) {
		printf("ERROR: There is no book with this id in the library!\n");

	} else {
		printf("SUCCESS: The book has been removed from the library!\n");
	}
}

void logout_function(char *message, char *response, int sockfd, char **cookie,
					 char **token)
{
	if (!(*cookie)) {
        printf("ERROR: No user is logged in!\n");
        return;
    }

	message = compute_get_request(SV_HOST, LOGOUT_PATH, NULL, *cookie, *token);

	send_to_server(sockfd, message);

	response = receive_from_server(sockfd);

	if (basic_extract_json_response(response)) {
		printf("ERROR!\n");

	} else {
		*cookie = NULL;
		*token = NULL;

		printf("SUCCESS: The user has been logged out!\n");
	}
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void compute_message(char *message, const char *line)
{
    strcat(message, line);
    strcat(message, "\r\n");
}

int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag)
{
    struct sockaddr_in serv_addr;
    int sockfd = socket(ip_type, socket_type, flag);
    if (sockfd < 0)
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(portno);
    inet_aton(host_ip, &serv_addr.sin_addr);

    /* connect the socket */
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;
}

void close_connection(int sockfd)
{
    close(sockfd);
}

void send_to_server(int sockfd, char *message)
{
    int bytes, sent = 0;
    int total = strlen(message);

    do
    {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            error("ERROR writing message to socket");
        }

        if (bytes == 0) {
            break;
        }

        sent += bytes;
    } while (sent < total);
}

char *receive_from_server(int sockfd)
{
    char response[BUFLEN];
    buffer buffer = buffer_init();
    int header_end = 0;
    int content_length = 0;

    do {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0){
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
        
        header_end = buffer_find(&buffer, HEADER_TERMINATOR, HEADER_TERMINATOR_SIZE);

        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;
            
            int content_length_start = buffer_find_insensitive(&buffer, CONTENT_LENGTH, CONTENT_LENGTH_SIZE);
            
            if (content_length_start < 0) {
                continue;           
            }

            content_length_start += CONTENT_LENGTH_SIZE;
            content_length = strtol(buffer.data + content_length_start, NULL, 10);
            break;
        }
    } while (1);
    size_t total = content_length + (size_t) header_end;
    
    while (buffer.size < total) {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
    }
    buffer_add(&buffer, "", 1);
    return buffer.data;
}

char *basic_extract_json_response(char *str)
{
    return strstr(str, "{\"");
}
