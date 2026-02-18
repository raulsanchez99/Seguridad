#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

// Función para manejar la autenticación del cliente
void authenticate(const char *username, const char *password, const char *server_address, int port);

int main(int argc, char *argv[]) {
    // Verificación de argumentos de línea de comandos
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <username> <password> <server_address> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Extracción de los argumentos de la línea de comandos
    const char *username = argv[1];
    const char *password = argv[2];
    const char *server_address = argv[3];
    int port = atoi(argv[4]);

    // Autenticación del cliente
    authenticate(username, password, server_address, port);

    return EXIT_SUCCESS;
}

// Función para manejar la autenticación del cliente
void authenticate(const char *username, const char *password, const char *server_address, int port) {
    // Creación del socket del cliente
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configuración de la dirección del servidor
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_address);
    server_addr.sin_port = htons(port);

    // Conexión al servidor
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Construcción del mensaje de autenticación
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%s:%s", username, password);

    // Envío del mensaje al servidor
    if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }

    // Recepción de la respuesta del servidor
    if (recv(client_socket, buffer, BUFFER_SIZE, 0) == -1) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    }

    // Impresión del resultado de la autenticación
    printf("AUTHENTICATION: %s\n", buffer);

    // Cierre del socket del cliente
    close(client_socket);
}

