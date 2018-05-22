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
#include <errno.h>


#define PORT 3535
#define MSGSIZE 32

int cantidadDeRegistros = 0;
	
struct DogType {
	char nombre[32];
	char tipo[32];
	int edad;
	char raza[16];
	int estatura;
	double peso;
	char sexo;
	int last;
};

int structSize = sizeof(struct DogType);

//Funcion para mostrar en pantalla la estructura 
void printDogType(struct DogType *mascota,int pos){
	printf("\nNombre: \t%s\n", mascota->nombre);
	printf("Tipo: \t\t%s\n", mascota->tipo);
	printf("Edad: \t\t%i\n", mascota->edad);
	printf("Raza: \t\t%s\n", mascota->raza);
	printf("Estatura: \t%i\n", mascota->estatura);
	printf("Peso: \t\t%.2lf\n", mascota->peso);
	printf("Sexo: \t\t%c\n", mascota->sexo);
	printf("Last: \t\t%i\n", mascota->last + 1);
	printf("Posicion: \t%i\n\n", pos);
}

void getData(int clientfd){

	char nombre[32];
	char tipo[32];	
	int edad;
	char raza[32];
	int estatura;
	double peso;
	char sexo;
	
	printf("Ingrese el nombre de su mascota:");
	scanf("%s", nombre);
	printf("Ingrese el tipo de mascota:");
	scanf("%s", tipo);
	printf("Ingrese la edad de su mascota:");

	while(true){	
		scanf("%i", &edad);
		if(edad<0){
			printf("La edad debe ser mayor o igual que 0\n");
		}else{
			break;
		}
	}

	printf("Ingrese la raza de su mascota:");
	scanf("%s", raza);
	printf("Ingrese la estatura de su mascota:");

	while(true){	
		scanf("%i", &estatura);
		if(estatura<0){
			printf("La estatura debe ser mayor o igual que 0\n");
		}else{
			break;
		}
	}

	printf("Ingrese el peso de su mascota:");

	while(true){	
		scanf("%lf", &peso);
		if(peso<0){
			printf("El peso debe ser mayor o igual que 0\n");
		}else{
			break;
		}
	}

	printf("Ingrese el sexo de su mascota:");

	while(true){	
		scanf(" %c", &sexo);
		if(sexo =='M' || sexo =='F'){
			break;
		}else{
			printf("El sexo de debe ser M o F\n");
		}
	}

	int len=strlen(nombre)-1;

	if(nombre[len]=='\n'){
		nombre[len] = '\0';				
	}
	
	struct DogType *mascota;
	mascota = malloc(structSize);
	snprintf(mascota->nombre,32,"%s",nombre);
	snprintf(mascota->tipo,32,"%s",tipo);
	mascota->edad = edad;
	snprintf(mascota->raza,32,"%s",raza);
	mascota->estatura = estatura;
	mascota->peso = peso;
	mascota->sexo = sexo;
	mascota->last = -1;
	printDogType(mascota,0);
	int r;
	r = send(clientfd, mascota, structSize,0);
	if(r == -1){
		perror("Error en send");
		exit(-1);
	}
	free(mascota);	
}

