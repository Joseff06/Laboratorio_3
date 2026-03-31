#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../mensajes.h"

#define PORT 8080
#define MAX_SUBS 100

typedef struct {
    struct sockaddr_in addr;
    char topic[MAX_TOPIC];
} UdpSubscriber;

UdpSubscriber subscribers[MAX_SUBS];
int sub_count = 0;

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    MensajeDeportivo msg;
    socklen_t client_len = sizeof(client_addr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en el bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("=== BROKER UDP INICIADO ===\n");
    printf("Escuchando datagramas en el puerto %d...\n\n", PORT);

    while (1) {
        int n = recvfrom(sockfd, &msg, sizeof(MensajeDeportivo), 0, 
                         (struct sockaddr *)&client_addr, &client_len);
        
        if (n > 0) {
            if (strcmp(msg.data, "SUBSCRIBE") == 0) {
                if (sub_count < MAX_SUBS) {
                    subscribers[sub_count].addr = client_addr;
                    strcpy(subscribers[sub_count].topic, msg.topic);
                    sub_count++;
                    printf("[BROKER] Nuevo suscriptor registrado para: %s\n", msg.topic);
                }
            } else {
                printf("[BROKER] Noticia de [%s]: %s\n", msg.topic, msg.data);
                
                for (int i = 0; i < sub_count; i++) {
                    if (strcmp(subscribers[i].topic, msg.topic) == 0) {
                        sendto(sockfd, &msg, sizeof(MensajeDeportivo), 0, 
                               (const struct sockaddr *)&subscribers[i].addr, sizeof(client_addr));
                    }
                }
            }
        }
    }

    close(sockfd);
    return 0;
}