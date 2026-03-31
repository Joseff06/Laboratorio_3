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

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, BROKER_IP, &serv_addr.sin_addr);

    printf("=== PUBLICADOR UDP INICIADO ===\n");
    strcpy(msg.topic, "Nacional_vs_Millonarios");

    for (int i = 1; i <= 10; i++) {
        snprintf(msg.data, MAX_DATA, "Minuto %d: ¡Ataque peligroso! (Datagrama UDP)", i * 9);
        sendto(sockfd, &msg, sizeof(msg), 0, (const struct sockaddr *)&serv_addr, sizeof(serv_addr));
        printf("[ENVIADO] [%s] -> %s\n", msg.topic, msg.data);
        sleep(2);
    }

    printf("\nTransmisión finalizada.\n");
    close(sockfd);
    return 0;
}