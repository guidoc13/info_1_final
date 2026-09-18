# SOCKETS
Redes -> permiten la comunicación entre aplicaciones.
Redes Socket -> herramienta que conecta los procesos -> TCP / IP

TCP / IP -> son un conjunto de protocolos de red que permiten a los sistemas intercambiar recursos e información, independientemente de la ubicación física, el sistema operativo, el medio de red del sistema principal o del usuario.

[CLIENTE] <--------------------SOCKET TCP--------------------> [SERVIDOR]
App31                                                App777
IP_PC71                                              IP_PC22
Puerto_tcp 1234                                      Puerto_tcp 7892
Inicia la solicitud                                  Escucha y espera conexiones

Naturalmente TCP/IP no son seguros puesto que la comunicación puede ser modificada y/o espiada. Por lo tanto, la opción más segura es SSL (no entra en la materia).

Notas:
    - Protocolo Ipv4:
        - Una IP identifica una conexión de una red IP.
        - Las direcciones IP tienen 32 bits.
        - Hay tres clases de direcciones unicast (comunicación uno a uno) A, B y C
            -- Clase A: redes muy grandes. El rango de ip es desde 1.0.0.0 a 126.255.255.255 
                --- RED: 8 bits
                --- HOST: 24 bits 
            -- Clase B: redes medianas a grandes. El rango de ip es desde 128.0.0.0 a 191.255.255.255
                --- RED: 16 bits
                --- HOST: 16 bits 
            -- Clase C: redes pequeñas. El rango de ip es desde 192.0.0.0 a 223.255.255.255
                --- RED: 24 bits
                --- HOST: 8 bits 
        - Las direcciones IP de Broadcast sirven para enviar mensajes a toda la red.
        - **Todos** los paquetes tienen IP de origen y destino.
        - Un router analiza las IP de destino y elige hacia donde enviar el paquete.

    - TCP significa Transmission Control Protocol. Los Protocolo se pueden definir como el algoritmo que rige las comunicaciones.
        - Construye un "caño virtual" entre dos etremos llamados endpoint's.
        - Un puerto asocia una conexión de red con un proceso.
        - TCP controla recepción, secuencia e integridad de los datos, pero no es seguro.
        - Ninguna comunicación TCP tiene autenticación de extremos, ni confidencialidad.
        - El número de puerto tiene 16 bits.
    
    - Analogía: TCP es el camino del tren y la IP es la estación.

TX -> ENVIA PAQUETE
RX <- RECIBE PAQUETE

Notas:
    - Switch:
        - Utilizando la MAC de destino envia los paquetes hacia el destino correspondiente.
        - Si la MAC de destino es de broascast, saca el paquete por todos las interfaces.
        - Aprende las MAC de los dispositivos conectados. Cuando un paquete ingresa, revisa la MAC de origen y lo mapea a una tabla que asocia número de puerto y MAC.

    
# SERVIDOR
## FUNCIÓN SOCKET (UNIX, no C)
```c

    socket(AF_INET, SOCK_STREAM, 0);

    Si retorna un número negativo, hubo un error. Si devuelve un número positivo, este será el descriptor del socket.

```

`AF_INET` indica que utilizará IPv4. Proporciona comunicación entre procesos que se ejecutan en el mismo o en distintos sistemas. Las direcciones para los sockets, `AF_INET` son direcciones IP y números de puertos -> usa la estructura de direc: sockaddr_in.
Para su uso se necesita `#include <arpa/inet.h>`.

`SOCK_STREAM` especifica que la comunicación será orientada a conexión mediante el protocolo TCP. Envía datos sin error ni duplicación, y recibe datos en el orden de envío. COnsidera que los datos son una secuencia de bytes.

El tercer argumento especifica el tipo de protocolo a usar
    - TCP: Para sockets del tipo stream
    - UDP: Para sockets tipo datagram
    - 0: El sistema elige el protocola para el tipo solicitado en `SOCK_STREAM` (la mejor opción)

