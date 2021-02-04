/* Archivo: servidor-tcp-no-bloqueante-con-puerto-reutilizable.c
*      Abre un socket TCP y espera la conexión. 
*      Con setsockopt(2) aplica al conector ``sock´´ la opción 
*      ``SO_REUSEADDR´´ del nivel ``SOL_SOCKET´´ con el valor 
*      de la variable ``yes´´ para permitir que más de una instancia
*      del proceso cliente pueda conectarse al mismo puerto en forma
*      simultánea. 
*      La comunicación bidireccional solo se logra entre un par 
*      cliente servidor, el resto de los clientes pueden conectarse
*      pero no podrán enviar ni recibir mensajes.
*      Una vez establecida la conexión, verifica si existen paquetes 
*      de entrada por el socket ó por STDIN.
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

/* SINONIMOS */				   
typedef struct sockaddr *sad;

/* FUNCIONES */
void error(char *s){
	perror(s);
	exit(-1);
}

/* FUNCION PRINCIPAL MAIN */
int main(){
	const int yes = 1; //para setsockopt: SO_REUSEADDR
	int sock, sock1, cto, largo;
	struct sockaddr_in sin, sin1;
	char linea[SIZE];
	fd_set in, in_orig;
	struct timeval tv;
	
	if((sock=socket(PF_INET, SOCK_STREAM,0))<0) //sock es el que escucha
		error("socket");		    //sock1 es el que acepta y recibe
	
	sin.sin_family=AF_INET; // Familia de direcciones de sock
	sin.sin_port=htons(PORT);// Puerto, con bytes en orden de red, para sock
	sin.sin_addr.s_addr=INADDR_ANY;	//dirección de internet, con bytes en
					// orden de red, para sock

	/* Aplica al conector ``sock´´ la opción ``SO_REUSEADDR´´ 
	 * del nivel ``SOL_SOCKET´´ con el valor de ``yes´´
	 **/ 
	if( setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) < 0)
		error("setsockopt: sock");
	
	if(bind(sock, (sad)&sin, sizeof sin) < 0) //publica dirección sin de sock
		error("bind");
	if(listen(sock, 5) < 0 ) //pone en escucha a sock, 5 es tamaño de cola espera
		error("listen");

	largo = sizeof sin1;
	
	/*por ``sock1´´ acepta conexiones TCP desde dirección remota ``sin1´´, 
	 * mediante dirección local ``sin´´ de ``sock´´
	 **/
	if((sock1 = accept(sock, (sad)&sin1, &largo)) < 0) 	
		error("accept");
	
	// lo que se lea de STDIN mediante dirección ``sin´´ de ``sock´´, se escribirá en ``sock1´´
	// lo que se lea por sock1 desde dirección remota sin1, se escribirá en STDOUT
	
	/*tiene select*/
	FD_ZERO(&in_orig); //Limpia el conjunto de descriptores de ficheros in_orig
	FD_SET(0, &in_orig); //añade STDIN al conjunto in_orig
	FD_SET(sock1, &in_orig); //añade sock1 al conjunto in_orig
	
	/*tiene 1 hora*/
	tv.tv_sec = TIME; //tiempo hasta que select(2) retorne: 3600 segundos
	tv.tv_usec=0;
	for(;;){
		memcpy(&in, &in_orig, sizeof in); // copia conjunto in_orig en in
		if((cto=select(MAX(0,sock1)+1,&in,NULL,NULL,&tv))<0) // observa
			error("select");	// si hay algo para leer en el conjunto in
		if(cto==0)	//si tiempo de espera de select(2) termina ==> error
			error("timeout");

		/* averiguamos donde hay algo para leer*/
		if(FD_ISSET(0,&in)){	//si hay para leer desde STDIN
			
			fgets(linea, SIZE, stdin); // lee hasta 1024 caracteres de STDIN, los pone en linea
			
			if( write(sock1, linea, strlen(linea)) < 0 ) // escribe contenido de linea en sock1
				error("write");
		}
		
		if(FD_ISSET(sock1,&in)){	// si hay para leer desde sock1 ``remoto´´
			if( (cto=read(sock1,linea,1024)) < 0 ) // lee hasta 1024 caracteres de
				error("read");		   // sock1, los pone en linea
			else if( cto == 0 )	// si lectura devuelve 0 ==> parar ejecucion
				break;
			linea[cto]=0;	// marcar fin del buffer con ``0´´
			
			/* Imprime en pantalla dirección de internet del cliente desde donde vienen datos */
			printf("\nDe la direccion[ %s ] : puerto[ %d ] --- llega el mensaje:\n",
					inet_ntoa(sin1.sin_addr),
					ntohs(sin1.sin_port));
			printf("%s \n",linea);
		}
	}
	
	close(sock1);
	close(sock);
	return 0;
}
/* Archivo: servidor-tcp-no-bloqueante-con-puerto-reutilizable.c */