int main(){
	struct sockaddr_in client;
	struct DogType *mascota;
	mascota = malloc(structSize);
	socklen_t tama=sizeof(struct sockaddr);
	int clientfd, r;
	//se define elsocket que utilizara el cliente
	clientfd = socket(AF_INET,SOCK_STREAM,0);
	//validar
	if(clientfd==-1){
		perror("Error en socket del clientfd\n");
		exit(-1);
	}

	//se inicializan las variables de la estructura de destino	
	client.sin_family= AF_INET;
	client.sin_port = htons(PORT);
	client.sin_addr.s_addr= inet_addr("127.0.0.1");
	bzero(client.sin_zero,8);

	//Conexión con el cliente
	r=connect(clientfd,(struct sockaddr*)&client, tama);
	//validar
	if(r==-1){
		perror("Error en connect de r\n");
		exit(-1);
	}

	int menu = 0;
	bool flag = true;
	char msg[MSGSIZE];

	r=recv(clientfd,msg,MSGSIZE,0);
	//validar
	printf("%s\n",msg);
	while(flag){
		if(strcmp(msg,"Exit") == 0){
			puts("Numero de clientes maximos alcanzados, intente mas tarde");		
			break;
		}
		
		printf("Menuº: \n 1. Ingresar registro \n 2. Ver registro \n 3. Borrar registro \n 4. Buscar registro \n 5. Salir \n");
		scanf("%i", &menu);

		r = send(clientfd, (int *)&menu, sizeof(int),0);
		if(r == -1){
			perror("Error en send menu principal");
			exit(-1);
		}

		switch(menu){
			case 1:
				getData(clientfd);	
				break;
			case 2:
				r=recv(clientfd, &cantidadDeRegistros,sizeof(int),0);
				if(r == -1){
					perror("Error en recv case 2");
					exit(-1);
				}	
				printf("El numero de registros es: %i\nIngrese el Numero de registro: ", cantidadDeRegistros);
				scanf("%i",&menu);

				r = send(clientfd, (int *)&menu, sizeof(int),0);
				if(r == -1){
					perror("Error en send menu principal");
					exit(-1);
				}

	
				if(menu <= cantidadDeRegistros && menu>0){
					

					r=recv(clientfd,mascota,structSize,0);
		
					//validar
					if(r==-1){
						perror("Error en recv de case 2...\n");
						exit(-1);
					}

					printDogType(mascota,menu);

					char nameFile[200]="";

					char commandLine[100]="gedit ";

					char n[64]={0};

					strcat(nameFile,mascota->nombre);
					strcat(nameFile,mascota->tipo);
					strcat(nameFile,mascota->raza);
					nameFile[strlen(nameFile)]=mascota->sexo;
					sprintf(n, "%d", mascota->edad);
					strcat(nameFile,n);
					strcat(commandLine, nameFile);
					system(commandLine);			
					
				}else{
					printf("El Numero de registro no es valido\n");
				}
		  		break;
		  	case 3:
				r=recv(clientfd, &cantidadDeRegistros,sizeof(int),0);
				if(r == -1){
					perror("Error en recv case 2");
					exit(-1);
				}
				printf("El numero de registros es: %i\nIngrese el Numero de registro: ", cantidadDeRegistros);
				scanf("%i",&menu);

				r = send(clientfd, (int *)&menu, sizeof(int),0);
				if(r == -1){
					perror("Error en send case 3");
					exit(-1);
				}				

				if(menu > cantidadDeRegistros || menu<=0){
					printf("El Numero de registro no es valido\n");
				}
						  		
		  		
		  		break;
		  	case 4:
		  		printf("Ingrese el nombre de la mascota: ");
				scanf("%s",msg);
				r = send(clientfd, msg, MSGSIZE,0);
				if(r == -1){
					perror("Error en send case 4");
					exit(-1);
				}
				while(true){
					r=recv(clientfd,mascota,structSize,0);
					if(r==-1){
						perror("Error en recv case 4\n");
						exit(-1);
					}
					if(strcmp(mascota->nombre,"EstructuraFinal") == 0){
						break;
					} else {
						r=recv(clientfd,&menu,sizeof(int),0);
						if(r==-1){
							perror("Error en recv posicion case 4\n");
							exit(-1);
						}
						printDogType(mascota,menu);
					}
				}
				break;
			case 5:
				flag=false;

				break;
			
			default:
				printf("Opcion Incorrecta\n");
				break;
			}
		if(flag){
			printf("Presione cualquier tecla para volver al menu principal \n");
			scanf("%s", msg);
		}

	}


	free(mascota);
	close(clientfd);


return 0;
}
