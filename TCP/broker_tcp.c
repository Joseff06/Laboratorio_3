#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#include "../mensajes.h" // Incluimos la estructura compartida

#define PORT 8080
#define MAX_SUBS 100

// Estructura para almacenar los suscriptores activos
typedef struct {
    int socket;
    char topic[MAX_TOPIC];
} Subscriber;

Subscriber subscribers[MAX_SUBS];
int sub_count = 0;

// Mutex para evitar problemas de concurrencia al modificar la lista de suscriptores
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Función que manejará la conexión de cada cliente en un hilo separado
void *handle_client(void *arg) {
    int client_sock = *(int*)arg;
    free(arg); // Liberamos la memoria reservada para el puntero del socket
    MensajeDeportivo msg;
    int read_size;

    // Leemos el primer mensaje. Esto nos dirá la intención del cliente.
    if ((read_size = recv(client_sock, &msg, sizeof(MensajeDeportivo), 0)) > 0) {
        
        // Estrategia: Si el campo 'data' dice "SUBSCRIBE", es un Suscriptor.
        if (strcmp(msg.data, "SUBSCRIBE") == 0) {
            
            pthread_mutex_lock(&mutex); // Bloqueamos para escritura segura
            if (sub_count < MAX_SUBS) {
                subscribers[sub_count].socket = client_sock;
                strcpy(subscribers[sub_count].topic, msg.topic);
                sub_count++;
                printf("[BROKER] Nuevo suscriptor registrado para el partido: %s\n", msg.topic);
            }
            pthread_mutex_unlock(&mutex); // Desbloqueamos
            
            // Mantenemos la conexión TCP abierta. recv() se bloqueará hasta que 
            // el cliente cierre la conexión (devolviendo 0).
            while((read_size = recv(client_sock, &msg, sizeof(MensajeDeportivo), 0)) > 0) {
                // Los suscriptores solo escuchan, no envían más datos.
            }
            printf("[BROKER] Un suscriptor se ha desconectado.\n");
            
        } else {
            // Si el primer mensaje NO es "SUBSCRIBE", asumimos que es un Publicador
            // enviando su primera noticia deportiva.
            printf("[BROKER] Noticia recibida de [%s]: %s\n", msg.topic, msg.data);
            
            // Reenviamos a todos los suscriptores cuyo tema coincida
            pthread_mutex_lock(&mutex);
            for (int i = 0; i < sub_count; i++) {
                if (strcmp(subscribers[i].topic, msg.topic) == 0) {
                    send(subscribers[i].socket, &msg, sizeof(MensajeDeportivo), 0);
                }
            }
            pthread_mutex_unlock(&mutex);

            // Bucle para seguir escuchando los siguientes mensajes de este mismo publicador
            while((read_size = recv(client_sock, &msg, sizeof(MensajeDeportivo), 0)) > 0) {
                printf("[BROKER] Noticia recibida de [%s]: %s\n", msg.topic, msg.data);
                
                pthread_mutex_lock(&mutex);
                for (int i = 0; i < sub_count; i++) {
                    if (strcmp(subscribers[i].topic, msg.topic) == 0) {
                        send(subscribers[i].socket, &msg, sizeof(MensajeDeportivo), 0);
                    }
                }
                pthread_mutex_unlock(&mutex);
            }
            printf("[BROKER] Un publicador ha finalizado su transmisión.\n");
        }
    }
    
    // Al salir del bucle (el cliente se desconectó), cerramos su socket particular
    close(client_sock);
    return NULL;
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // 1. Crear el socket TCP (AF_INET para IPv4, SOCK_STREAM para TCP)
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección y puerto del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Escuchar en cualquier interfaz
    server_addr.sin_port = htons(PORT);

    // 2. Asociar el socket al puerto especificado (Bind)
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en el bind");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // 3. Poner el socket en modo escucha (Listen)
    if (listen(server_sock, 10) < 0) {
        perror("Error en el listen");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("=== BROKER TCP INICIADO ===\n");
    printf("Esperando conexiones en el puerto %d...\n\n", PORT);

    // 4. Bucle infinito para aceptar conexiones entrantes
    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Error al aceptar la conexión");
            continue;
        }

        // Reservamos memoria para el descriptor del socket del cliente para pasarlo al hilo
        int *new_sock = malloc(sizeof(int));
        *new_sock = client_sock;

        // Creamos un hilo para atender a este cliente específico
        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handle_client, (void*)new_sock) < 0) {
            perror("Error al crear el hilo");
            free(new_sock);
            close(client_sock);
            continue;
        }
        
        // Hacemos detach al hilo para que el sistema libere sus recursos al terminar
        pthread_detach(client_thread); 
    }

    close(server_sock);
    return 0;
}