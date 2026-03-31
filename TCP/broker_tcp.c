#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#include "../mensajes.h" 

#define PORT 8080
#define MAX_SUBS 100

typedef struct {
    int socket;
    char topic[MAX_TOPIC];
} Subscriber;

Subscriber subscribers[MAX_SUBS];
int sub_count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg) {
    int client_sock = *(int*)arg;
    free(arg); 
    MensajeDeportivo msg;
    int read_size;

    if ((read_size = recv(client_sock, &msg, sizeof(MensajeDeportivo), 0)) > 0) {
        
        if (strcmp(msg.data, "SUBSCRIBE") == 0) {
            
            pthread_mutex_lock(&mutex); 
            if (sub_count < MAX_SUBS) {
                subscribers[sub_count].socket = client_sock;
                strcpy(subscribers[sub_count].topic, msg.topic);
                sub_count++;
                printf("[BROKER] Nuevo suscriptor registrado para el partido: %s\n", msg.topic);
            }
            pthread_mutex_unlock(&mutex); 
            
            while((read_size = recv(client_sock, &msg, sizeof(MensajeDeportivo), 0)) > 0) {
            }
            printf("[BROKER] Un suscriptor se ha desconectado.\n");
            
        } else {
            printf("[BROKER] Noticia recibida de [%s]: %s\n", msg.topic, msg.data);
            
            pthread_mutex_lock(&mutex);
            for (int i = 0; i < sub_count; i++) {
                if (strcmp(subscribers[i].topic, msg.topic) == 0) {
                    send(subscribers[i].socket, &msg, sizeof(MensajeDeportivo), 0);
                }
            }
            pthread_mutex_unlock(&mutex);

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
    
    close(client_sock);
    return NULL;
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en el bind");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

   
    if (listen(server_sock, 10) < 0) {
        perror("Error en el listen");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("=== BROKER TCP INICIADO ===\n");
    printf("Esperando conexiones en el puerto %d...\n\n", PORT);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Error al aceptar la conexión");
            continue;
        }

        int *new_sock = malloc(sizeof(int));
        *new_sock = client_sock;

        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handle_client, (void*)new_sock) < 0) {
            perror("Error al crear el hilo");
            free(new_sock);
            close(client_sock);
            continue;
        }
        
        pthread_detach(client_thread); 
    }

    close(server_sock);
    return 0;
}