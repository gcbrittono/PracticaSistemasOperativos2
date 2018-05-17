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
#include <errno.h>
#include <sys/select.h>
#include <pthread.h>
#include <time.h>


#define PORT 3535
#define HASH_SIZE 1000
#define BACKLOG 8
#define MSGSIZE 32
#define STRUCTSIZE sizeof(struct DogType)
#define MAXCLIENTS 4




/*struct sockaddr_in{
	short sin_family;
	unsigned short sin_port;
	struct in_addr sind_addr;
	char sin_zero[8] ;
};
struct in_addr{
	unsigned long sind_addr.s_addr;
};
*/
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
int cantidad_clientes;
int cantidadDeRegistros = -1;


struct ListNode{
	struct ListNode *next;
	struct ListNode *tail;
	int data;
	bool used;
};
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
struct DogType {
        char nombre[32];
        char tipo[32];
        int edad;
        char raza[16];
        int estatura;
        double peso;
        char   sexo;
};
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
struct ListNode *readHashTable(struct ListNode *hashTable){ 
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

void *function(void *sock_id); //Función que genera hilos para controlar clientes

void printDogType(struct DogType *mascota);//Funcion para mostrar en pantalla la estructura 
int regCant();
void leer(int a); //Funcion para ver la información dada en una estructura
void removeFromFile(int index); //Funcion para eliminar una estructura especificada del arreglo


int main(int argc, char const *argv[]){
	pthread_t hilo;


	char trash[32];
	cantidadDeRegistros=regCant();
	socklen_t tama;
	struct sockaddr_in server, client;
	int servfd, clientfd, r; 

	cantidad_clientes = 0;
//-----------------------------------------------------------------------------------------------------------------------------------------
//  Creacion del socket() servfd	
//-----------------------------------------------------------------------------------------------------------------------------------------	
	//Creación del descriptor del socket para el servidor
	//Opciones son IPv4, TCP, 0 par IP 
	servfd = socket(AF_INET, SOCK_STREAM, 0);
	if(servfd == -1){
		perror("Error en socket");
		exit(-1);
	}
	puts("Creacion del socket");
//-----------------------------------------------------------------------------------------------------------------------------------------
// configuracion para poder utilizar el sockect repetidas veces
//-----------------------------------------------------------------------------------------------------------------------------------------

	// configuracion para poder utilizar el sockect repetidas veces y dar configuraciones al puerto
	if (setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &(int){ 1 }, sizeof(int)) < 0){
    		perror("setsockopt(SO_REUSEADDR) failed");
	}

	//Aqui se fijan los parametros de configuración necesarios para la estructura del servidor
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(server.sin_zero,8);
	
	
//-----------------------------------------------------------------------------------------------------------------------------------------
// creacion bind
//-----------------------------------------------------------------------------------------------------------------------------------------

	//Esta función se encarga de ligar al socket servfd con el puerto que utilizaremos para la transmisión , aqui &server es un apuntador a esa estructura que contiene los parametros relacionados con el numero de puerto y la dirección ip
	r = bind(servfd, (struct sockaddr *)&server, sizeof(struct sockaddr_in));
	if(r == -1){
		perror("Error en bind");
		exit(-1);
	}
	puts("Bind del socket creado");

//-----------------------------------------------------------------------------------------------------------------------------------------
// El socket entra en modo escucha hasta aceptar la conexion
//-----------------------------------------------------------------------------------------------------------------------------------------
	
	r = listen(servfd, BACKLOG);
	if(r == -1){
		perror("Error en listen");
		exit(-1);
	}

	puts("Esperando conexion...");
//-----------------------------------------------------------------------------------------------------------------------------------------
// Se conectan el cliente y el servidor
//-----------------------------------------------------------------------------------------------------------------------------------------
int val = 5;
	while(true){
		clientfd = accept(servfd, (struct sockaddr *)&client, &tama);
		cantidad_clientes++;
		if(cantidad_clientes > MAXCLIENTS){
			puts("Clientes máximos alcanzados");
			send(clientfd, "Exit",MSGSIZE,0);
			//send(clientfd,(int *)&val,sizeof(int),0);
			//sleep(5);
			
		}
		if(clientfd == -1){
			perror("Error en accept");
			exit(-1);
		}
		printf("Conexión del cliente :%i\n ",cantidad_clientes);
		if(pthread_create(&hilo, NULL , function , (void*) &clientfd)<0){
				perror("Error al crear los hilos");
				exit(-1);
			}
	}
	

	return 0;
}

