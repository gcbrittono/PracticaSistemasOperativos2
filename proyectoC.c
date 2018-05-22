#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<ctype.h>
#include<math.h>

#define HASH_SIZE 1000

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
int cantidadDeRegistros = 0;


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

void insertar(int *hashTable, struct DogType *mascota, FILE *file ){
	
	int fun = funHash(mascota->nombre);
	
	mascota->last = hashTable[fun];
	fwrite(mascota, structSize, 1, file);
	
	hashTable[fun] = cantidadDeRegistros;
	cantidadDeRegistros++;
}

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
/*
void leerTodo(){
	FILE *ach;
	int i=0;
	ach = fopen("dataDogs.dat", "r");
	printf("----------Impresión---------\n");
	if(ach==NULL){
		printf("----------Finalizo(noEncontrado)------\n");

		return ;
	}
	struct DogType *mascota;
    	mascota = malloc(structSize);
	while(fread(mascota, structSize, 1, ach)){
		printDogType(mascota,i);
		i++;
	}
	printf("----------Finalizo---------\n");
	fclose(ach);
	free(mascota);
}*/
void search(char* nombre, int *hashTable){
	
	int posicion = hashTable[funHash(nombre)];
	FILE *file;
	file = fopen("dataDogs.dat","r");
	if(file == NULL){
		return ;
	}
	struct DogType *mascota;
	mascota = malloc(structSize);
	char nombreS[32];
	toLowerStr(nombre);
	
	while(posicion != -1){	
		fseek(file, posicion * structSize, SEEK_SET);		
		fread(mascota, structSize, 1, file);
		snprintf(nombreS, 32, "%s", mascota->nombre);
		toLowerStr(nombreS);
		
		//printf(">>%s\t%s\t%i\n", nombreS, nombre, strcmp(nombreS,nombre));
		
		if(strcmp(nombreS,nombre)==0){
			printDogType(mascota,posicion + 1);
			printf("\n");
		}
		posicion=mascota->last;			
	}
	
	fclose(file);
	free(mascota);
}

void removeFromFile(int index){
	
	FILE *file;
	FILE *temp;
	temp = fopen("temp.dat","w+");
	file = fopen("dataDogs.dat","r");
	fseek(file, 0, SEEK_SET);
	struct DogType *mascota;
	mascota = malloc(structSize);
	
	for(int i = 1; fread(mascota, structSize,1,file); i++){				
		if(i!=index){
			fwrite(mascota, structSize,1, temp);
		}
	}

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

void read(int a){
	FILE *file;
	file = fopen("dataDogs.dat", "r");
	fseek(file, (a-1)*structSize, SEEK_SET);

	struct DogType *mascota;
	mascota = malloc(structSize);
	fread(mascota, structSize, 1,file);
	printDogType(mascota, a);

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
	fclose(file);
	free(mascota);

}


void initHash(int *hashTable){
	

	int i;
	FILE *file;
	file = fopen("dataDogs.dat", "r");

	for(i = 0; i < HASH_SIZE; i++){
		hashTable[i]=-1;		
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
		mascota->last = hashTable[fun];

	
		fwrite(mascota, structSize, 1, new);
		
		
		
		hashTable[fun] = cantidadDeRegistros;
		cantidadDeRegistros++;
								
	}
	
	
	fclose(new);
	fclose(file);
	free(mascota);
	remove("temp2.dat");
	
}

int main(){

	int menu = 0;
	bool flag = true;
	char trash[32];

	FILE *archivo;
	
	int hashTable[HASH_SIZE];
	
	initHash(hashTable);
	while(flag){
		/*for(i=0;i<HASH_SIZE;i++){
			if(hashTable[i]!=-1){
				printf("Hash:%iPos:%i\n",i,hashTable[i]);	
			}	
		}*/ 
		//leerTodo();
		printf("Menuº: \n 1. Ingresar registro \n 2. Ver registro \n 3. Borrar registro \n 4. Buscar registro \n 5. Salir \n");
		scanf("%i", &menu);

		switch(menu){
			case 1:
				getData(hashTable);
				break;
			case 2:

				printf("El numero de registros es: %i\nIngrese el Numero de registro: ", cantidadDeRegistros);
				scanf("%i",&menu);
				if(menu <= cantidadDeRegistros && menu>0){
					read(menu);
				}else{
					printf("El Numero de registro no es valido\n");
				}
		  		break;
		  	case 3:

				printf("El numero de registros es: %i\nIngrese el Numero de registro: ", cantidadDeRegistros);
				scanf("%i",&menu);
				if(menu <= cantidadDeRegistros && menu>0){
		  			removeFromFile(menu);
					initHash(hashTable);
				}else{
					printf("El Numero de registro no es valido\n");
				}
		  		break;
		  	case 4:
		  		printf("Ingrese el nombre de la mascota: ");
				scanf("%s",trash);
				search(trash,hashTable);
				break;
		  	case 5:
				flag=false;
				break;
			default:
				break;
			}
		if(flag){
			printf("Presione cualquier tecla para volver al menu principal \n");
			scanf("%s", trash);
		}

	}
	

	return 0;
}
