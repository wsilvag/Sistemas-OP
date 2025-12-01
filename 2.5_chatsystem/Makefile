CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -Iinclude -pthread
LDFLAGS = -pthread

SERVER_SRC = src/chat_server.c src/ipc_sockets.c src/message.c src/utils.c
CLIENT_SRC = src/chat_client.c src/message.c src/utils.c

SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

.PHONY: all clean run_server run_client

all: chat_server chat_client

# Regla para compilar el Servidor
chat_server: $(SERVER_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Regla para compilar el Cliente
chat_client: $(CLIENT_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Reglas de compilación de objetos
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run_server: chat_server
	./chat_server

run_client: chat_client
	./chat_client

clean:
	rm -f chat_server chat_client $(SERVER_OBJ) $(CLIENT_OBJ)
	rm -f /tmp/chat.sock  # Limpiar socket UNIX
	@echo "Cleaned."
