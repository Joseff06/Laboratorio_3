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

    // 1. Crear el socket TCP
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n[ERROR] Creación del socket fallida\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convertir IP
    if (inet_pton(AF_INET, BROKER_IP, &serv_addr.sin_addr) <= 0) {
        printf("\n[ERROR] Dirección inválida o no soportada\n");
        return -1;
    }

    // 2. Conectar al Broker
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n[ERROR] Fallo en la conexión al Broker. ¿Está corriendo el servidor?\n");
        return -1;
    }

    printf("=== SUSCRIPTOR TCP CONECTADO ===\n");

    // 3. Enviar mensaje de suscripción
    // Como definimos en el Broker, si el 'data' es "SUBSCRIBE", lo registra en la lista
    strcpy(msg.data, "SUBSCRIBE");
    strcpy(msg.topic, "Nacional_vs_Millonarios"); // Debe coincidir con el topic del publisher
    
    send(sock, &msg, sizeof(msg), 0);
    printf("Suscrito exitosamente al partido: %s. Esperando noticias...\n\n", msg.topic);

    // 4. Bucle para recibir noticias en tiempo real
    int read_size;
    // recv() se bloquea aquí esperando a que lleguen datos por el socket
    while ((read_size = recv(sock, &msg, sizeof(msg), 0)) > 0) {
        printf("[NOTICIA EN VIVO - %s] %s\n", msg.topic, msg.data);
    }

    // Si recv devuelve 0, significa que el otro extremo cerró la conexión
    if (read_size == 0) {
        printf("\nEl Broker ha cerrado la conexión.\n");
    } else if (read_size == -1) {
        perror("\nError al recibir datos");
    }

    close(sock);
    return 0;
}