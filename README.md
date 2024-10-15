**Duca Andrei-Rares**  
**321CA**

# WEB CLIENT

## Algorithm:

Simulates an online library where users can create accounts, log in, and access information about books. The HTTP protocol is used to communicate with the server, and access to future actions is established through JSON and JWT tokens.

## Implementation:

The main functions are implemented in the helpers.c file. We also use JSON format handling, through the library whose skeleton is in parson.c.

### client.c

The connection to the server is established, and the functions corresponding to the client's operation are called.

### read_user_info

User information is read, and errors are displayed if the format is incorrect.

### register_function

Registers a user, and in case of failure, handles errors.

### login_function

First checks if the user exists, and then, if there are no other errors, allows the user to log in. It also sets the cookie.

### enter_library_function

Checks if a user is logged in and grants access to the library. It sets the access token.

### get_books_function + get_book_function

Displays information about one or all books in JSON format.

### add_book_function

Adds a book to the library if it respects the format.

### delete_book_function

Deletes a book from the library by its ID.

### logout_function

Logs the user out from the server. The login cookie and the library access token are also reset.