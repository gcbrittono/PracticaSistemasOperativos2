//Cliente
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>

#define PORT 3535
#define STRUCTSIZE sizeof(struct DogType)

int main(){
	char input[128];
	int clientfd,r;
	char msg[128];
	int menu = 0;
	int menu_int;
	
	struct sockaddr_in client;
	socklen_t tama=sizeof(struct sockaddr);
	clientfd = socket(AF_INET,SOCK_STREAM,0);
	//validar
	if(clientfd==-1){
		perror("Error en socket del clientfd\n");
		exit(-1);
	}	
	client.sin_family= AF_INET;
	client.sin_port = htons(PORT);
	client.sin_addr.s_addr= inet_addr("127.0.0.1");
	bzero(client.sin_zero,8);
	r=connect(clientfd,(struct sockaddr*)&client, tama);
	//validar
	if(r==-1){
		perror("Error en connect de r\n");
		exit(-1);
	}
	
	bool flag = true;
	
	while(flag){
		
		r=recv(clientfd, input, 128, 0);
		if(r==-1){
			perror("Error en recv\n");
			exit(-1);
		}
		printf("Recibido:%s\n", input);
		scanf("%s", msg);
		r = send(clientfd, msg, 128,0);
		if(r == -1){
			perror("Error en send");
			exit(-1);
		}
		if(strcmp(msg,"5")==0){
			flag=false;
		}
	}
	
	close(clientfd);
	return 0;
}
