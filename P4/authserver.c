#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

// Función para autenticar al cliente
void authenticate(FILE *accounts, int client_socket, struct sockaddr_in client_addr);

int main(int argc, char *argv[]) {
    // Verificación de argumentos de línea de comandos
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <accounts_file> [port]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Apertura del archivo de cuentas
    FILE *accounts = fopen(argv[1], "r");
    if (accounts == NULL) {
        perror("Error opening accounts file");
        return EXIT_FAILURE;
    }

    // Obtención del puerto del servidor
    int port = (argc == 3) ? atoi(argv[2]) : 9999;

    // Creación del socket del servidor
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    // Configuración de la dirección del servidor
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Enlace del socket del servidor a la dirección
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        return EXIT_FAILURE;
    }

    // Espera de conexiones entrantes
    if (listen(server_socket, 5) == -1) {
        perror("Listening failed");
        return EXIT_FAILURE;
    }

    printf("Server listening on port %d...\n", port);

    // Bucle de aceptación de clientes
    struct sockaddr_in client_addr = {0};
    socklen_t client_addr_len = sizeof(client_addr);
    int client_socket;

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }
        // Autenticación del cliente
        authenticate(accounts, client_socket, client_addr);
    }

    // Cierre del archivo de cuentas y el socket del servidor
    fclose(accounts);
    close(server_socket);

    return EXIT_SUCCESS;
}

// Función para autenticar al cliente
void authenticate(FILE *accounts, int client_socket, struct sockaddr_in client_addr) {
    char buffer[BUFFER_SIZE];
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    char client_ip[INET_ADDRSTRLEN];
    char *token;
    int auth_result = 0;

    // Obtención de la dirección IP del cliente
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

    // Recepción de datos del cliente
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    token = strtok(buffer, ":");
    strcpy(username, token);
    token = strtok(NULL, ":");
    strcpy(password, token);

    // Autenticación
    fseek(accounts, 0, SEEK_SET);
    while (fscanf(accounts, "%s:%s\n", buffer, password) == 2) {
        if (strcmp(buffer, username) == 0 && strcmp(password, password) == 0) {
            auth_result = 1;
            break;
        }
    }

    // Envío del resultado de autenticación al cliente
    if (auth_result)
        dprintf(client_socket, "SUCCESS, %s from %s\n", username, client_ip);
    else
        dprintf(client_socket, "FAILURE, %s from %s\n", username, client_ip);

    close(client_socket);
}

