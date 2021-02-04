/* Archivo: servidor-udp.c
*      Abre un socket UDP (NO ORIENTADO A CONEXIÓN). 
*      Falta verificar si existen paquetes de entrada por el 
*      socket ó por STDIN (select(2)).
*      Deberá ver el orden de envíos/recepción de mensajes
*      hacia/desde el servidor udp, es decir el usuario
*      debe sincronizar los envíos/recepciones.
*/ 
/* ARCHIVOS DE CABECERA */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/time.h>
#include<sys/select.h>
/* DEFINICIONES */
#define PORT 5000
#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define SIZE 1024
#define TIME 3600
/* SINONIMOS*/
typedef struct sockaddr *sad;
/* FUNCIONES */
void error(char *s){
	perror(s);
	exit(-1);
}
/* FUNCION PRINCIPAL MAIN */
int main(){
	int sockio, largo, recibidos;
	struct sockaddr_in sinio;
	char linea[SIZE];
	if((sockio=socket(PF_INET, SOCK_DGRAM, 0)) < 0 )
		error("socket");
	sinio.sin_family = AF_INET; // Familia de direcciones de sockio
	sinio.sin_port = htons(PORT); // Puerto, con bytes en orden de red, para sockio
	sinio.sin_addr.s_addr = INADDR_ANY; 
	//dirección de internet, con bytes en orden de red, para sockio
	if( bind(sockio, (sad)&sinio, sizeof sinio) < 0 )
		error("bind");
	largo = sizeof sinio;
	for(;;){
		// lee hasta 1024 caracteres de sockio, los pone en linea
		if((recibidos = recvfrom(sockio, linea, SIZE, 0, (sad)&sinio, &largo )) < 0)
			error("recvfrom");
		linea[recibidos]=0;	// marcar fin del buffer con ``0´´
		/* Imprime en pantalla dirección de internet del cliente desde 
		 * donde vienen datos */
		printf("\nDe la direccion[ %s ] : puerto[ %d ] --- llega el mensaje:\n",
				inet_ntoa(sinio.sin_addr),ntohs(sinio.sin_port));
		printf("%s \n",linea);
		//lee hasta 1024 caracteres de STDIN, los pone en linea
		fgets(linea, SIZE, stdin);
		//escribe contenido de linea en sockio
		if(sendto(sockio, linea, sizeof linea, 0, (sad)&sinio, largo) < 0 )
			error("sendto");
	}
	close(sockio);
	return 0;
}
/* Fin Archivo: servidor-udp.c */
