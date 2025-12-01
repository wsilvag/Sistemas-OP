#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "../include/chat.h"

#define SOCKET_PATH "/tmp/chat.sock"

// Función para inicializar y escuchar en el servidor de sockets UNIX
int setup_server_socket() {
    int server_fd;
    struct sockaddr_un addr;

    // 1. Crear el socket
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("[SERVER ERROR] socket");
        return -1;
    }

    // Asegurarse de que el archivo de socket no exista
    unlink(SOCKET_PATH);

    // 2. Definir la dirección
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // 3. Enlazar (Bind)
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("[SERVER ERROR] bind");
        close(server_fd);
        return -1;
    }

    // 4. Escuchar (Listen)
    if (listen(server_fd, 20) == -1) { // 20 es el backlog de la cola de espera
        perror("[SERVER ERROR] listen");
        close(server_fd);
        return -1;
    }
    
    log_server_activity("SERVER", "Using IPC method: UNIX_SOCKETS");
    log_server_activity("SERVER", "Chat server listening on " SOCKET_PATH);
    
    return server_fd;
}

// Función para conectar el cliente al servidor de sockets UNIX
int connect_to_server_socket() {
    int client_fd;
    struct sockaddr_un addr;

    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("[CLIENT ERROR] socket");
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // Conectar
    if (connect(client_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        // Si el servidor no está corriendo, errno será ENOENT
        if (errno == ENOENT) {
            fprintf(stderr, "[CLIENT ERROR] Server socket not found at %s. Is the server running?\n", SOCKET_PATH);
        } else {
            perror("[CLIENT ERROR] connect");
        }
        close(client_fd);
        return -1;
    }

    return client_fd;
}

// Implementar funciones de envío/recepción (send_message, recv_message)
// ...
