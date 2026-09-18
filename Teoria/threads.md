# THREADS
Un único proceso donde suceden **"varias cosas a la vez"** (el uP le asigna un tiempo de procesamiento a cada uno, pero lo hace tan rápido que parece que se ejecutan simultáneamente). Son funciones que empiezan y terminan, comparten memoria del mismo proceso. 

Todas las funciones de flujo son **BUFFEREADAS y no BLOQUEANTES**. Esto significa que usan buffers o colas de memoria. Guardan los datos en un espacio de memoria intermedio (buffer) así el hilo principal puede usar esos datos cuando esté libre. Hay que esperar a que el buffer se llene y se vacíe. Por ejemplo, poner `\n` para vaciar el buffer, y no ponerlo para escribir y no vaciarlo. Cuando el hilo (thread) principal finaliza, mata al resto.

# CREAR UN THREAD

```c

#include <pthread.h>

    [tipo de dato]  [nombre de la variable]
    pthread_t       thread_id;

    [función pthread_create]
    pthread_create( (1), (2), (3), (4) ) // Esta función arranca el thread y lo manda a correr en segundo plano

 En caso de éxito, la función retorna `0`. Y en caso de error, retorna un número de error y el número de id es indefinido.

``` 

(1): Dirección de memoria de la variable del tipo `pthread_t`. Originalmente es un puntero, pero si se trata como tal primero se debe asignarle memoria antes de usarlo.
Ejemplo:

    pthread_t thread_id;

    El argumento (1) será: &thread_id. Hay que pasar la dirección de memoria explícitamente porque NO es un puntero. Esto es lo más comúnmente utilizado.

o en su defecto

    pthread_t *thread_id;
    thread_id = (pthread_t *) malloc(sizeof(pthread_t));

    El argumento (1) será: thread_id, ya que ES un puntero. Recordar liberar la memoria al final.

(2): Poner un `NULL`. Esto simplemente indica traer las configuraciones que vienen por defecto. 

(3): Dirección de ejecución de la función a ejecutar (la dirección de memoria de la función, en otras palabras).

(4): Dirección de memoria de los datos que se van a trabajar dentro del thread. Por ejemplo, si se quieren mandar muchas cosas mandar un `struct *` y castearlo dentro del thread. Recordar
que este solo recibe y devuelve `void *`, si no se quiere enviar nada al thread, simplemente ingresar `NULL`.

Un thread tiene esta forma:

```c

    void * __________(void *( nombre de la variable )){

        // código

        return (algo); // puede no estar
    }
```

# MECANISMO DE ESPERA Y SINCRONIAZCIÓN DE THREADS 

```c

#include <pthread.h>

    [función pthread_join]
    pthread_join( (1), (2)) // Esta función es la que usa el thread principal (main) para esperar a que el thread especificado en (1) termine

 En caso de éxito, la función retorna `0`. Y en caso de error, retorna un número de error.

``` 
(1): Es una variable del tipo `pthread_t`. Con lo cual solo va el ID del thread que se quiere joinear
(2): Es una variable del tipo `void **` donde se guardaŕa el valor de retorno del thread que se joineó (se pone `NULL`);

## Ejemplo 1 en archivo ejemplo1.c

```c
    #include <stdio.h>
    #include <pthread.h>
    
    struct infoAlThread             // datos con los que va a operar el thread
    {
        char caracter;
        int cant;
    };

    void *char_print(void *param)   // esto es lo que HARÁ el thread.
    {
        int i;
        struct infoAlThread *INFO = (struct infoAlThread *)param; // se castea para poder usar los datos en el thread

        char caract = INFO->caracter;
        int canti = INFO->cant;

        for(i = 0; i < canti; i++){
            fputc(carac,stdout);
        }

        return(0);
    } 

    int main()
    {
        struct infoAlThread I1 = {"A",30};
        struct infoAlThread I2 = {"B",20};
        struct infoAlThread I3 = {"C",18};

        pthread_t thread_id1;
        pthread_t thread_id2;
        pthread_t thread_id3;

        phthread_create(&thread_id1, NULL,&char_print,&I1);
        phthread_create(&thread_id2, NULL,&char_print,&I2);
        phthread_create(&thread_id3, NULL,&char_print,&I3);

        // Hasta acá ya creé los tres threads. Ahora los voy a joinear

        phtread_join(thread_id1, NULL);
        phtread_join(thread_id2, NULL);
        phtread_join(thread_id3, NULL);

        printf("\n Fin del programa. \n);
        return (0);
    }

```

Nota: pthread_join es una función que hace que el sistema espera a que termine el thread que se le carga para continuar. Es una función BLOQUEANTE. Esta bloquea al thread donde se hace el join. Pero los demás threads siguen corriendo.


Cuando hay más de un thread operando una misma variable, entonces operan sobre una misma dirección de memoria.

Pasos que hace el uP en la ejecución de un proceso, por ejemplo, una suma:

1) RAM -> CPU
2) ++ en el CPU
3) CPU -> RAM

Dichos pasos NO pueden ser interrumpidos a la mitad, pero sí entre paso y paso. Esto puede llevar a problemas que la variable no se actualice. La solución es implementar un **MUTEX**

# MUTEX
Es una herramienta de sincronización que se usa para evitar que varios hilos modifiquen un recurso compartido al mismo tiempo.Es una barrera que hace que un procesamiento en particular no siga en las siguientes líneas hasta que algo suceda (se llegue al `unlock`).

OJO: MUTEX no es lo mismo que JOIN. MUTEX lo que hace es bloquear hasta que termine un procesamiento en particular, en cambio JOIN bloquea hasta que termina el thread entero.

Para hacer un mutex, hay que:
    1) Inicializar: `pthread_mutex_init`
    2) Lockear:     `pthread_mutex_lock`. Mutex guarda en una cola el orden en que se llegó a la línea de lock.
    3) Unlockear:   `pthread_mutex_unlock`
    4) Destruir:    `pthread_mutex_destroy` 

## Ejemplo 2 en archivo ejemplo2_mutex.c
```c

    #include <stdio.h>
    #include <stdlib.h>
    #include <pthread.h>

    pthread_mutex_t my_mutex; // variable global

    void *incremento(void *param) // 1er función de thread
    {
        int i = 0;
        int *p = (int *)param;
        for(i = 0; i < 5; i++){
            pthread_mutex_lock(&my_mutex);
            if((*p) < 10){
                (*p)++;
            }
            printf("%d\n",(*p));
            pthread_mutex_unlock(&my_mutex);
        }

        return (0);
    }

    void *decremento(void *param) //2da función de thread
    {
        int i;
        int *p = (int *)param;

        for(i = 0; i < 5; i++){
            pthread_mutex_lock(&my_mutex);
            if((*p) > 0){
                (*p)--;
            }
            printf("%d\n",(*p));
            pthread_mutex_unlock(&my_mutex);
        }

        return(0);

    }

    int main()
    {
        int valor = 0;
        pthread_t id1;
        pthread_t id2;

        if(pthread_mutex_init(&my_mutex,NULL)){
            printf("ERROR MUTEX\n");
        }else{
            pthread_create(&id1,NULL,&incremento,&valor);
            pthread_create(&id2,NULL,&decremento,&valor);

            pthread_join(id1,NULL);
            pthread_join(id2,NULL);
            
            pthread_mutex_destroy(&my_mutex);
        }
        return 0;
    }

```


