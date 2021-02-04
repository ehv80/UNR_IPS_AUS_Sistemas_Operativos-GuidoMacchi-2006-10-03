/* Archivo: hola-mundo-cliente-udp.c */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

/* DEFINICIONES */
#define PORT 5000
#define SIZE 1024

/* SINONIMOS */
typedef struct sockaddr * sad;

/* FUNCIONES */
void error (char *s)
{
	perror(s);
	exit (-1);
}

/* FUNCION PRINCIPAL */
int main (int argc , char **argv)
{
	if(argc < 2)
	{
		fprintf(stderr, "Uso: %s ipaddr\n", argv[0]);
		exit(-1);
	}
	int sock;
	struct sockaddr_in sini, sino;
	char linea[SIZE];
	int cuanto,L;
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
		error("socket");
	sino.sin_family = AF_INET;
	sino.sin_port = htons(PORT);
	inet_pton(AF_INET, argv[1], &sino.sin_addr);
	//Posix To Network. Leer: man 3 inet_pton
	if( (sendto(sock, "Hola mundo!\n", 12, 0, (sad)&sino, sizeof sino)) < 0)
		error ("send to");
	/* No hay necesidad de establecer una CONEXION para una SESION */
	L = sizeof (sini);
	if((cuanto = recvfrom(sock, linea, SIZE, 0, (sad)&sini, &L)) < 0)
		error("recv from");
	linea[cuanto] = 0;
	printf("De %s : %d --> Mensaje[ %s ]\n",
			inet_ntoa(sini.sin_addr),
			ntohs(sini.sin_port), 
			linea);
	close(sock);
	return 0;
}
/* Archivo: hola-mundo-cliente-udp.c */
