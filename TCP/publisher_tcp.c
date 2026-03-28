#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>          // Para la función sleep() y close()
#include <arpa/inet.h>       // Para inet_pton y estructuras de red
#include <sys/socket.h>

#include "../mensajes.h"     // Nuestra estructura compartida

#define PORT 8080
#define BROKER_IP "127.0.0.1" // Apunta a localhost para las pruebas en tu máquina virtual

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    MensajeDeportivo msg;

    // 1. Crear el socket TCP (SOCK_STREAM)
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n[ERROR] Creación del socket fallida\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convertir la dirección IP de texto a formato binario de red
    if (inet_pton(AF_INET, BROKER_IP, &serv_addr.sin_addr) <= 0) {
        printf("\n[ERROR] Dirección inválida o no soportada\n");
        return -1;
    }

    // 2. Establecer la conexión con el Broker (Aquí ocurre el 3-way handshake de TCP)
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n[ERROR] Fallo en la conexión al Broker. ¿Está corriendo?\n");
        return -1;
    }

    printf("=== PUBLICADOR TCP CONECTADO ===\n");
    printf("Iniciando transmisión del partido...\n\n");

    // Definimos el partido que este periodista va a cubrir
    strcpy(msg.topic, "Nacional_vs_Millonarios");

    // 3. Enviar 10 mensajes obligatorios según la guía
    for (int i = 1; i <= 10; i++) {
        // Formateamos un mensaje dinámico simulando minutos del partido
        snprintf(msg.data, MAX_DATA, "Minuto %d: ¡Actualización importante en el campo!", i * 9);
        
        // Enviamos la estructura completa por el socket
        send(sock, &msg, sizeof(msg), 0);
        printf("[ENVIADO] [%s] -> %s\n", msg.topic, msg.data);
        
        // Pausa de 2 segundos. Ideal para capturas limpias en Wireshark.
        sleep(2); 
    }

    // 4. Finalizar transmisión
    printf("\nPartido finalizado. Cerrando conexión.\n");
    close(sock);
    
    return 0;
}