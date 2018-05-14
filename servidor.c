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

#define HASH_SIZE 1000
#define BACKLOG 8
#define MSGSIZE 32
#define STRUCTSIZE sizeof(struct DogType)

int cantidadDeRegistros = -1;

/*struct sockaddr_in{
	short sin_family;
	unsigned short sin_port;
	struct in_addr sind_addr;
	char sin_zero[8] ;
};

struct in_addr{
	unsigned long s_addr;
};
*/

struct DogType {
        char nombre[32];
        char tipo[32];
        int edad;
        char raza[16];
        int estatura;
        double peso;
        char   sexo;
};

struct ListNode{
	struct ListNode *next;
	struct ListNode *tail;
	int data;
	bool used;
};

int funHash(char* str){ // tomado de https://stackoverflow.com/questions/7666509/hash-function-for-string
	int hash, i;
	int len = strlen(str);
	
    for(hash = i = 0; i < len; ++i)
    {
        hash += str[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return (int)fabs(hash % HASH_SIZE);
}

struct ListNode * getEmptyListNode(){
	struct ListNode * salida = malloc(sizeof(struct ListNode));
	salida->next = NULL;
	salida->used = false;
	salida->data = -1;
	salida->tail = salida;
	return salida;
}

struct ListNode * getHashTable(){
	struct ListNode *hashTable;
	hashTable = malloc(HASH_SIZE * sizeof(struct ListNode));
	int j;
	struct ListNode new;
	for(j = 0; j < HASH_SIZE; j++){
		new = *getEmptyListNode();
		hashTable[j] = new;
	}
	return hashTable;
}

int setElementInHash(struct ListNode *hashAp, int data, char *nombre){
	
	int hashVal = funHash(nombre);

	struct ListNode *actual = &hashAp[hashVal];

	if(actual->used){
		struct ListNode *last = actual->tail;
		last->data = data;
		last->used = true;
		last->next = getEmptyListNode();
		actual->tail = last->next;			
	}else{
		actual->data=data;
		actual->used = true;
		actual->next=getEmptyListNode();
		actual->tail= actual->next;
	}
	
	return hashVal;
}

void printDogType(struct DogType *mascota){
	printf("\nNombre: \t%s\n", mascota->nombre);
	printf("Tipo: \t\t%s\n", mascota->tipo);
	printf("Edad: \t\t%i\n", mascota->edad);
	printf("Raza: \t\t%s\n", mascota->raza);
	printf("Estatura: \t%i\n", mascota->estatura);
	printf("Peso: \t\t%.2lf\n", mascota->peso);
	printf("Sexo: \t\t%c\n\n", mascota->sexo);
}
struct ListNode *readHashTable(){
	struct ListNode * hashTable = getHashTable();
	FILE *file;
	file = fopen("dataDogs.dat", "r");
	if(file != NULL){
		struct DogType *mascota;
    		mascota = malloc(sizeof(struct DogType));
    		int i = 1;
		while(fread(mascota, sizeof(struct DogType),1,file)){
			setElementInHash(hashTable, i, mascota->nombre);
			i++;
		}
		fclose(file);
		free(mascota);
		cantidadDeRegistros = i - 1;
	}else{
		cantidadDeRegistros = 0; 
	}
	return hashTable;
}
int main(){
	
	struct DogType *mascota;
	mascota = malloc(sizeof(struct DogType));
	struct ListNode *hashTable;	
	hashTable = readHashTable();

	char trash[32];

	socklen_t tama;
	struct sockaddr_in server, client;
	int servfd, clientfd, r; 
	char msg[32];
	int msgInd;
	int msgInd_int;
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

	r = send(clientfd, "Conexion Exitosa", MSGSIZE,0);
	if(r == -1){
		perror("Error en send");
		exit(-1);
	}	
	
	//validar
	bool flag = true;
	while(flag){

		r=recv(clientfd,&msgInd_int,MSGSIZE,0);
		msgInd = ntohl(msgInd_int);
		printf("%i\n",msgInd);
		//validar
		if(r==-1){
			perror("Error en recv del menu\n");
			exit(-1);
		}

		
		switch(msgInd){
			case 1:

				r = -1;
				while(r<=0){
					r=recv(clientfd,&mascota,STRUCTSIZE,0);
					printf("r es igual a: %i\n", r);				

				}
				//validar
				if(r==-1){
					perror("Error en recv de ingresar datos\n");
					exit(-1);
				}

				setElementInHash(hashTable,cantidadDeRegistros+1,mascota->nombre);
				printDogType(mascota);

				FILE *file;
				file = fopen("dataDogs.dat", "a+");
				fwrite(mascota, sizeof(struct DogType),1, file);
				fclose(file);
				
				cantidadDeRegistros++;
				
				r = send(clientfd, "Conexion Exitosa", MSGSIZE,0);
				
				if(r == -1){
					perror("Error en send");
					exit(-1);
				}				

				break;
			/*
			case 2:

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
	close(servfd);
	
	free(mascota);	

	
return 0;
}
