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
#include <semaphore.h>


#define SEMINIT 1
#define PORT 3535
#define HASH_SIZE 1000
#define BACKLOG 8
#define MSGSIZE 32
#define MAXCLIENTS 4


//-----------------------------------------------------------------------------------------------------------------------------------------
// Definición de variables de control threads
//-----------------------------------------------------------------------------------------------------------------------------------------


//MUTEX----------------------------------------------------------------------------------------------------------------------------------------
/*Definición del mutex*/
pthread_mutex_t mutex;

//SEMAFOROS----------------------------------------------------------------------------------------------------------------------------------------
/*definición de la variable semaforo*/
sem_t semaforo;



//-----------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------


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

int hashTable[HASH_SIZE];
int cantidad_clientes;
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

struct setVar {
	int clientefd;
	char direccionIp[32];
};

int structSize = sizeof(struct DogType);

void toLowerStr(char *str){
	int lenght = strlen(str);
	int i;
	for(i = 0; i < lenght; i++){
		str[i] = tolower(str[i]);
	}
}

int funHash(char* str){ // tomado de https://stackoverflow.com/questions/7666509/hash-function-for-string
	char new[32];
	snprintf(new, 32, "%s", str);
	toLowerStr(new);
	
	int hash, i;
	int len = strlen(new);
	
	
    for(hash = i = 0; i < len; ++i)
    {
        hash += new[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    
    return (int)fabs(hash % HASH_SIZE);
}
void insertar(int *hashTable1, struct DogType *mascota, FILE *file ){
	/*Adquiere el lock para el mutex*/
	//pthread_mutex_lock(&mutex);

	/*Adquiere el semaforo para el inicio*/
	//sem_wait(&semaforo);
	
	/*Seccion critica*/
	int fun = funHash(mascota->nombre);
	
	mascota->last = hashTable1[fun];
	fwrite(mascota, structSize, 1, file);
	
	hashTable1[fun] = cantidadDeRegistros;
	cantidadDeRegistros++;
	/*seccion critica*/

	/*Suelta el semaforo*/
	//sem_post(&semaforo);


	/*Suelta el valor del lock del mutex*/
	pthread_mutex_unlock(&mutex);
}

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

void search(char* nombre, int *hashTable1, int clientfd){
	
	int posicion = hashTable1[funHash(nombre)];
	FILE *file;
	file = fopen("dataDogs.dat","r");
	if(file == NULL){
		return ;
	}
	struct DogType *mascota;
	mascota = malloc(structSize);
	char nombreS[32];
	toLowerStr(nombre);
	int r;
	while(posicion != -1){	
		fseek(file, posicion * structSize, SEEK_SET);		
		fread(mascota, structSize, 1, file);
		snprintf(nombreS, 32, "%s", mascota->nombre);
		toLowerStr(nombreS);
		
		if(strcmp(nombreS,nombre)==0){
			r = send(clientfd, mascota, structSize,0);
			if(r == -1){
				perror("Error en send search mascota");
				exit(-1);
			}
			posicion++;
			r = send(clientfd, (int *)&posicion, sizeof(int),0);
			if(r == -1){
				perror("Error en send search posicion ");
				exit(-1);
			}
		}
		posicion=mascota->last;			
	}
	snprintf(mascota->nombre, 32, "%s", "EstructuraFinal");
	r = send(clientfd, mascota, structSize,0);
	if(r == -1){
		perror("Error en send search mascota");
		exit(-1);
	}
	
	fclose(file);
	free(mascota);
}
//Funcion para eliminar una estructura especificada del arreglo
void removeFromFile(int index){
	
	FILE *file;
	FILE *temp;
	temp = fopen("temp.dat","w+");
	file = fopen("dataDogs.dat","r");
	fseek(file, 0, SEEK_SET);

	/*Adquiere el lock del mutex*/
	//pthread_mutex_lock(&mutex);

	/*Adquiere el semaforo para el inicio*/
	//sem_wait(&semaforo);

	/*Sección crítica*/
	struct DogType *mascota;
	mascota = malloc(structSize);
	
	for(int i = 1; fread(mascota, structSize,1,file); i++){				
		if(i!=index){
			fwrite(mascota, structSize,1, temp);
		}
	}
	/*Seccion critica*/

	
	/*Suelta el semaforo*/
	//sem_post(&semaforo);


	/*Libera el lock del mutex*/
	//pthread_mutex_unlock(&mutex);

	free(mascota);
	fclose(file);
	fclose(temp);
	system("rm dataDogs.dat");
	system("mv temp.dat dataDogs.dat");
	
	cantidadDeRegistros = 0;
}

void getData(int *hashAp){

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
	
	struct DogType *mascota;
	mascota = malloc(structSize);
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
	mascota->last = -1;
	FILE *file;
	file = fopen("dataDogs.dat","a+"); 
	insertar(hashAp, mascota, file);
	free(mascota);
	fclose(file);
}
//Funcion para ver la información dada en una estructura
void leer(int a,int clientfd){
	FILE *file;
	file = fopen("dataDogs.dat", "r");
	fseek(file, (a-1)*structSize, SEEK_SET);
	int r;
	struct DogType *mascota;
	mascota = malloc(structSize);
	fread(mascota, structSize, 1,file);
	printDogType(mascota, a);

	r = send(clientfd, mascota, structSize,0);
	if(r == -1){
		perror("Error en send leer");
		exit(-1);
	}
	fclose(file);
	free(mascota);

}
void initHash(int *hashTable1){
	cantidadDeRegistros=0;

	int i;
	FILE *file;
	file = fopen("dataDogs.dat", "r");

	for(i = 0; i < HASH_SIZE; i++){
		hashTable1[i]=-1;		
	}	

	if(file == NULL){ //Verificar si existe
		return;
	}
	
	fclose(file);
	system("mv dataDogs.dat temp2.dat");
	file =fopen("temp2.dat", "r");
	
	
	struct DogType *mascota;
	mascota = malloc(structSize);
	FILE *new;
	new = fopen("dataDogs.dat","a+");
	int fun;
	
	
	while(fread(mascota, structSize, 1, file)){
		
		mascota->last = -1; //0.222 segs
		
		fun = funHash(mascota->nombre);//2.208 segs
		mascota->last = hashTable1[fun];

	
		fwrite(mascota, structSize, 1, new);
		
		
		
		hashTable1[fun] = cantidadDeRegistros;
		cantidadDeRegistros++;
								
	}
	
	
	fclose(new);
	fclose(file);
	free(mascota);
	remove("temp2.dat");
	
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
void function(struct setVar *sock_id){

	int clientfd, r;

	char direccion[32]; 

	clientfd =  sock_id->clientefd;
	
	snprintf(direccion,32,"%s",sock_id->direccionIp);	

	r = send(clientfd, "Conexion Exitosa", MSGSIZE,0);
	if(r == -1){
		perror("Error en send");
		exit(-1);
	}	
	
	puts("Conexión exitosa");	
	
	//validar

	struct DogType *mascota;
	mascota = malloc(structSize);

	int menu = 0;
	bool flag = true;
	char trash[MSGSIZE];

	FILE *file;
	
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


				r=recv(clientfd,mascota,structSize,0);
				
				log_send(direccion,"inserción",mascota->nombre);		
				//validar
				if(r==-1){
					perror("Error en recv de ingresar datos...\n");
					exit(-1);
				}
				//Seccion critica para escritura func insertar .
				file = fopen("dataDogs.dat","a+");
				insertar(hashTable,mascota,file);
				fclose(file);
				break;
			
			case 2:
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
					
					//Sección critica varios archivos pueden abrirse al tiempo				
					leer(menu,clientfd);
					//
					sprintf(trash,"%d",menu);
					log_send(direccion,"lectura",trash);
					
				}else{
					printf("El Numero de registro no es valido\n");
				}
		  		break;
		  	case 3:
				r = send(clientfd, (int *)&cantidadDeRegistros, sizeof(int),0);
				if(r == -1){
					perror("Error en send case 2");
					exit(-1);
				}
				r=recv(clientfd, &menu,sizeof(int),0);
				printf("%i\n",menu);
				//validar
				if(r==-1){
					perror("Error en recv del case 3\n");
					exit(-1);
				}
				if(menu <= cantidadDeRegistros && menu>0){
					//Seccion critica para escritura func removeFromFile
		  			removeFromFile(menu);
					sprintf(trash,"%d",menu);
					log_send(direccion,"borrado",trash);
					initHash(hashTable);
					//
				}else{
					printf("El Numero de registro no es valido\n");
				}
		  		break;
		  	case 4:
		  		r=recv(clientfd,trash,MSGSIZE,0);
				if(r==-1){
					perror("Error en recv del case 4\n");
					exit(-1);
				}
				//Sección critica varios archivos pueden abrirse al tiempo				
				search(trash,hashTable,clientfd);
				log_send(direccion,"búsqueda",trash);
				break;
			case 5:
				flag=false;
				break;
			default:
				printf("Opcion Incorrecta\n");
				break;
			}

	}
	free(mascota);
	close(clientfd);
	cantidad_clientes--;	
}

