#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../mensajes.h"

#define PORT 8080
#define BROKER_IP "127.0.0.1" // Apunta a localhost para las pruebas en tu máquina virtual

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
        printf("\n[ERROR] Fallo en la conexión al Broker. ¿Está corriendo?\n");
        return -1;
    }

    printf("=== PUBLICADOR TCP CONECTADO ===\n");
    printf("Iniciando transmisión del partido...\n\n");

    strcpy(msg.topic, "Nacional_vs_Millonarios");
    for (int i = 1; i <= 10; i++) {
        snprintf(msg.data, MAX_DATA, "Minuto %d: ¡Actualización importante en el campo!", i * 9);
        send(sock, &msg, sizeof(msg), 0);
        printf("[ENVIADO] [%s] -> %s\n", msg.topic, msg.data);
        sleep(2); 
    }

    printf("\nPartido finalizado. Cerrando conexión.\n");
    close(sock);
    
    return 0;
}