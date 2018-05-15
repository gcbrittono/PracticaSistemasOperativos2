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
#define STRUCTSIZE sizeof(struct DogType)

struct DogType *mascota;
	
struct DogType {
        char nombre[32];
        char tipo[32];
        int edad;
        char raza[16];
        int estatura;
        double peso;
        char   sexo;
};
void printDogType(){
	printf("\nNombre: \t%s\n", mascota->nombre);
	printf("Tipo: \t\t%s\n", mascota->tipo);
	printf("Edad: \t\t%i\n", mascota->edad);
	printf("Raza: \t\t%s\n", mascota->raza);
	printf("Estatura: \t%i\n", mascota->estatura);
	printf("Peso: \t\t%.2lf\n", mascota->peso);
	printf("Sexo: \t\t%c\n\n", mascota->sexo);
}

void getData(){

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
		scanf("%d", &edad);
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
		scanf("%d", &estatura);
		if(estatura<0){
			printf("La estatura debe ser positiva\n");
		}else{
			break;
		}
	}

	printf("Ingrese el peso de su mascota:");

	while(true){	
		scanf("%lf", &peso);
		if(peso<0){
			printf("El peso debe ser positivo\n");
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
	

	snprintf(mascota->nombre,32,"%s",nombre);
	snprintf(mascota->tipo,32,"%s",tipo);
	mascota->edad = edad;
	snprintf(mascota->raza,32,"%s",raza);
	mascota->estatura = estatura;
	mascota->peso = peso;
	mascota->sexo = sexo;

	printDogType(mascota);
		
}

int main(int argc, char *argv[]){

	char trash[32];
	int clientfd,r;
	char msg[32];
	int menu = 0;
	int menu_int;
	mascota = malloc(sizeof(struct DogType));
	
	struct sockaddr_in client;
	socklen_t tama=sizeof(struct sockaddr);

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
//	client.sin_addr.s_addr= inet_addr("127.0.0.1");
//	bzero(client.sin_zero,8);


	inet_pton(AF_INET, "127.0.0.1", &client.sin_addr);	

	//Conexión con el cliente
	r=connect(clientfd,(struct sockaddr*)&client, tama);
	//validar
	if(r==-1){
		perror("Error en connect de r\n");
		exit(-1);
	}


	r=recv(clientfd,msg,MSGSIZE,0);
	//validar
	printf("%s\n",msg);

	bool flag=true;
	
	while(flag){
		printf("Menuº: \n 1. Ingresar registro \n 2. Ver registro \n 3. Borrar registro \n 4. Buscar registro \n 5. Salir \n");
		scanf("%i", &menu);
		//menu_int = htonl(menu);
		r = send(clientfd, (int *)&menu, sizeof(int),0);


		if(r == -1){
			perror("Error en send menu principal");
			exit(-1);
		}

		switch(menu){
			case 1:
				getData();
				r = send(clientfd, mascota, STRUCTSIZE,0);
				if(r == -1){
					perror("Error en send");
					exit(-1);
				}				
				break;
			/*case 2:

				printf("El numero de registros es: %i\nIngrese el Numero de registro: ", cantidadDeRegistros);
				scanf("%i",&registerToRead);

				if(registerToRead <= cantidadDeRegistros && registerToRead>0){
					read(registerToRead);
				}else{
					printf("El Numero de registro no es valido\n");
				}
		  		break;
		  	case 3:
				printf("El numero de registros es: %i\nIngrese el Numero de registro: ", cantidadDeRegistros);
				scanf("%i",&registerToRead);
				if(registerToRead <= cantidadDeRegistros && registerToRead>0){

		  			removeFromFile(registerToRead);
					hashTable=readHashTable();

				}else{

					printf("El Numero de registro no es valido\n");

				}
						  		
		  		
		  		break;
		  	case 4:
		  		printf("Ingrese el nombre de la mascota: ");
				scanf("%s",busqueda);
				search(busqueda,hashTable);
				break;
		  	*/
			case 5:
				flag=false;

				break;
			
			default:
				printf("Opcion Incorrecta\n");
				break;
			}
		if(flag){
			printf("Presione cualquier tecla para volver al menu principal \n");
			scanf("%s", trash);
		}

	}



	close(clientfd);


return 0;
}
