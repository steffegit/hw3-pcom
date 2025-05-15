# Movie Library Client (4th Homework Communication Networks class)

## Overview

This project implements a C++ client for interacting with a movie library server via HTTP requests. The client supports both admin and user functionalities, such as authentication, user management, and CRUD operations for movies and collections. The communication with the server is performed using sockets, and the client constructs and parses HTTP requests and responses manually.

## Features

- **Admin Operations:**
  - Login as admin
  - Logout as admin
  - Add users
  - List users
  - Delete users

- **User Operations:**
  - Login/logout as user
  - Obtain access tokens (JWT)
  - List, add, update, and delete movies
  - List, add, and delete collections
  - Add or remove movies from collections

- **Input Validation:**
  - Usernames and passwords cannot contain spaces or be empty.
  - IDs and numeric fields are validated for correct format.

## Implementation Details

### HTTP Communication

The client uses low-level socket programming to connect to the server, send HTTP requests, and receive responses. Helper functions are used to construct HTTP requests for different methods (GET, POST, PUT, DELETE) and to parse the server's responses.

### HTTP Request Construction

The client implements a set of overloaded functions for constructing different types of HTTP requests. These functions handle the details of formatting HTTP headers, including cookies and authorization tokens, and serializing JSON data for request bodies.

#### Function Overloading Strategy

The request builder functions are overloaded to handle different authentication scenarios:

1. **Basic Authentication**: The base versions of these functions accept cookies for session-based authentication, used primarily for admin operations.
2. **JWT Authentication**: The overloaded versions include an additional `jwt_token` parameter for token-based authentication, used for authorized user operations.

This overloading approach provides several benefits:

- **Separation of Concerns**: Clearly distinguishes between different authentication methods
- **Code Reusability**: Avoids duplication while maintaining specialized behavior
- **API Clarity**: Makes the authentication requirements explicit at the call site
- **Flexibility**: Allows for easy extension to support additional authentication methods
The implementation could be enhanced by extracting shared functionality into common utility methods, reducing duplication between the overloaded functions.

These functions handle all the details of constructing properly formatted HTTP/1.1 requests, including:
- Setting appropriate Content-Type headers
- Adding Authorization headers for JWT tokens
- Including Cookie headers for session management
- Calculating Content-Length for request bodies
- Using Connection: keep-alive for persistent connections
- Formatting the request according to HTTP/1.1 specifications

### Command Loop

The main function runs a command loop, reading commands from standard input and dispatching them to the appropriate handler functions. The following commands are supported:

1. Admin Commands
   a. login_admin - authenticate as admin
   b. add_user - add a new regular user
   c. get_users - get all users from server
   d. delete_user - delete a regular user
   e. logout_admin - logout admin session

2. User Authentication Commands
   a. login - authenticate as regular user
   b. get_access - request access to movie collection
   c. logout - logout user session

3. Movie Management Commands
   a. get_movies - get all movies from server
   b. get_movie - get information about a specific movie
   c. add_movie - add a new movie
   d. delete_movie - delete a movie
   e. update_movie - update movie details

4. Collection Management Commands
   a. get_collections - get all movie collections from server
   b. get_collection - get information about a specific collection
   c. add_collection - add a new movie collection
   d. delete_collection - delete a movie collection
   e. add_movie_to_collection - add a movie to a collection
   f. delete_movie_from_collection - remove a movie from a collection

5. System Commands
   a. exit - exit the program

### Input Validation

All user input is validated before sending requests to the server. For example, usernames and passwords are checked to ensure they do not contain spaces and are not empty. Numeric fields are parsed and validated to prevent invalid requests.

### JSON Parsing

#### Library Choice: nlohmann/json

For JSON parsing and serialization, we use the [nlohmann/json](https://github.com/nlohmann/json) library. This library was chosen for several reasons:

- **Ease of Use:** The syntax is intuitive and similar to working with standard C++ containers. For example, JSON objects can be constructed and accessed using familiar `[]` operators.
- **Header-only:** The library is header-only, which simplifies integration and avoids additional build dependencies.
- **Performance:** It provides efficient parsing and serialization, suitable for real-time client-server communication.
- **Community Support:** nlohmann/json is one of the most popular and well-maintained C++ JSON libraries, with extensive documentation and community support.

#### Usage Example

Parsing a JSON response from the server:
```cpp
json response_json = json::parse(body);
std::string token = response_json["token"];
```

Constructing a JSON request body:
```cpp
json body_data = {{"username", username}, {"password", password}};
```

Dumping the JSON data:
```cpp
std::string json_str = body_data.dump();  // Convert to string with default formatting
std::string pretty_json = body_data.dump(2);  // Convert to string with 2-space indentation
```

Working with JSON arrays:
```cpp
for (const auto& movie : response_json["movies"]) {
    std::cout << "Title: " << movie["title"] << std::endl;
    std::cout << "Year: " << movie["year"] << std::endl;
}
```


### Error Handling

The client provides clear error messages for invalid input, failed requests, and server errors. All network and parsing operations are wrapped in try-catch blocks where appropriate to ensure robustness.

The error handling could be enhanced by implementing specific handling for all possible server error codes, enabling more detailed and informative error messages.

## How to Run

The project includes the `nlohmann/json` library in its source files, so no additional installation steps are required. To build and run the client:
```sh
make
./client
```

## Conclusion

This client demonstrates robust handling of HTTP communication, user input validation, and JSON parsing in C++. The use of the nlohmann/json library greatly simplifies working with JSON data, making the code more readable and maintainable.