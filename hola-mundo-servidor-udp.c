/* Archivo: hola-mundo-servidor-udp.c */
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

void error (char *s)
{
	perror(s);
	exit (-1);
}

/* FUNCION PRINCIPAL */
int main ()
{
	int sock;
	struct sockaddr_in sin;
	char linea[SIZE];
	int cuanto, L;
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
		error("socket");
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	sin.sin_addr.s_addr = INADDR_ANY; //Cualquier interface.
	if( bind(sock, (sad)&sin, sizeof sin) < 0)
		error("bind");
	L = sizeof (sin);
	if((cuanto = recvfrom(sock, linea, SIZE, 0, (sad)&sin, &L)) < 0)
		error("recv from");
	//linea[0]++;
	if(sendto(sock, linea, cuanto, 0, (sad)&sin, L) < 0)
		error ("send to");
	close(sock);
	return 0; //FINALIZACION EXITOSA
}
/* Archivo: hola-mundo-servidor-udp.c */
