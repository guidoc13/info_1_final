#include "cliente.h"

int main()
{
    int socket_desc;
    struct sockaddr_in server;

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc == -1){
        printf("Socket ERROR\n");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(8888);

    if(connect(socket_desc, (struct sockaddr *)&server,sizeof(server)) == -1){
        printf("Connect ERROR\n");
        return 1;
    }

    puts("Conectado al servidor\n");
    printf("IP server: %s \t, Puerto server: %d\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port));

    // --------------------------------------------------------------------------
    // El cliente será mi sensor recolectando datos de BPM y SpO2
    // Inicializar la semilla 
    union Telemetria datoTx;
    int i;
    
    srand(time(NULL));
    
    for(i = 0; i < MAX_BUFFER; i++){
        
        usleep(150000);                      // simular muestreo cada 100 ms
        datoTx.dataSens.bpm   = (rand() % 41) + 60;   // simular datos muestreados
        datoTx.dataSens.spo2  = (rand() % 6)  + 95;   // simular datos muestreados

        write(socket_desc,&(datoTx.trama_cruda),sizeof(uint16_t));
        printf("El sensor envia: BPM: %d \t spO2: %d\n",datoTx.dataSens.bpm,datoTx.dataSens.spo2);    
    }

    close(socket_desc);
    return 0;
}
