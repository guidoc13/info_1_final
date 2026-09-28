// en cliente.h tengo esto y sigue tirando todos esos subrayados rojos insoportables

#ifndef CLIENTE_H
#define CLIENTE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // para usleep
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define MAX_BUFFER 20

// Union  global
union Telemetria{
    struct{
        uint16_t bpm  : 9;
        uint16_t spo2 : 7;
    } dataSens;
    
    uint16_t trama_cruda;
};

#endif // CLIENTE_H