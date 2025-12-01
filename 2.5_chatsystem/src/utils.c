#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "../include/chat.h"

// Variables externas definidas en chat_server.c
extern client_t *client_list;
extern pthread_mutex_t client_list_mutex; 
extern int running; // Para verificar el estado del servidor

// -------------------------------------------------------------------
// FUNCIONES DE LOGGING
// -------------------------------------------------------------------

void log_server_activity(const char *level, const char *msg) {
    time_t now = time(NULL);
    char *time_str = ctime(&now);
    // Remover el newline agregado por ctime
    time_str[strlen(time_str) - 1] = '\0'; 
    
    // Imprimir el log en formato: [Timestamp] [LEVEL] Message
    printf("[%s] [%s] %s\n", time_str, level, msg);
    fflush(stdout);
}

// -------------------------------------------------------------------
// GESTIÓN SEGURA DE CLIENTES (Sincronización - Parte 4)
// -------------------------------------------------------------------

void add_client_safe(client_t *client) {
    pthread_mutex_lock(&client_list_mutex);
    
    // Añadir al inicio de la lista enlazada
    client->next = client_list;
    client_list = client;
    
    pthread_mutex_unlock(&client_list_mutex);
}

void remove_client_safe(int fd) {
    pthread_mutex_lock(&client_list_mutex);

    client_t **current = &client_list;
    client_t *temp = NULL;

    // Buscar y remover el cliente de la lista
    while (*current) {
        if ((*current)->fd == fd) {
            temp = *current;
            *current = temp->next; // Enlazar el anterior con el siguiente
            // NOTA: El thread que maneja el cliente es responsable de close(fd) y free(temp)
            break;
        }
        current = &(*current)->next;
    }

    pthread_mutex_unlock(&client_list_mutex);
}

client_t* find_client_by_username(const char *username) {
    client_t *found = NULL;
    pthread_mutex_lock(&client_list_mutex);
    
    client_t *current = client_list;
    while (current) {
        if (strcmp(current->username, username) == 0) {
            found = current;
            break;
        }
        current = current->next;
    }
    
    pthread_mutex_unlock(&client_list_mutex);
    return found;
}

int get_active_client_count() {
    int count = 0;
    pthread_mutex_lock(&client_list_mutex);
    
    client_t *current = client_list;
    while (current) {
        count++;
        current = current->next;
    }
    
    pthread_mutex_unlock(&client_list_mutex);
    return count;
}

void get_client_list_string(char *buffer, size_t buffer_size) {
    buffer[0] = '\0';
    pthread_mutex_lock(&client_list_mutex);
    
    client_t *current = client_list;
    while (current) {
        // Añadir el nombre de usuario al buffer
        strncat(buffer, current->username, buffer_size - strlen(buffer) - 1);
        if (current->next) {
            strncat(buffer, ", ", buffer_size - strlen(buffer) - 1);
        }
        current = current->next;
    }
    
    pthread_mutex_unlock(&client_list_mutex);
}


// -------------------------------------------------------------------
// LÓGICA DE COMANDOS DEL SERVIDOR (Parcial)
// -------------------------------------------------------------------

void handle_server_commands(const char *input) {
    if (strcmp(input, "/list") == 0) {
        char user_list[1024];
        get_client_list_string(user_list, sizeof(user_list));
        log_server_activity("SERVER", "Connected users:");
        printf("    %s\n", user_list);
    } else if (strcmp(input, "/stats") == 0) {
        int count = get_active_client_count();
        log_server_activity("SERVER", "Statistics:");
        printf("    Active users: %d\n", count);
        // ... añadir total de mensajes, etc.
    } else if (strcmp(input, "/shutdown") == 0) {
        // Esto debe forzar el cierre del bucle principal en chat_server.c
        extern int running;
        running = 0;
        log_server_activity("SERVER", "Initiating graceful shutdown...");
    } else {
        log_server_activity("SERVER", "Unknown command or broadcast feature not fully implemented yet.");
    }
}

// -------------------------------------------------------------------
// LÓGICA DE MENSAJERÍA (Parcial)
// -------------------------------------------------------------------

void broadcast_message(message_t *msg) {
    pthread_mutex_lock(&client_list_mutex);
    
    client_t *current = client_list;
    char log_msg[512];
    snprintf(log_msg, sizeof(log_msg), "MSG] %s -> ALL: %s", msg->from_user, msg->text);
    log_server_activity("MSG", log_msg);

    // Iterar sobre todos los clientes y enviar el mensaje
    while (current) {
        // No enviar el mensaje de broadcast al remitente original, si es necesario
        if (strcmp(current->username, msg->from_user) != 0) {
            // Usar send_protocol_message (definido en message.c)
            if (send_protocol_message(current->fd, msg) == -1) {
                // Manejar error de envío (e.g., cliente muerto)
                if (errno != EPIPE && errno != ECONNRESET) {
                    // Si hay error en el envío, el thread de cliente_handler debe manejar la desconexión
                }
            }
        }
        current = current->next;
    }

    pthread_mutex_unlock(&client_list_mutex);
}
