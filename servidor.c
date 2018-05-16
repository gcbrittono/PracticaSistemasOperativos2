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

#define PORT 3535
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

void printDogType(){
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

	fseek(file, (a-1)*104, SEEK_SET);

	fread(mascota, sizeof(struct DogType),1,file);

	printDogType(mascota);

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


int main(int argc, char const *argv[]){
	
	mascota = malloc(sizeof(struct DogType));

	char trash[32];
	cantidadDeRegistros=regCant();
	socklen_t tama;
	struct sockaddr_in server, client;
	int servfd, clientfd, r; 

	
	
	//Creación del descriptor del socket para el servidor
	//Opciones son IPv4, TCP, 0 par IP 
	servfd = socket(AF_INET, SOCK_STREAM, 0);
	if(servfd == -1){
		perror("Error en socket");
		exit(-1);
	}

	// configuracion para poder utilizar el sockect repetidas veces y dar configuraciones al puerto
	if (setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &(int){ 1 }, sizeof(int)) < 0){
    		perror("setsockopt(SO_REUSEADDR) failed");
	}

	//Aqui se fijan los parametros de configuración necesarios para la estructura del servidor
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(server.sin_zero,8);
	
	

	//Esta función se encarga de ligar al socket servfd con el puerto que utilizaremos para la transmisión , aqui &server es un apuntador a esa estructura que contiene los parametros relacionados con el numero de puerto y la dirección ip
	r = bind(servfd, (struct sockaddr *)&server, sizeof(struct sockaddr_in));
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
	int menu;
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
	
	free(mascota);	

	
return 0;
}
