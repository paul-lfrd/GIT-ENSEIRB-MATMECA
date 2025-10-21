#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#define BUFFSIZE 256



int main (int argc, char *argv[]){
int sd;
int len;
int s;

struct sockaddr_in sa;		/* Structure Internet sockaddr_in */
struct hostent *hptr ; 		/* Infos sur le serveur */
struct servent *sp;			/* Structure service Internet */
char *myname; 				/*pointeur sur le nom du programme*/
char buf[BUFFSIZE];
char *serveur ;        		/* Nom du serveur distant */
char *fin = "quit";

int port;			/* Pointeurs sur le serveur et l'utilisateur */ 
char user[BUFFSIZE];
myname = argv[0];

if (argc != 3) {
printf("Usage : %s serveur user\n", myname); exit(1);
}

serveur = argv[1]; /* Recuperation nom du serveur */

port = atoi(argv[2]); /* Recuperation numero de port */


if((hptr = gethostbyname(serveur)) == NULL) /* Recuperation des infos sur le serveur dans /etc/hosts pour par DNS */
{
	printf("Probleme de recuperation d'infos sur le serveur\n");
	exit(1);
}

bcopy((char *)hptr->h_addr, (char*)&sa.sin_addr, hptr->h_length); /* Initialisation la structure sockaddr sa avec les infos  formattees : */

sa.sin_family = hptr->h_addrtype; /* Famille d'adresse : AF_INET ici */


sa.sin_port = htons(port); /* Initialisation du numero du port */
char *ip_str = inet_ntoa(sa.sin_addr);
printf("%s : %d\n", ip_str, ntohs(sa.sin_port));

int quit = 1;
while(quit){ /*boucle pour pouvoir faire plusieurs demandes de calcul*/
	printf("calcul à effectuer?\n");
	int k = read(0, buf, BUFFSIZE);
	buf[k - 1] = '\0';
	
	if(strcmp(buf,fin)==0){ /*Condition de sortie du serveur*/
		printf("on quitte\n");
		quit = 0;
	}
	else{
		if((s = socket (AF_INET, SOCK_STREAM, 0)) < 0){ /* Création de la socket client */
			perror("socket");
			exit(1);
		}

		if (connect(s, (struct sockaddr *)&sa, sizeof(sa)) < 0){ /*Connexion au serveur, infos dans la structure adresse internet sa */
			perror("connect");
			exit(1);
		}
		printf("Connexion établie avec le serveur\n");

		fflush(stdout); /* Envoi de la requête */ 
		fflush(stdin);

		buf[k]='\0';
		printf("Envoi de la requête : %s\n", buf);
		write(s, buf, k);

		int n = read(s, buf, BUFFSIZE); /* Lecture de la réponse */ 
		buf[n]='\0';
		
		printf("Réponse : %s\n", buf);  /* Affichage de la réponse */ 
			
		close(s);
	}
}

exit(0); /* Fermeture de la connexion */
}