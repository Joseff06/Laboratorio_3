# Laboratorio 3 - Sistema Pub/Sub Deportivo (TCP y UDP)

## Descripción

Sistema de publicación/suscripción en tiempo real para eventos deportivos implementado en C. Utiliza un broker central que enruta noticias en vivo desde publicadores a suscriptores usando tanto TCP como UDP.

### Arquitectura

- **Broker**: Recibe conexiones de publicadores y suscriptores, enruta mensajes por topic
- **Publicadores**: Envían noticias de eventos deportivos (10 mensajes por ejecución)
- **Suscriptores**: Se suscriben a un tema específico y reciben noticias en tiempo real

---

## Librerías Utilizadas

### 1. **stdio.h** - Entrada/Salida Estándar (POSIX)

**Propósito**: Facilita la entrada y salida de datos a través de la consola.

**Funciones utilizadas**:
- `printf()` - Imprime mensajes formateados en consola
- `perror()` - Imprime mensajes de error del sistema con descripción

**Ubicación en el código**:
- Todos los archivos TCP (`broker_tcp.c`, `publisher_tcp.c`, `subscriber_tcp.c`)
- Todos los archivos UDP (`broker_udp.c`, `publisher_udp.c`, `subscriber_udp.c`)

**Ejemplo de uso**:
```c
printf("[BROKER] Nuevo suscriptor registrado para el partido: %s\n", msg.topic);
perror("Error al crear el socket");
```

---

### 2. **stdlib.h** - Funciones de Utilidad Estándar de C (POSIX)

**Propósito**: Proporciona funciones para asignación de memoria, conversión de datos y terminación de programas.

**Funciones utilizadas**:
- `malloc()` - Asigna memoria dinámica en el heap
- `free()` - Libera memoria previamente asignada
- `exit()` - Termina el programa con un código de estado

**Ubicación en el código**:
- `broker_tcp.c`: `malloc()` para crear nuevo socket de cliente, `free()` para liberarlo en caso de error, `exit()` para terminar en caso de error
- Todos los archivos con manejo de errores utilizan `exit(EXIT_FAILURE)`

**Ejemplo de uso**:
```c
int *new_sock = malloc(sizeof(int));
*new_sock = client_sock;
free(new_sock);
exit(EXIT_FAILURE);
```

---

### 3. **string.h** - Manipulación de Cadenas de Caracteres (POSIX)

**Propósito**: Provee funciones para copiar, comparar, y manipular cadenas de caracteres y bloques de memoria.

**Funciones utilizadas**:
- `strcpy()` - Copia una cadena de caracteres a un destino
- `strcmp()` - Compara dos cadenas (retorna 0 si son iguales)
- `memset()` - Llena un bloque de memoria con un valor específico

**Ubicación en el código**:
- Todos los archivos para asignar valores a los campos `topic` y `data` de la estructura `MensajeDeportivo`
- Inicialización de estructuras de dirección de red

**Ejemplo de uso**:
```c
strcpy(msg.topic, "Nacional_vs_Millonarios");
if (strcmp(msg.data, "SUBSCRIBE") == 0) { /* Mensaje de suscripción */ }
memset(&server_addr, 0, sizeof(server_addr));
```

---

### 4. **unistd.h** - API Estándar POSIX

**Propósito**: Proporciona acceso a la API estándar POSIX para operaciones del sistema.

**Funciones utilizadas**:
- `sleep()` - Pausa la ejecución durante un número especificado de segundos
- `close()` - Cierra un descriptor de archivo o socket

**Ubicación en el código**:
- `sleep(2)` en publicadores para espaciar el envío de mensajes (cada 2 segundos)
- `close()` en todos los archivos para cerrar sockets antes de terminar

**Ejemplo de uso**:
```c
sleep(2);
close(sock);
close(sockfd);
```

---

### 5. **arpa/inet.h** - Funciones de Conversión de Direcciones de Red

**Propósito**: Proporciona funciones para convertir direcciones de red entre diferentes formatos (texto a binario y viceversa).

**Funciones utilizadas**:
- `inet_pton()` - Convierte una dirección IP en formato texto (ej. "127.0.0.1") a formato binario de red
- `htons()` - Convierte un puerto de host byte order a network byte order
- `ntohs()` - Convierte un puerto de network byte order a host byte order (usado en `htons` internamente)

**Ubicación en el código**:
- Todos los archivos cliente (publicadores y suscriptores TCP/UDP)
- Brokers para configurar puertos en las estructuras de dirección

**Ejemplo de uso**:
```c
inet_pton(AF_INET, BROKER_IP, &serv_addr.sin_addr);
serv_addr.sin_port = htons(PORT);
server_addr.sin_port = htons(PORT);
```

---

### 6. **sys/socket.h** - API de Sockets

**Propósito**: Define las constantes, estructuras y funciones necesarias para usar sockets (comunicación de red).

**Funciones utilizadas**:

#### Creación y configuración:
- `socket()` - Crea un nuevo socket (TCP con `SOCK_STREAM`, UDP con `SOCK_DGRAM`)
- `bind()` - Vincula un socket a una dirección y puerto
- `listen()` - Pone el socket en modo escucha (solo TCP)
- `accept()` - Acepta una conexión entrante (solo TCP)
- `connect()` - Establece conexión con servidor (solo TCP)

#### Envío y recepción:
- `send()` - Envía datos por un socket conectado (TCP)
- `recv()` - Recibe datos de un socket conectado (TCP)
- `sendto()` - Envía datos a una dirección específica (UDP)
- `recvfrom()` - Recibe datos y obtiene la dirección del remitente (UDP)

