#ifndef MENSAJES_H
#define MENSAJES_H

#define MAX_TOPIC 50   // Tamaño máximo para el nombre del partido (ej. "Nacional vs Millonarios")
#define MAX_DATA 256   // Tamaño máximo para el mensaje (ej. "Gol al minuto 32")

// Estructura que viajará por los sockets
typedef struct {
    char topic[MAX_TOPIC];
    char data[MAX_DATA];
} MensajeDeportivo;

#endif