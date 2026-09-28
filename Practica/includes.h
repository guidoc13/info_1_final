#ifndef FUNCIONES_H
#define FUNCIONES_H

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


union serverRegistro{
    struct{
    	uint32_t bpm           : 9;
        uint32_t spo2          : 7;
        uint32_t socket_client : 8; //  para identificar al cliente. Ejemplo cliente 0, cliente 2, ... cliente 255 ya que es de 8 bits 
    } dataSens;
    
    uint32_t trama_cruda;
};


typedef struct Nodo {
    union serverRegistro paquete;
    struct Nodo *sig;
} Nodo;


void encolar(Nodo **ptr_inicio, Nodo **ptr_final, union serverRegistro nuevo_paquete);
union serverRegistro desencolar(Nodo **ptr_inicio, Nodo **ptr_final);

pthread_mutex_t my_mutex;
Nodo *ptr_inicio = NULL;
Nodo *ptr_final  = NULL; 

void *procesar_datos(void *arg)
{
    int *pointer_socket = (int *)arg;
    int my_socket; 
    union Telemetria datoRx;

    my_socket = *pointer_socket;
    free(pointer_socket);
    
    int byterx;
    printf("[Cliente %d] Conectado y transmitiendo datos...\n",my_socket);
    while(1){
        byterx = recv(my_socket, &datoRx, sizeof(uint16_t),0);
        if(byterx < 0){
            printf("Recv ERROR en [Cliente %d].\n",my_socket);
            return NULL;
        }else if(byterx == 0){
            printf("[Cliente %d] finalizó la transmisión y se desconectó.\n",my_socket);
            return NULL;
        }else{
            union serverRegistro nuevo_paquete;
            nuevo_paquete.dataSens.bpm = datoRx.dataSens.bpm;
            nuevo_paquete.dataSens.spo2 = datoRx.dataSens.spo2;
            nuevo_paquete.dataSens.socket_client = my_socket;

            pthread_mutex_lock(&my_mutex);
            encolar(&ptr_inicio,&ptr_final,nuevo_paquete); // datoRx deberia ser union serverRegistro 
            pthread_mutex_unlock(&my_mutex);

        }
    }
    close(my_socket);
    return NULL;
}

void *vaciar_datos(void *arg)
{   
    FILE *afile;
    int aux;
    char nombre_archivo[20];
    int client_id;

    while(1){
        usleep(10000);        
        pthread_mutex_lock(&my_mutex);
        if(ptr_inicio != NULL){
            union serverRegistro paquete_listo;
            paquete_listo = desencolar(&ptr_inicio,&ptr_final);
            pthread_mutex_unlock(&my_mutex);

            client_id = paquete_listo.dataSens.socket_client;
            sprintf(nombre_archivo, "sensor_%d.dat",client_id);
            afile = fopen(nombre_archivo,"ab");
            if(afile == NULL){
                printf("fopen ERROR.\n");
                pthread_mutex_unlock(&my_mutex);
                return NULL;
            }

            aux = fwrite(&paquete_listo,sizeof(union serverRegistro),1,afile);
            if(aux != 1){
                printf("fprintf ERROR.\n");
                fclose(afile);
                pthread_mutex_unlock(&my_mutex); 
                return NULL;
            }
            fclose(afile);
        }else{
            pthread_mutex_unlock(&my_mutex);
        }
    }
    return NULL;
}

void encolar(Nodo **ptr_inicio, Nodo **ptr_final, union serverRegistro nuevo_paquete)
{
    Nodo *ptr_nuevo;
    ptr_nuevo = (Nodo *)malloc(sizeof(Nodo));
    if(ptr_nuevo == NULL){
        printf("malloc ERROR\n");
        return;
    }

    ptr_nuevo->paquete = nuevo_paquete;
    ptr_nuevo->sig = NULL;
    
    if(*ptr_inicio == NULL){
        *ptr_inicio = ptr_nuevo;
        *ptr_final  = ptr_nuevo; 
    }else{
        (*ptr_final)->sig = ptr_nuevo;
        *ptr_final = ptr_nuevo;
    }
}

union serverRegistro desencolar(Nodo **ptr_inicio, Nodo **ptr_final)
{
    Nodo *ptr_temp;
    
    ptr_temp = *ptr_inicio;
    union serverRegistro paquete_eliminado;
    paquete_eliminado.trama_cruda = 0; // armamos un paquete vacio por las dudas de que no haya nada

    if(*ptr_inicio == NULL){
        return paquete_eliminado;
    }

    paquete_eliminado = ptr_temp->paquete;
    *ptr_inicio = ptr_temp->sig;
    if(*ptr_inicio == NULL){
        *ptr_final = NULL;
    }

    free(ptr_temp);
    return paquete_eliminado;
}



#endif // FUNCIONES_H