#### Limpieza:
- `close()` - Cierra un socket

**Estructuras utilizadas**:
- `struct sockaddr_in` - Estructura para direcciones IPv4
- `socklen_t` - Tipo para longitudes de sockets

**Ubicación en el código**:
- **TCP**: `broker_tcp.c`, `publisher_tcp.c`, `subscriber_tcp.c`
- **UDP**: `broker_udp.c`, `publisher_udp.c`, `subscriber_udp.c`

**Ejemplo de uso**:
```c
// TCP
int sock = socket(AF_INET, SOCK_STREAM, 0);
bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
listen(server_sock, 10);
client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
send(subscribers[i].socket, &msg, sizeof(MensajeDeportivo), 0);

// UDP
int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
sendto(sockfd, &msg, sizeof(msg), 0, (const struct sockaddr *)&serv_addr, sizeof(serv_addr));
recvfrom(sockfd, &msg, sizeof(MensajeDeportivo), 0, (struct sockaddr *)&client_addr, &client_len);
```

---

### 7. **pthread.h** - Librería de Threads POSIX

**Propósito**: Proporciona funciones para crear y gestionar múltiples hilos de ejecución (threads) dentro de un proceso.

**Funciones utilizadas**:
- `pthread_create()` - Crea un nuevo thread que ejecuta una función
- `pthread_detach()` - Detacha un thread (libera recursos automáticamente al terminar)
- `pthread_mutex_init()` - Inicializa un mutex (candado para sincronización)
- `pthread_mutex_lock()` - Adquiere el mutex (bloquea si está en uso)
- `pthread_mutex_unlock()` - Libera el mutex

**Propósito de uso**:
Permite que el broker TCP maneje múltiples clientes simultáneamente. Cada conexión se ejecuta en un thread separado, evitando bloqueos y mejorando concurrencia.

**Ubicación en el código**:
- Solo `broker_tcp.c` (UDP no necesita threads, es sin conexión)

**Ejemplo de uso**:
```c
pthread_t client_thread;
pthread_create(&client_thread, NULL, handle_client, (void*)new_sock);
pthread_detach(client_thread);

pthread_mutex_lock(&mutex);
// Sección crítica: acceso a recursos compartidos
pthread_mutex_unlock(&mutex);
```

---

### 8. **mensajes.h** - Header Local Personalizado

**Propósito**: Define la estructura de datos compartida entre publicadores, brokers y suscriptores.

**Contenido**:
```c
#define MAX_TOPIC 50   // Tamaño máximo del nombre del evento/partido
#define MAX_DATA 256   // Tamaño máximo del contenido del mensaje

typedef struct {
    char topic[MAX_TOPIC];
    char data[MAX_DATA];
} MensajeDeportivo;
```

**Campos**:
- `topic`: Nombre del evento deportivo (ej. "Nacional_vs_Millonarios")
- `data`: Contenido del mensaje (ej. "Gol al minuto 45")

**Ubicación en el código**:
Se incluye en todos los archivos .c con: `#include "../mensajes.h"`

---

## Resumen de Dependencias

| Librería | Tipo | Propósito | Archivos |
|----------|------|----------|---------|
| stdio.h | POSIX | Entrada/salida | Todos |
| stdlib.h | POSIX | Asignación de memoria y utilidades | Todos |
| string.h | POSIX | Manipulación de caracteres | Todos |
| unistd.h | POSIX | API estándar (sleep, close) | Todos |
| arpa/inet.h | POSIX Network | Conversión de direcciones IP | Todos |
| sys/socket.h | POSIX Network | Comunicación por sockets | Todos |
| pthread.h | POSIX Threads | Manejo de threads | broker_tcp.c |
| mensajes.h | Local | Estructura de mensajes | Todos |

---

## Compilación

### Requisitos
- Compilador GCC
- Soporte POSIX (Linux, macOS, WSL en Windows)
- Librería pthread (incluida en POSIX)

### Comando de compilación
```bash
# TCP
gcc -o broker_tcp TCP/broker_tcp.c -lpthread
gcc -o publisher_tcp TCP/publisher_tcp.c
gcc -o subscriber_tcp TCP/subscriber_tcp.c

# UDP
gcc -o broker_udp UDP/broker_udp.c
gcc -o publisher_udp UDP/publisher_udp.c
gcc -o subscriber_udp UDP/subscriber_udp.c
```

**Nota**: El flag `-lpthread` es necesario solo para compilar `broker_tcp.c` debido a su uso de la librería pthread.

---

## Ejecución

### Orden recomendado (terminal separadas)

1. **Inicia el broker**:
   ```bash
   ./broker_tcp    # O ./broker_udp
   ```

2. **Inicia suscriptores** (en otras terminales):
   ```bash
   ./subscriber_tcp    # O ./subscriber_udp
   ```

3. **Inicia publicadores** (en otras terminales):
   ```bash
   ./publisher_tcp    # O ./publisher_udp
   ```

---

## Características Clave

- ✅ **Pub/Sub Pattern**: Desacoplamiento entre publicadores y suscriptores
- ✅ **Multi-topic**: Soporte para múltiples temas/eventos simultáneamente
- ✅ **Concurrencia TCP**: Manejo de múltiples clientes con threads POSIX
- ✅ **Sockets estándar**: Implementación con APIs POSIX portables
- ✅ **Sin dependencias externas**: Solo librerías estándar del sistema

---

## Autor
Laboratorio 3 - Programación de Redes (2026)
