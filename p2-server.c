 #include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h> 
#include <time.h>

#define PORT 3535
#define BACKLOG 32
#define ADDRSTRLEN 16 

struct travel{
	char *ido;
	char *idd;
	char *hora;
	char *tiempo;
}Travel;

long int searchIdo(int ido)
{/*funcion que busca la posicion almacenada en la hashTable, del ido. Devuelve la posicion del ido en la lista principal

*/
    long auxNum;
    char *auxChar;
	auxChar = malloc(11*sizeof(char));
    FILE *file;
    file = fopen("hashTable.txt", "r");
	if (file==NULL) { fputs ("File error",stderr); exit (1);}
	else{
		fseek(file, 11 * ido, SEEK_SET);
		//fread(&auxChar, 1, 10, file);
		if(fgets(auxChar,11,file)){
			//printf("solo char %s\n",auxChar);
			auxNum = atoi(auxChar);
			//printf("en char %s  en numero %d\n",auxChar,auxNum);
			free(auxChar);
		}else{
			printf("error de lectura %\n");
		}
		
	}
    fclose(file);

    return auxNum;
}

void searchMeanTime(int ido, int idd, int hod,char *time)
{/*funcion que busca en la lista los datos del viaje, recibe los datos a comparar
    recibe un puntero donde almacenara el tiempo encontrado, si no encuentra tiempo 
    retornara una cadena con "-1"
*/
    const char *filename = "lista.txt";             //archivo donde buscamos
    FILE *file = fopen(filename, "r");
    if (!file)
        printf("%s", "No se pudo abrir el archivo");

    char *contents = malloc(50*sizeof(char));       //asigna memoria a la cadena donde almacenamos las filas
    
    int end = 0;
    struct travel tr;                                      //estructura que nos ayuda a separar y almacenar los datos de cada fila
    fseek(file, searchIdo(ido-1), SEEK_SET);
    while (end == 0 && fgets(contents, 50, file))
    {
        // Se extraen las variables de cada renglon

        tr.ido = strtok(contents, ",");
        tr.idd = strtok(NULL,",");
        tr.hora = strtok(NULL,",");
        tr.tiempo = strtok(NULL,",");

		//printf("funcion searchmt %d %d %d %s\n", atoi(tr.ido), atoi(tr.idd), atoi(tr.hora), time);
       
        // Se busca tiempo medio
        if (ido == atoi(tr.ido))
        {
            if (idd == atoi(tr.idd))
            {
                if (hod == atoi(tr.hora))
                {
                    end = 1;
                }
            }
        }
        else
        {
            tr.tiempo = "-1";
            end = 1;
        }
    }
    strcpy(time,tr.tiempo);
	//printf("%s %s",time, tr.tiempo);
    free(contents);
    fclose(file);
    
}

int FechaHora(struct sockaddr *client , char *busqueda, char *origen, char *destino, char *ip){
	//Abre o crea el archivo .log y guarda las búsquedas de cada cliente
	FILE *fp;
	fp  = fopen ("data.log", "a");
	time_t t = time(NULL);
  	struct tm tiempoLocal = *localtime(&t);
	char fechaHora[70];
	char *formato = "%Y-%m-%d %H:%M:%S";
	int bytesEscritos = strftime(fechaHora, sizeof fechaHora, formato, &tiempoLocal);
	if (bytesEscritos != 0) {    
		//printf("Fecha y hora: %s", fechaHora);
		//printf(" ip %s\n", ip);
		fputs("[Fecha: ",fp);
		fputs(fechaHora,fp);
		fputs("] ",fp);
		fputs("Cliente: [",fp);
		fputs(ip,fp);
		fputs("] [",fp);
		fputs(busqueda,fp);
		fputs("-",fp);
		fputs(origen,fp);
		fputs("-",fp);
		fputs(destino,fp);
		fputs("]\n", fp);
		
	}else{
		printf("Error formateando fecha");
		return -1;
	}

	fclose(fp);
	return 0;
}

