#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "../include/chat.h"
#include "../include/message.h"

// Declaración global del mutex y la lista de clientes (para usar en chat.h)
pthread_mutex_t client_list_mutex = PTHREAD_MUTEX_INITIALIZER;
client_t *client_list = NULL;

int server_fd = -1;
int running = 1;

// Externo (implementado en ipc_sockets.c)
int setup_server_socket();
// Externo (implementado en utils.c)
void handle_server_commands(const char *input);
void broadcast_message(message_t *msg);
// Externo (implementado en message.c)
int send_protocol_message(int fd, message_t *msg);
int receive_protocol_message(int fd, message_t *msg);

// Función para manejar la comunicación con un cliente específico
void *client_handler(void *arg) {
    client_t *client = (client_t *)arg;
    message_t msg;
    
    // 1. Fase de Autenticación (recibir MSG_JOIN con el username)
    if (receive_protocol_message(client->fd, &msg) > 0 && msg.type == MSG_JOIN) {
        strncpy(client->username, msg.from_user, USERNAME_MAX - 1);
        add_client_safe(client);
        
        char log_msg[512];
        snprintf(log_msg, sizeof(log_msg), "Client '%s' connected (FD: %d)", client->username, client->fd);
        log_server_activity("INFO", log_msg);
        
        // Broadcast de JOIN a todos
        broadcast_message(&msg);
    } else {
        log_server_activity("ERROR", "Failed to receive JOIN message or invalid type.");
        remove_client_safe(client->fd);
        close(client->fd);
        free(client);
        return NULL;
    }

    // 2. Bucle de Recepción de Mensajes (mantener conexión)
    while (running) {
        int bytes_received = receive_protocol_message(client->fd, &msg);
        
        if (bytes_received > 0) {
            // Procesar mensaje (MSG_BROADCAST, MSG_PRIVATE, MSG_LIST_USERS, etc.)
            
            // Ejemplo de broadcast
            if (msg.type == MSG_BROADCAST) {
                broadcast_message(&msg);
            }
            // ... (implementar MSG_PRIVATE, MSG_LIST_USERS, etc.)
            
        } else if (bytes_received == 0) {
            // Cliente se desconectó amablemente (cierre de socket)
            break; 
        } else if (bytes_received == -1 && errno != EINTR && errno != EWOULDBLOCK) {
            // Error en el socket (desconexión forzada)
            break;
        }
        
        // Agregar un pequeño sleep o select aquí si no se usa un socket no bloqueante
    }

    // 3. Limpieza y Salida
    char log_msg[512];
    snprintf(log_msg, sizeof(log_msg), "Client '%s' disconnected (FD: %d)", client->username, client->fd);
    log_server_activity("INFO", log_msg);
    
    remove_client_safe(client->fd);
    close(client->fd);
    free(client);
    return NULL;
}

void graceful_shutdown(int sig) {
    if (sig == SIGINT) {
        log_server_activity("SERVER", "Shutdown signal received. Shutting down gracefully...");
    }
    running = 0;
}

int main(int argc, char *argv[]) {
    // Manejar SIGINT (Ctrl+C)
    signal(SIGINT, graceful_shutdown);

    server_fd = setup_server_socket();
    if (server_fd < 0) {
        return 1;
    }

    // Bucle de Aceptación de Clientes
    while (running) {
        // Usar select() para manejar la entrada de comandos del servidor
        // Y la llegada de nuevas conexiones
        
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);
        
        int max_fd = server_fd;
        struct timeval timeout = {1, 0}; // Timeout de 1 segundo
        
        if (select(max_fd + 1, &readfds, NULL, NULL, &timeout) > 0) {
            
            // 1. Nueva Conexión
            if (FD_ISSET(server_fd, &readfds)) {
                struct sockaddr_un client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

                if (client_fd > 0) {
                    client_t *new_client = (client_t *)malloc(sizeof(client_t));
                    new_client->fd = client_fd;
                    new_client->next = NULL; 

                    // Crear un thread para manejar al cliente
                    if (pthread_create(&new_client->thread_id, NULL, client_handler, (void *)new_client) != 0) {
                        perror("[SERVER ERROR] pthread_create");
                        close(client_fd);
                        free(new_client);
                    } else {
                        pthread_detach(new_client->thread_id); // El thread se limpia solo al terminar
                    }
                }
            }

            // 2. Comando de Servidor
            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                char input[256];
                if (fgets(input, sizeof(input), stdin)) {
                    // Remover newline
                    input[strcspn(input, "\n")] = 0; 
                    handle_server_commands(input); 
                }
            }
        }
    }

    // Limpieza final
    close(server_fd);
    unlink(SOCKET_PATH);
    // ... liberar client_list ...
    log_server_activity("SERVER", "Server shutdown complete.");
    return 0;
}
