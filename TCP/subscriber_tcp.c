#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../mensajes.h" // Nuestra estructura compartida

#define PORT 8080
#define BROKER_IP "127.0.0.1"

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    MensajeDeportivo msg;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n[ERROR] Creación del socket fallida\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, BROKER_IP, &serv_addr.sin_addr) <= 0) {
        printf("\n[ERROR] Dirección inválida o no soportada\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n[ERROR] Fallo en la conexión al Broker. ¿Está corriendo el servidor?\n");
        return -1;
    }

    printf("=== SUSCRIPTOR TCP CONECTADO ===\n");

    strcpy(msg.data, "SUBSCRIBE");
    strcpy(msg.topic, "Nacional_vs_Millonarios"); // Debe coincidir con el topic del publisher
    
    send(sock, &msg, sizeof(msg), 0);
    printf("Suscrito exitosamente al partido: %s. Esperando noticias...\n\n", msg.topic);

    int read_size;
    while ((read_size = recv(sock, &msg, sizeof(msg), 0)) > 0) {
        printf("[NOTICIA EN VIVO - %s] %s\n", msg.topic, msg.data);
    }

    if (read_size == 0) {
        printf("\nEl Broker ha cerrado la conexión.\n");
    } else if (read_size == -1) {
        perror("Error al recibir datos");
    }

    close(sock);
    return 0;
}