Parámetros para establecer una conexión (definir un socket)
1) Familia
2) Puerto servidor
3) IP servidor

## FUNCIÓN BIND

``` c
    bind( (1),(2),(3) )
    Retorna -1 en caso de error y 0 si está todo bien. Siempre CHEQUEAR.
```
(1): Es el número devuelto por socket (su descriptor)
(2): Un puntero a struct `sockaddr_in` (en caso de usar `AF_INET`). En la estructura se guardarán el puerto y la dirección IP del servidor.
(3): Acá va el tamaño en bytes de (2) (no del puntero, sino del struct con el puerto y la dirección IP cargados). Se suele utilizar sizeof(server) por ejemplo siendo server un dato del tipo `struct sockaddr_in`

Notas:
    - socket: crea el socket y devuelve su descriptor (su numero de identificación).
    - bind: asigna puerto y dirección IP del servidor al descriptor devuelto por la función socket.

### Cargar el puerto y las direcciones IP en el bind
En (2) se hace un puntero a `struct sockaddr_in`. A continuación, se muestra como cargar **puerto** y **dirección IP** en el struct y luego ponerlo en el bind

```c

    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <sys/types.h>

    int main()
    {
        .
        .
        .
        struct sockaddr_in server;
        int s;
        .
        .
        .
        s = socket(AF_INET, SOCK_STREAM, 0);
        // Hay que chequear errores. Luego:
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(15001); // Se usa para convertir el número de puerto al formato de bytes que entiende la red (big endian).
        bind(s, (struct sockaddr *)&server, sizeof(server));
        // Hay que chequear errores de bind también.
        .
        .
        .
        return(0);
    }

```

Nota sobre `struct sockaddr_in`
```c
struct sockaddr_in
{
    short  sin_family;
    u_short sin_port;
    struct in_addr sin_addr;
    u_long sin_zero[8];
};

donde
    - sin_family es la familia de nuestro socket. `AF_INET` pues así lo pusimos en el socket.
    - sin_port es el puerto del servidor. Puede ser cualquier número mayor a 1024.
    - struct in_addr
    {
        unsigned long sin_addr; // es la direc ip dl servidor. Ponemos INADDR_ANY pues así ponemos la direc sin saber cual es 
    };
```

## FUNCIÓN LISTEN
Tiene dos objetivos
    - Pone al socket a espera de conexiones.
    - Fija el número o la cantidad de otros clientes que pueden estar a la espera de conectarse.

```c

    listen( (1), (2));

    Retorna -1 en caso de error y 0 en caso de éxito.

```
(1): El descriptor devuelto por socket.
(2): Es el máximo número de pedidos de conexión que pueden esperar en la cola. (Donde cada cliente en la cola tiene que esperar a que el servidor haga un nuevo accept para que se conecten).

## FUNCIÓN ACCEPT
Cuando el cliente desea conectarse es cuando se ejecuta el `accept`

```c

    num = accept( (1), (2), (3));

    Retorna -1 en caso de error y 0 en caso de éxito.
```
(1): El descriptor devuelto por el socket.
(2): Puntero a struct `sockaddr_in` que contiene el puerto y la dirección ip del cliente.
(3): Puntero a `socklen_t` que devuelve el tamaño en bytes de (2).

Notas: 
    - Accept aceptará la conexión de cualquier proceso que tenga la dirección de nuestro socket.
    - num es el descriptor de archivo que se usará en write y read para transmitir información. Por ejemplo `write(num, ...)`, `read(num, ...)`

## TIPOS DE SOCKET
1) Socket de conexión: es el que se crea con la función `socket` (recibe las peticiones de conexión).
2) Socket de transmisión: es el que se crea al ejecutarse el `accept`. Para poder transmitirse info entre servidor y cliente.