void recibe(int clientfd, struct sockaddr_in client, struct travel tr){
	
	int r;
	char *tipo;
	tipo = malloc(2*sizeof(char));
	do{
		
		
		int dato;
		//dato = malloc(8*sizeof(int));
		char *time;
		time = malloc(8*sizeof(char));
		char*  buffer;
		buffer = malloc(10*sizeof(char));
		r= recv(clientfd, buffer, 10, 0);
		//printf("%s\n",buffer);
		if(r<0){
			perror("error en recv");
			exit(-1);
		}
		buffer[r]=0;
		dato = atoi(strtok(buffer,","));
		tipo = strtok(NULL,",");
		switch (atoi(tipo))
		{
		case 1:
			tr.ido = malloc(10*sizeof(char));
		    sprintf(tr.ido, "%i", dato);
			//tr.ido = dato;
			//printf("%d %d\n",atoi(tipo), dato);
			break;
		case 2:
			tr.idd = malloc(10*sizeof(char));
			sprintf(tr.idd, "%i", dato);
			//tr.idd = dato;
			//printf("%d %d %s\n",atoi(tipo), dato, tr.idd);
			break;
		case 3:
			//tr.hora = dato;
			tr.hora = malloc(10*sizeof(char));
			sprintf(tr.hora, "%i", dato);
			//printf("%d %s %s\n",atoi(tipo),tr.hora,tr.idd);
			break;
		case 4:
			//printf("case 4 %d %d %d %s", atoi(tr.ido), atoi(tr.idd), atoi(tr.hora), time);
			searchMeanTime(atoi(tr.ido), atoi(tr.idd), atoi(tr.hora), time);
			//printf("%s",time);
			if(strcmp(time,"-1") == 0) {
				strcpy(time, "NA");
			}
			//printf(" %s\n",time);
			r= send(clientfd, time, 8, 0);
			if(r<0){
				perror("error en send to client");
				exit(-1);
			}
			struct in_addr clientIP;
			clientIP = client.sin_addr;
			char ipStr[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &clientIP, ipStr, INET_ADDRSTRLEN);	//encuentra la ip del cliente
			FechaHora(((struct sockaddr*)&client), time, tr.ido, tr.idd, ipStr);
			free(time);
			break;
		default:
			break;
		}
		//free(dato);
	}while(strcmp(tipo,"5")!=0);
	close(clientfd);
	





}

int main(int argc, char *argv[]){
 
	int serverfd, clientfd, r;
	struct sockaddr_in server, client;
	socklen_t socklen;
	//int castT = 20;//atoi(argv[1]);
	char buffer[10];
	int i;
	pid_t childpid;


	serverfd=socket(AF_INET, SOCK_STREAM, 0);
	if (serverfd <0){
		perror("error en socket");
		exit(-1);
	}
	server.sin_family=AF_INET;
	server.sin_port= htons(PORT);
	server.sin_addr.s_addr= INADDR_ANY;
	bzero(server.sin_zero, 8);

	r=bind(serverfd, (struct sockaddr*)&server, sizeof(struct sockaddr));
	if(r<0){
		perror("error en bind");
		exit(-1);
	}
	r= listen(serverfd, BACKLOG);
	if(r<0){
		perror("error en listen");
		exit(-1);
	}
	int cnt = 0;
    while (1) {
	clientfd = accept(serverfd, (struct sockaddr*)&client, &socklen);
	if(r<0){
		perror("Error en accept");
		exit(-1);
	}
	if ((childpid = fork()) == 0) {
 
            // Closing the server socket id
           
 
            // Send a confirmation message
            // to the client
            struct travel tra;
            recibe(clientfd, client, tra);
            close(serverfd);
        }

	// funcion que reciba del cliente almacene en estrctura hasta que llegue un 4 para la busqueda
	//struct travel tra;


	//printf("\naqui vendria funcion de busqueda ");
	//recibe(clientfd,client,tra);
	}


}
