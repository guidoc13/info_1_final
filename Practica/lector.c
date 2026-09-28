#include <stdio.h>
#include <stdint.h>
union serverRegistro{
    struct{
    	uint32_t bpm			: 9;
    	uint32_t spo2			: 7;
    	uint32_t socket_client	: 8;
    } dataSens;

    uint32_t trama_cruda;
};


int main(int argc, char *argv[])
{
    // Comprobación de la cantidad de argumentos de main 
    if(argc != 2){
        printf("[ERROR] Para ejecutar la lectura ingresar por terminal  %s <nombre_del_archivo.dat>\n",argv[0]);
        return 1;
    }

    FILE *file;
    union serverRegistro dataBin_read;
    int aux;

    file = fopen(argv[1], "rb");
    if(file == NULL){
        printf("fopen ERROR\n");
        return 1;
    }

    while(1){
        aux = fread(&dataBin_read.trama_cruda, sizeof(uint32_t), 1, file);
        if (aux != 1){
            if (feof(file)){
                printf("Lectura exitosa (EOF)\n");
            } 
            else if (ferror(file)){
                printf("fread ERROR\n");
            }
            break;
        }

        printf("ID: %d \t BPM: %d \t SpO2: %d \n",
        	   dataBin_read.dataSens.socket_client,
        	   dataBin_read.dataSens.bpm,
        	   dataBin_read.dataSens.spo2);

    }

    fclose(file);
    

    return 0;
}