## FUNCIÓN CLOSE
```c
    close( (1) );

    Retorna -1 en caso de error y 0 en caso de éxito.
```
(1): Descriptor del socket, puede ser tanto por el devuelto por el `accept` (socket de transmisión) o por el devuelto por el `socket` (socket de conexión)

# CLIENTE
Se crea un socket al igual que el servidor. También hay que cargarle la dirección IP y el puerto del servidor, para así poder conectarse a este. Por ejemplo

```c

    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <sys/types.h>

    int main()
    {
        .
        .
        .
        struct sockaddr_in server;
        int s;
        .
        .
        .
        s = socket(AF_INET, SOCK_STREAM, 0); 
        // Hay que chequear errores. Luego:
        server.sin_family = AF_INET;                        // (1)
        server.sin_addr.s_addr = inet_addr('127.0.0.1');    // (2)
        server.sin_port = htons(15001);                     // (3)
        .
        .
        return(0);
    }

```
(1): Familia del servidor al que nos queremos conectar.
(2): Dirección IP del servidor que al ser una cadena de char, se lo traduce con `inet_addr`.
(3): Puerto del servidor.

## FUNCIÓN CONNECT 
Crea una conexión con el destino especificado.
```c
    connect( (1), (2), (3) );

    Retorna -1 en caso de error y 0 en caso de éxito.
```
(1): Descriptor devuelto por la función socket del cliente.
(2): Puntero a struct `sockaddr_in` ¿o a `sockaddr *`?que contiene la dirección IP y el puerto del servior.
(3): Tamaño en bytes de la estructura que contiene la dirección IP y el puerto del servidor. Ej `sizeof(server)`

Luego se hace un `close` del socket creado por el socket del cliente.

# Machete

SERVER                          CLIENTE
s = socket()                    a = socket()
bind()
listen
num = accept()<-----------------connect()
write()------------------------>read()
read ()<------------------------write()
close(num)
close(s)                        close(a)


# PLANTILLA

```c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

// SERVER.c

int main()
{    
    int socket_desc,new_socket,c;
    struct sockaddr_in server, client;

    // Crear el socket 
    socket_desc = socket(AF_INET,SOCK_STREAM,0);
    if(socket_desc == -1){
        printf("No se pudo crear el socket.\n");
        return 1;
    }

    // Cargo el puerto y la dirección IP para el bind()
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0){
        printf("Bind ERROR\n");
        return 1;
    }

    // Escucha si hay clientes a la espera de conectarse
    if(listen(socket_desc,5) == -1){
        printf("Listen ERROR\n");
        return 1;
    }

    puts("Servidor escuchando en el puerto 8888...");


    // Saca a los clientes de la cola de espera y los conecta 
    c = sizeof(struct sockaddr_in);
    new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
    if(new_socket < 0){
        printf("Accept ERROR\n");
        return 1;
    }

    puts("Cliente conectado.");
    printf("IP Cliente: %s\t Puerto Cliente: %d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port)); // inet_ntoa permite imprimirla con %s y ntohs no genera conflicto con big endian

    close(new_socket);
    close(socket_desc);

    return 0;
}
```

```c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

// CLIENT.c
int main()
{
    int socket_desc;
    struct sockaddr_in server;

    socket_desc = socket(AF_INET, SOCK_STREAM,0);
    if(socket_desc == -1){
        printf("No se pudo crear el socket.\n");
        return 1;
    }

    // Conecto puerto e ip
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    server.sin_port = htons(8888);


    if(connect(socket_desc, (struct sockaddr *)&server,sizeof(server)) == -1){
        printf("Connect ERROR\n");
    }

    puts("Conectado al servidor");
    printf("IP Servidor: %s\t Puerto Servidor: %d\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port));
    
    close(socket_desc);

    return 0;
}
```

# Prueba de LaTeX
$$Id = k \cdot [(V_{\text{gs}} - V_{\text{t}}) \cdot V_{\text{ds}} - \frac{V_{\text{ds}}^{2}}{2}]$$$