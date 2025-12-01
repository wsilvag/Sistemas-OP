#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "../include/message.h"

// Macro para manejar interrupciones de señales (EINTR)
#define HANDLE_EINTR(func, args) ({ \
    ssize_t res; \
    do { \
        res = func args; \
    } while (res == -1 && errno == EINTR); \
    res; \
})

// Envía la estructura message_t completa a través del FD
// Retorna el número de bytes enviados o -1 en caso de error.
int send_protocol_message(int fd, message_t *msg) {
    // 1. Asegurar que el timestamp esté actualizado
    msg->timestamp = time(NULL); 
    
    size_t len = sizeof(message_t);
    const char *buffer = (const char *)msg;
    ssize_t sent_bytes;
    
    // Usar la macro para manejar EINTR
    sent_bytes = HANDLE_EINTR(send, (fd, buffer, len, 0)); 
    
    if (sent_bytes == -1) {
        // En un chat, EPIPE/ECONNRESET significan que el cliente/servidor se fue
        if (errno != EPIPE && errno != ECONNRESET) {
            perror("[MSG ERROR] send");
        }
    }
    
    return (int)sent_bytes;
}

// Recibe la estructura message_t completa a través del FD
// Retorna el número de bytes recibidos, 0 si se cerró la conexión, o -1 en caso de error.
int receive_protocol_message(int fd, message_t *msg) {
    size_t len = sizeof(message_t);
    char *buffer = (char *)msg;
    ssize_t received_bytes;

    // Usar la macro para manejar EINTR
    // MSG_WAITALL asegura que recibamos la estructura completa o falle
    received_bytes = HANDLE_EINTR(recv, (fd, buffer, len, MSG_WAITALL));
    
    if (received_bytes == -1) {
        // Ignorar EWOULDBLOCK/EAGAIN si el socket es non-blocking
        if (errno != EWOULDBLOCK && errno != EAGAIN && errno != EINTR) { 
            perror("[MSG ERROR] recv");
        }
    } else if (received_bytes > 0 && received_bytes != len) {
        // Esto indica un error de protocolo o un socket corrupto
        fprintf(stderr, "[MSG ERROR] Protocol mismatch: Received %zd bytes instead of %zu\n", received_bytes, len);
        return -1;
    }

    return (int)received_bytes;
}
