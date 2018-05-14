#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>

#define BACKLOG 8

void sendStr(char * str, int clientfd){
	int r;
	r = send(clientfd, str, 128,0);
	if(r == -1){
		perror("Error en send");
		exit(-1);
	}
	
}

void recvStr(int clientfd, char* str){
	int r;
	
}

int main(){
	
	char input[128];

	socklen_t tama;
	struct sockaddr_in server, client;
	int servfd, clientfd, r; 
	char msg[128];
	int menu;
	tama = sizeof(struct sockaddr);

	servfd = socket(AF_INET, SOCK_STREAM, 0);
	if(servfd == -1){
		perror("Error en socket");
		exit(-1);
	}
	// configuracion para poder utilizar el sockect repetidas veces
	if (setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0){
    		perror("setsockopt(SO_REUSEADDR) failed");
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(3535);
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(server.sin_zero,8);
	tama = sizeof(struct sockaddr_in);
	r = bind(servfd, (struct sockaddr *)&server, tama);
	if(r == -1){
		perror("Error en bind");
		exit(-1);
	}
	//validar
	r = listen(servfd, BACKLOG);
	if(r == -1){
		perror("Error en listen");
		exit(-1);
	}

	clientfd = accept(servfd, (struct sockaddr *)&client, &tama);
	if(clientfd == -1){
		perror("Error en accept");
		exit(-1);
	}
	
	bool flag = true;
	while(flag){
		
		r = send(clientfd,"Menuº: \n 1. Ingresar registro \n 2. Ver registro \n 3. Borrar registro \n 4. Buscar registro \n 5. Salir \n", 128,0);
		if(r == -1){
			perror("Error en send");
			exit(-1);
		}
		r=recv(clientfd, input, 128, 0);
		if(r==-1){
			perror("Error en recv\n");
			exit(-1);
		}
		printf("Recibido:%s\n", input);
		menu=atoi(input);	
		switch (menu){
			case 1:
				r = send(clientfd,"Ingrese el nombre de su mascota:",128,0);
				if(r == -1){
					perror("Error en send");
					exit(-1);
				}
				r=recv(clientfd, input, 128, 0);
				if(r==-1){
					perror("Error en recv\n");
					exit(-1);
				}
				printf("Recibido:%s\n", input);
				r = send(clientfd,"Ingrese el tipo de mascota:",128,0);
				if(r == -1){
					perror("Error en send");
					exit(-1);
				}
				r=recv(clientfd, input, 128, 0);
				if(r==-1){
					perror("Error en recv\n");
					exit(-1);
				}
				printf("Recibido:%s\n", input);
				r = send(clientfd,"Ingrese la edad de su mascota:",128,0);
				if(r == -1){
					perror("Error en send");
					exit(-1);
				}
				r=recv(clientfd, input, 128, 0);
				if(r==-1){
					perror("Error en recv\n");
					exit(-1);
				}
				printf("Recibido:%s\n", input);
			
				break;
			default:
				
				break;		
		}

	}

	close(clientfd);
	close(servfd);
	
return 0;
}
