/* Archivo: cliente-udp.c 
 *	Abre un socket UDP (NO ORIENTADO A CONEXIÓN).
 *	Falta verificar si existen paquetes DATAGRAMAS de
 *	entrada por el socket ó por STDIN (select(2)).
 *	Deberá ver el orden de envíos/recepción de mensajes
 *	hacia/desde el servidor udp, es decir el usuario
 *	debe sincronizar los envíos/recepciones.
 **/
/* ARCHIVOS DE CABECERA */
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/time.h>
#include<sys/select.h>
#include<string.h>
/* DEFINICIONES */
#define PORT 5000
#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define SIZE 1024
#define TIME 3600
/* SINONIMOS */
typedef struct sockaddr *sad;
/* FUNCIONES */
void error(char *s){
    perror(s);
    exit(-1);
}
/* FUNCION PRINCIPAL MAIN */
int main(int argc, char **argv){
	if(argc < 2){
		fprintf(stderr,"usa: %s ipaddr \n", argv[0]);
		exit(-1);
	}
	int sockio, largo, recibidos; 
	//por sockio va a enviar-recibir paquetes datagramas
	struct sockaddr_in sini, sino; // direcciones de internet para sockio
	// sini para recibir-leer
	// sino para enviar-escribir
	char linea[SIZE];	// ``buffer´´ de 1024 caracteres (un arreglo)
	if((sockio = socket(PF_INET, SOCK_DGRAM, 0)) < 0) // abre conector UDP
		error("socket");
	sino.sin_family = AF_INET; //familia de direcciones a la que pertenece sino, para sockio
	sino.sin_port = htons(PORT); // puerto de sino, para sockio
    	inet_pton(AF_INET, argv[1], &sino.sin_addr);
	/* POSIX TO NETWORK, ver man 3 inet_pton
	 * transforma argv[1] (la direccion IP pasada como argumento al programa)
	 * desde formato ``ascii´´ (puntero a cadena de caracteres)
	 * al formato ``network´´ (guarda en la ``struct in_addr´´ ``sino.sin_addr´´ )
	 * que a su vez es miembro de la ``struct sockaddr_in´´ ``sino´´.
	 * Se trata de la direccion IP del servidor al que quiere enviar datagramas.
	 * La dirección pertenece a la familia de direcciones ``AF_INET´´
	 */
	for(;;){
		//leer hasta 1024 caracteres de STDIN y ponerlos en linea
		fgets(linea, SIZE, stdin);
		//escribir contenido de linea en sockio
		if(sendto(sockio, linea, sizeof linea, 0, (sad)&sino, sizeof sino) < 0 )
				error("sendto");
		largo = sizeof sini;
		if((recibidos = recvfrom(sockio, linea, SIZE, 0, (sad)&sini, &largo)) < 0)
			error("recvfrom"); //si recepción devuelve < 0 ==> error al leer
		linea[recibidos] = 0;	//marcar el final del buffer con ``0´´
		/* Imprime en pantalla la direccion del servidor desde donde vienen datos */
		printf("\nDe la direccion[ %s ] : puerto [ %d ] --- llega el mensaje:\n",
				inet_ntoa(sini.sin_addr),
				ntohs(sini.sin_port));
		//Imprime en pantalla lo que vino desde sockio mediante buffer linea
		printf("%s \n",linea);
	}
	close(sockio);
	return 0;
}
/* Fin Archivo: cliente-udp.c */
