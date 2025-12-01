#ifndef CHAT_H
#define CHAT_H

#include <pthread.h>
#include "message.h"

// Estructura para la lista de clientes (Gestión del Servidor)
typedef struct client {
    int fd;                             // Descriptor de Archivo o identificador IPC
    char username[USERNAME_MAX];
    time_t last_seen;
    pthread_t thread_id;                // ID del thread que maneja este cliente
    struct client *next;
} client_t;

// Mutex para proteger la lista de clientes
extern pthread_mutex_t client_list_mutex;

// Cabeceras de funciones de gestión de clientes (en utils.c)
void add_client_safe(client_t *client);
void remove_client_safe(int fd);
client_t* find_client_by_username(const char *username);
void log_server_activity(const char *level, const char *msg);
int get_active_client_count();
void get_client_list_string(char *buffer, size_t buffer_size);

#endif // CHAT_H
