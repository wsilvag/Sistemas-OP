#ifndef MESSAGE_H
#define MESSAGE_H

#include <time.h>

#define MSG_MAX_SIZE 256
#define USERNAME_MAX 32
#define MAX_CLIENTS 100 // Límite de clientes

typedef enum {
    MSG_JOIN,           // Client joining
    MSG_LEAVE,          // Client leaving
    MSG_BROADCAST,      // Message to all
    MSG_PRIVATE,        // Private message
    MSG_LIST_USERS,     // Request user list
    MSG_USER_LIST,      // Response with users
    MSG_ERROR,          // Error message
    MSG_PING,           // Keep-alive
    MSG_PONG            // Keep-alive response
} message_type_t;

typedef struct {
    message_type_t type;
    char from_user[USERNAME_MAX];
    char to_user[USERNAME_MAX];
    char text[MSG_MAX_SIZE];
    time_t timestamp;
    int client_fd; // Usaremos FD para sockets, o PID para MQs
} message_t;

#endif // MESSAGE_H
