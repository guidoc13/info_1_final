#include "includes.h"

void *procesar_datos(void *arg);
void *vaciar_datos(void *arg);

int main(int argc, char *argv[])
{
    if(argc != 2){
        printf("[ERROR] Ingresar por terminal %s <puerto>\n",argv[0]);
        return 1;
     }

    int puerto;
    puerto = atoi(argv[1]);


    int socket_desc, new_socket, c;
    struct sockaddr_in server, client;

    socket_desc = socket(AF_INET, SOCK_STREAM,0);
    if(socket_desc == -1){
        printf("Socket ERROR\n");
        return 1;
    }

    int opt = 1;
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        printf("Error configurando SO_REUSEADDR\n");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(puerto);

    if(bind(socket_desc, (struct sockaddr *)&server,sizeof(server)) == -1){
        printf("Bind ERROR\n");
        return 1;
    }

    if(listen(socket_desc,5) == -1){
        printf("Listen ERROR\n");
    }

    printf("Servidor escuchando en el puerto %d...\n",puerto);

    pthread_mutex_init(&my_mutex, NULL);
    
    pthread_t thread_id2;
    pthread_create(&thread_id2,NULL,vaciar_datos,NULL);
    pthread_detach(thread_id2);

    // la clave para implementar el servidor concurrente.
    c = sizeof(struct sockaddr_in);
    while(1){
        new_socket = accept(socket_desc,(struct sockaddr *)&client,(socklen_t *)&c);
        if(new_socket == -1){
            printf("Accept ERROR\n");
            return 1;
        }
        puts("Cliente conectado");
        printf("IP Cliente: %s\t, Puerto cliente: %d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));

        int *socket_thread = malloc(sizeof(int));
        *socket_thread = new_socket;

        pthread_t thread_id1;
        pthread_create(&thread_id1,NULL,procesar_datos,(void *)socket_thread);
        pthread_detach(thread_id1);
    }
    
    close(socket_desc);


    return 0;
}