//[Fecha YYYYMMDDTHHMMSS] Cliente [IP] [inserción | lectura | borrado | búsqueda] [registro
//| cadena buscada ]
void log_send(char*cliente, char*funcion, char*busqueda){
	FILE *file;
	file = fopen("serverDogs.log","a+");
	char temp[20];
	
	time_t t = time(NULL);
	char logm [200] = "Fecha ";


	struct tm *tm = localtime(&t);
	
	sprintf(temp,"%d",(tm->tm_year)+1900);	
	
	strcat(logm, temp);
	
	sprintf(temp,"%.2d",(tm->tm_mon)+1);	

	strcat(logm, temp);

	sprintf(temp,"%.2d",tm->tm_mday);	

	strcat(logm, temp);

	sprintf(temp,"%.2d",tm->tm_hour);	

	strcat(logm, temp);
	
	sprintf(temp,"%.2d",tm->tm_min);
		
	strcat(logm, temp);

	sprintf(temp,"%.2d",tm->tm_sec);	

	strcat(logm, temp);
	
	strcat(logm, " Cliente ");
	
	strcat(logm, cliente);
	
	strcat(logm, " ");

	strcat(logm, funcion);
	
	strcat(logm, " ");

	strcat(logm, busqueda);

	strcat(logm, "\n");
	printf("%s",logm);

	fwrite(logm,strlen(logm),1,file);
	fclose(file);
}

//Funcion para cada uno de los hilos para el control de las conexiones con los clientes
void *function(void *sock_id){

	struct DogType *mascota;
	
	mascota = malloc(sizeof(struct DogType));

	char trash[32];
	cantidadDeRegistros=regCant();
	socklen_t tama;
	struct sockaddr_in server, client;
	int servfd, clientfd, r; 

	clientfd =  *(int*)sock_id;

	r = send(clientfd, "Conexion Exitosa", MSGSIZE,0);
	if(r == -1){
		perror("Error en send");
		exit(-1);
	}	
	
	puts("Conexión exitosa");	
	
	//validar
	bool flag = true;
	int menu;
	struct ListNode *hashTable;
	hashTable = getHashTable();
	readHashTable(hashTable);
	while(flag){

		r=recv(clientfd, &menu,sizeof(int),0);

		printf("%i\n",menu);
		//validar
		if(r==-1){
			perror("Error en recv del menu\n");
			exit(-1);
		}
		
		switch(menu){
			case 1:


				r=recv(clientfd,mascota,STRUCTSIZE,0);
				
				log_send(inet_ntoa(client.sin_addr),"inserción",mascota->nombre);		
				//validar
				if(r==-1){
					perror("Error en recv de ingresar datos...\n");
					exit(-1);
				}

				printDogType(mascota);

				FILE *file;
				file = fopen("dataDogs.dat", "a+");
				fwrite(mascota, sizeof(struct DogType),1, file);
				fclose(file);
				
				
				break;
			
			case 2:
				cantidadDeRegistros=regCant();
				r = send(clientfd, (int *)&cantidadDeRegistros, sizeof(int),0);
				if(r == -1){
					perror("Error en send case 2");
					exit(-1);
				}
				r=recv(clientfd, &menu,sizeof(int),0);
						
				printf("%i\n",menu);
				//validar
				if(r==-1){
					perror("Error en recv del case 2\n");
					exit(-1);
				}
				if(menu <= cantidadDeRegistros && menu>0){					
					leer(menu);
					r = send(clientfd, mascota, STRUCTSIZE,0);
					sprintf(trash,"%d",menu);
					log_send(inet_ntoa(client.sin_addr),"lectura",trash);
					if(r == -1){
						perror("Error en send");
						exit(-1);
					}
				}else{
					printf("El Numero de registro no es valido\n");
				}
		  		break;
		  	case 3:
				r=recv(clientfd, &menu,sizeof(int),0);
				printf("%i\n",menu);
				//validar
				if(r==-1){
					perror("Error en recv del case 3\n");
					exit(-1);
				}
				
				if(menu <= cantidadDeRegistros && menu>0){
		  			removeFromFile(menu);
					sprintf(trash,"%d",menu);
					log_send(inet_ntoa(client.sin_addr),"borrado",trash);
				}else{
					printf("El Numero de registro no es valido\n");
				}
						  		
		  		
		  		break;
		  	/*case 4:
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

	}

	close(clientfd);
	close(servfd);
	cantidad_clientes--;
	
		
	
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
int regCant(){
	FILE *file;
	file = fopen("dataDogs.dat", "r");
	if (file==NULL){
		return 0;
	}else{
		
		int size=0;

		fseek(file, 0, SEEK_END); // seek to end of file
		size = ftell(file); // get current file pointer
		fseek(file, 0, SEEK_SET);
	
		fclose(file);
		return(size/104);
	}
}
void leer(int a){
	FILE *file;
	file = fopen("dataDogs.dat", "r");
	struct DogType *mascota;
	mascota = malloc(sizeof(struct DogType));
	fseek(file, (a-1)*104, SEEK_SET);

	fread(mascota, sizeof(struct DogType),1,file);

	printDogType(mascota);
	free(mascota);
	fclose(file);
};
void removeFromFile(int index){
	FILE *file;
	FILE *temp;
	temp = fopen("temp.dat","w+");
	file = fopen("dataDogs.dat","r");
		
	struct DogType *mascota;
	mascota = malloc(sizeof(struct DogType));
	
	for(int i = 1; i<=cantidadDeRegistros; i++){
		fread(mascota, sizeof(struct DogType),1,file);		
		if(i!=index){
			fwrite(mascota, sizeof(struct DogType),1, temp);
		}
	}

	free(mascota);
	fclose(file);
	fclose(temp);
	system("rm dataDogs.dat");
	system("mv temp.dat dataDogs.dat");

}