int main(){
	
	initHash(hashTable);
	pthread_t hilo;

	socklen_t tama;
	struct sockaddr_in server, client;
	int servfd, clientfd, r; 

	cantidad_clientes = 0;

//-----------------------------------------------------------------------------------------------------------------------------------------
//  creación de variables control threads	
//-----------------------------------------------------------------------------------------------------------------------------------------	
//MUTEX----------------------------------------------------------------------------------------------------------------------------------------
/*Creación del lock*/
//pthread_mutex_init(&mutex,NULL);
//SEMAFORO----------------------------------------------------------------------------------------------------------------------------------------
/*Iniciación de la variable sem_t en 1*/
//sem_init(&semaforo,0,SEMINIT);

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
	struct setVar *var;
	var = malloc(sizeof(struct setVar));	
	while(true){
		clientfd = accept(servfd, (struct sockaddr *)&client, &tama);
		cantidad_clientes++;
		if(cantidad_clientes > MAXCLIENTS){
			puts("Clientes máximos alcanzados");
			send(clientfd, "Exit",MSGSIZE,0);
		} else{
			if(clientfd == -1){
				perror("Error en accept");
				exit(-1);
			}
			printf("Conexión del cliente :%i\n",cantidad_clientes);
			var->clientefd=clientfd;
			inet_ntop(AF_INET,&client.sin_addr.s_addr,var->direccionIp,32);
			if(pthread_create(&hilo, NULL , function , var)<0){
				perror("Error al crear los hilos");
				exit(-1);
			}
		}
	}

	
	return 0;
}




