#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <termios.h>




#define PORT 3535
#define BUFFERL 10

int mygetch ( void ) 
{
//Función  importante para el "Press any key to continue"
	int ch;
	struct termios oldt, newt;
	
	tcgetattr ( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr ( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr ( STDIN_FILENO, TCSANOW, &oldt );
	return ch;
}

struct datosBusqueda{
	int origen;
	int destino;
	int hora;
};

int revisarDatos(struct datosBusqueda data, int aux){
//esta función no deja que se de la orden de inicio de búsqueda si no se han modificado los datos primero
if(data.origen==-1){
	printf("No ha ingresado el ID del origen \n");
	aux = -1;
}
if(data.destino==-1){
	printf("No ha ingresado el ID del destino \n");
	aux = -1;
}
if(data.hora==-1){
	printf("No ha ingresado la hora del viaje \n ");
	aux = -1;
}
return aux;
}
int menu(){

	
	
 return 0;   
}

int main(int argc, char *argv[]){

	time_t tiempo_inicio, tiempo_final;
	double segundos;
	int clientfd,r;
	int select = 0;
	int aux = 0;
	struct sockaddr_in client;
	struct hostent *he;
	struct datosBusqueda d1 = {-1,-1,-1};
	char buffer[BUFFERL];
	clientfd = socket(AF_INET, SOCK_STREAM, 0);

	if(clientfd<0){
		perror("error en socket");
		exit(-1);
	}
	client.sin_family = AF_INET;
	client.sin_port = htons(PORT);
	inet_aton("127.0.0.1", &client.sin_addr);

	r= connect(clientfd, (struct sockaddr*)&client, (socklen_t)sizeof(struct sockaddr));
	if(r<0){
		perror("error en connect");
		exit(-1);
	}
	startover:
		do {
		system("clear");
		printf("Bienvenide al menu principal\n");
		printf("1. Ingresar origen \n");
		printf("2. Ingresar destino \n");
		printf("3. Ingresar hora\n");
		printf("4. Buscar tiempo de viaje medio \n");
		printf("5. Salir \n");
		printf("Seleccione una opción (1-5) \n");
		scanf("%d", &select);
		}
		while((select < 1) && (select > 4));
	
		switch(select)
		{
			case 1:
				printf("Origen\n");
				scanf("%d", &d1.origen);
				sprintf(buffer,"%d", d1.origen);
				strcat(buffer , ",1" );
				r=send(clientfd, buffer, BUFFERL, 0);
				if(r<0){
			perror("error en send");
			exit(-1);
			}
			system("clear");
			printf("Has ingresado el dato: %d en origen. \n", d1.origen);
			printf("%c Press any key to continue.\n", getc(stdin));
				mygetch();				
				goto startover;
			case 2:
				printf("Destino \n");
				scanf("%d",&d1.destino);
				sprintf(buffer,"%d", d1.destino);
				strcat(buffer , ",2" );
				r=send(clientfd, buffer, BUFFERL, 0);
				if(r<0){
			perror("error en send");
			exit(-1);
			}
			system("clear");
			printf("Has ingresado el dato: %d en destino. \n", d1.destino);
			printf("%c Press any key to continue.\n", getc(stdin));
				mygetch();

				goto startover;
			case 3:
				printf("Hora\n");
				scanf("%d", &d1.hora);
				sprintf(buffer,"%d", d1.hora);
				strcat(buffer , ",3	" );
				r=send(clientfd, buffer, BUFFERL, 0);
				if(r<0){
			perror("error en send");
			exit(-1);
			}
			system("clear");
			printf("Has ingresado el dato: %d hora. \n", d1.hora);
			printf("%c Press any key to continue.\n", getc(stdin));
				mygetch();
				goto startover;
			case 4:
				system("clear");
				printf("Buscando...\n");
				strcpy(buffer, "0,4");
				if(revisarDatos(d1, aux)==-1){
					printf("Ingrese los datos completos \n");
					goto startover;
				} else {	
					printf ("Origen = %d, Destino = %d, Hora = %d \n", d1.origen, d1.destino, d1.hora);
					r=send(clientfd, buffer, BUFFERL, 0);
					if(r<0){
						perror("error en send");
						exit(-1);
					}
				//printf("%c Press any key to continue.\n", getc(stdin));
				//mygetch();
				r= recv(clientfd, buffer, 8, 0);
				if(r<0){
					perror("error en recv");
					exit(-1);
				}
				buffer[r]=0;
				printf("\n Tiempo de viaje medio: %s", buffer);}
				printf("%c Press any key to continue.\n", getc(stdin));
				mygetch();
				goto startover;	
			case 5:
				system("clear");
				printf("Saliendo...\n");
				printf("%c Press any key to continue.\n", getc(stdin));
				mygetch();
				printf("Bye.\n");
				strcpy(buffer, ",5");
				r=send(clientfd, buffer, BUFFERL, 0);
				if(r<0){
			perror("error en send");
			exit(-1);
			}
			close(clientfd);
				exit(0);
				//return 0;
			default:
				printf("Por favor selecione una opción válida\n");
				goto startover;
		}





}
