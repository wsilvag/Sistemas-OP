#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include "../include/chat.h"

// Externo (implementado en ipc_sockets.c)
int connect_to_server_socket();
// Externo (implementado en message.c)
int send_protocol_message(int fd, message_t *msg);
int receive_protocol_message(int fd, message_t *msg);

// ... implementaciones de funciones de manejo de UI/Comandos de cliente

int main(int argc, char *argv[]) {
    int server_fd;
    char username[USERNAME_MAX];

    printf("=== Chat Client ===\n");
    printf("Username: ");
    if (!fgets(username, USERNAME_MAX, stdin)) {
        return 1;
    }
    username[strcspn(username, "\n")] = 0; // Remover newline

    server_fd = connect_to_server_socket();
    if (server_fd < 0) {
        return 1;
    }
    printf("Connected to server!\n");

    // 1. Enviar MSG_JOIN
    message_t join_msg;
    join_msg.type = MSG_JOIN;
    strncpy(join_msg.from_user, username, USERNAME_MAX);
    send_protocol_message(server_fd, &join_msg);

    int running = 1;
    
    // Bucle principal de E/S
    while (running) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds); // Entrada de usuario
        FD_SET(server_fd, &readfds);   // Entrada del servidor
        
        int max_fd = (server_fd > STDIN_FILENO) ? server_fd : STDIN_FILENO;
        struct timeval timeout = {0, 500000}; // 0.5 segundos de timeout

        // Imprimir prompt solo si no hay entrada pendiente
        if (!FD_ISSET(STDIN_FILENO, &readfds)) {
             printf("%s> ", username);
             fflush(stdout);
        }

        if (select(max_fd + 1, &readfds, NULL, NULL, &timeout) > 0) {
            
            // A. Recepción de Mensajes del Servidor
            if (FD_ISSET(server_fd, &readfds)) {
                message_t msg;
                int bytes = receive_protocol_message(server_fd, &msg);
                
                if (bytes > 0) {
                    // ... Mostrar el mensaje (lógica de UI/Output)
                    // Ejemplo simple:
                    printf("\n[%s] %s\n", msg.from_user, msg.text);
                } else if (bytes == 0) {
                    printf("\n[SYSTEM] Server disconnected. Goodbye!\n");
                    running = 0;
                    break;
                }
            }
            
            // B. Entrada de Usuario (Comandos o Mensajes)
            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                char input[MSG_MAX_SIZE + 50];
                if (fgets(input, sizeof(input), stdin)) {
                    input[strcspn(input, "\n")] = 0;
                    
                    // Procesar input (enviar mensaje o ejecutar comando /quit, /msg, /users)
                    // Ejemplo de /quit
                    if (strcmp(input, "/quit") == 0) {
                        running = 0;
                    } else {
                        // Ejemplo: Tratar como BROADCAST
                        message_t send_msg;
                        send_msg.type = MSG_BROADCAST;
                        strncpy(send_msg.from_user, username, USERNAME_MAX);
                        strncpy(send_msg.text, input, MSG_MAX_SIZE);
                        send_protocol_message(server_fd, &send_msg);
                    }
                }
            }
        }
    }

    // 2. Enviar MSG_LEAVE y Limpieza
    message_t leave_msg;
    leave_msg.type = MSG_LEAVE;
    strncpy(leave_msg.from_user, username, USERNAME_MAX);
    send_protocol_message(server_fd, &leave_msg);
    
    close(server_fd);
    printf("[SYSTEM] Disconnected.\n");
    return 0;
}
