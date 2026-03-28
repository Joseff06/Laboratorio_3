#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../mensajes.h"

#define PORT 8080
#define BROKER_IP "127.0.0.1"

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    MensajeDeportivo msg;

    // 1. Crear socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, BROKER_IP, &serv_addr.sin_addr);

    printf("=== SUSCRIPTOR UDP INICIADO ===\n");

    // 2. Enviar mensaje de suscripción
    strcpy(msg.data, "SUBSCRIBE");
    strcpy(msg.topic, "Nacional_vs_Millonarios");
    
    sendto(sockfd, &msg, sizeof(msg), 0, (const struct sockaddr *)&serv_addr, sizeof(serv_addr));
    printf("Solicitud de suscripción enviada para: %s\nEsperando datagramas...\n\n", msg.topic);

    // 3. Bucle para recibir noticias
    socklen_t len = sizeof(serv_addr);
    while (1) {
        int n = recvfrom(sockfd, &msg, sizeof(msg), 0, (struct sockaddr *)&serv_addr, &len);
        if (n > 0) {
            printf("[NOTICIA UDP EN VIVO - %s] %s\n", msg.topic, msg.data);
        }
    }

    close(sockfd);
    return 0;
}