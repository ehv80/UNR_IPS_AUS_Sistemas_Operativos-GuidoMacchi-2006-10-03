/* Archivo: servidor-udp-no-bloqueante-v2.c
*      Abre un socket UDP (NO ORIENTADO A CONEXIÓN). 
*      verifica si existen paquetes de entrada por el socket ó por STDIN.
*      Si se reciben paquetes por el socket, los envía a STDOUT.
*      Si se reciben paquetes por STDIN, los envía por el socket.
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

	int sockio, cuanto, largo;

	struct sockaddr_in sinio;
	char linea[SIZE];
	fd_set in, in_orig;
	struct timeval tv;
	
	
	if((sockio=socket(PF_INET, SOCK_DGRAM, 0)) < 0 )
		error("socket");
	

	sinio.sin_family = AF_INET; // Familia de direcciones de sockio
	sinio.sin_port = htons(PORT); // Puerto, con bytes en orden de red, para sockio
	sinio.sin_addr.s_addr = INADDR_ANY; 
	//dirección de internet, con bytes en orden de red, para sockio
	
	if( bind(sockio, (sad)&sinio, sizeof sinio) < 0 )
		error("bind");

	largo = sizeof sinio;
	
	// lo que lea de STDIN mediante dirección sinio.sin_addr de sockio, se escribirá en sockio
	// lo que lea por sockio desde dirección remota sinio.sin_addr, se escribirá en STDOUT

	/*tiene select*/
	FD_ZERO(&in_orig); //Limpia el conjunto de descriptores de ficheros in_orig
	FD_SET(0, &in_orig); //añade STDIN al conjunto in_orig
	FD_SET(sockio, &in_orig); //añade sockio al conjunto in_orig
	
	/*tiene 1 hora*/
	tv.tv_sec=TIME;	//tiempo hasta que select(2) retorne: 3600 segundos
	tv.tv_usec=0;

	for(;;){
		memcpy(&in, &in_orig, sizeof in); // copia conjunto in_orig en in

		if( (cuanto = select( MAX(0,sockio)+1, &in, NULL, NULL, &tv) ) < 0 )
			error("select: error");
		

		if(cuanto == 0)
			error("select: timeout");

		/* averiguamos donde hay algo para leer*/

		if(FD_ISSET(0,&in)){	//si hay para leer desde STDIN
			
			fgets(linea, SIZE, stdin);//lee hasta 1024 caracteres de STDIN, los pone en linea
			
			if(sendto(sockio, linea, sizeof linea, 0, (sad)&sinio, largo) < 0 )//escribe contenido de linea en sockio
				error("sendto");
		}
		
		if(FD_ISSET(sockio, &in)){
			
			if((cuanto = recvfrom(sockio, linea, SIZE, 0, (sad)&sinio, &largo )) < 0)//lee hasta 1024 caracteres
				error("recvfrom"); 					     // de sockio, los pone en linea
			else if(cuanto == 0)	// si lectura devuelve 0 ==> parar ejecucion
				break;
			linea[cuanto]=0;	// marcar fin del buffer con ``0´´
			
			/* Imprime en pantalla dirección de internet del cliente desde donde vienen datos */
			printf("\nDe la direccion[ %s ] : puerto[ %d ] --- llega el mensaje:\n",
					inet_ntoa(sinio.sin_addr),
					ntohs(sinio.sin_port));
			printf("%s \n",linea);
		}
	}
	close(sockio);
	return 0;
}
/* Fin Archivo: servidor-udp-no-bloqueante-v2.c */
