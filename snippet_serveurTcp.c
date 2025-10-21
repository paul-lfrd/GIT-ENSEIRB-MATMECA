#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>

#define MAX_REQUEST 10 // nombre maximal de client que le serveur peut gérer en même temps
#define BUFF_SIZE 256 // taille max du buffer 

char* calculatrice(char cal[], int n){
    int nb = 0 ;
    char s_a[BUFF_SIZE];
    char s_b[BUFF_SIZE];
    double a; 
    double b;
    char op = '0';
    int m = 0 ; // longueur de a
    int j = -1 ; // indice de l'op
    int k = 0 ; // longueur de b
    int etat = 0 ; 
    double res = 0; 
    char* ret = (char*)malloc((n+1)*sizeof(char)); // allocation de la mémoire
    for(int i = 0 ; i < n&& cal[i]!='\0'; i ++ ){
        if ((cal[i]== '+'|| cal[i]== '-' || cal[i]== '*'||cal[i]== '/')&& op =='0'){
            op = cal[i]; 
            j = i ; 
        }
        else {
            if (j == -1 ){
                m +=1 ; 
                s_a[i] = cal[i];
            }
            else if (op!='0'){
                s_b[i-j-1] = cal[i];
                k += 1;
            }
        }
    }
    if((m != 0 && k != 0)&& (op != '0')){
        nb=3;
    }
    a = atof(s_a); 
    b = atof(s_b);
    if (nb == 3){
        // printf("op = %c\t j = %d\n",op,j);
        // printf("s_a = %s \t s_b = %s \n",s_a,s_b); 
        // printf("a = %f \t b = %f\n m = %d \t k = %d\n",a,b,m,k);

        if (op == '*'){
            res = a *b ;
            etat = 1;
            // printf("res* = %f\n",res);  
        }
        if (op == '+'){
            res = a+b;
            etat = 1;
            // printf("res+ = %f \n ",res);  
        }
        if (op == '-'){
            res = a - b;
            etat = 1; 
            // printf("res- = %f \n ",res);  
        }
        if (op == '/'){
            if (b == 0){
                etat = 2;   
            }
            else {
                res = a/b;
                etat = 1;
            }
            // printf("res/ = %f \n",res);
        }
        if (etat == 0 ){
            printf("on ne connait pas l'op\n");
            sprintf(ret,"on ne connait pas l'op"); 
        }
        else if (etat == 2){
            sprintf(ret,"Erreur math, division par zéro"); 
            printf("Division par 0\n");
        }
        else{
            sprintf(ret,"%.02f",res); // permet de convertir proprement le float en chaine de caractère
            //gcvt(res,n,ret)// cela nous retourne une écriture scientifique avec n chiffres significatifs 
            printf("ret = %s \t strlen = %lu \n ", ret,strlen(ret));
        }
    }
    
    else {
        printf("Erreur dans la requette\n"); 
        sprintf(ret,"Erreur dans la requette");
    }
    printf("ret = %s\n",ret);
    return ret ;
}


int main(int argc,char *argv[])
{
    int sd;
    int port; 
    struct sockaddr_in sa; /* Numero de port du serveur */

    /* Structure Internet sockaddr_in */
    int newsd; /* Id de la socket entrante */
    struct sockaddr_in newsa; /* adresse de la socket de la connection entrante */
    int newsalength; /*taille de l'adresse de la nous */
    int i;
    char buff[BUFF_SIZE]; /*initialisation du buffer*/
    int n ; 
    /* verification du nombre d'arguments de la ligne de commande */
    if (argc != 2) {
        printf("serveurTCP. Erreur d'arguments\n");
        printf("Syntaxe : %% snippet_serveurTCP numero_port\n");
        exit(1);
    }
    /* Recuperation numero port que l'on a passe en argument */
    port = atoi(argv[1]);

    /* Initialisation la structure sockaddr sa */
    /* Famille d'adresse : AF_INET = PF_INET */
    sa.sin_family = AF_INET;

    /* Initialisation du numero du port */
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = INADDR_ANY;

    /* Creation de la socket TCP */
    if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Probleme lors de la creation de socket\n");
        exit(1);
    }
    /* Bind sur la socket */
    if(bind(sd, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
        printf("Probleme avec le bind\n");
        exit(1);
    }
    /* Initialisation de la queue d'ecoute des requetes*/
    listen(sd, MAX_REQUEST);
    printf("Serveur TCP en ecoute...\n");
    i = 0;

    while(1) {
        /* newsalength contient la taille de la structure de la nouvelle connection sa */
        newsalength = sizeof(newsa) ;
        if((newsd = accept(sd, (struct sockaddr *)&newsa, &newsalength)) < 0 ) {
          printf("Erreur sur accept\n");
          exit(1);
        }

        pid_t pid = fork(); // on crée le processus parallèle 

        if (pid < 0) { // en cas de problème lors de l'initialisation du fork on ferme le socket 
        perror("Erreur fork");
        close(newsd);
        continue;
    }

    if (pid == 0) {
        // Processus de parallélisation
        close(sd); // Le process n’a pas besoin de la socket d’écoute global 
        // /*Début de la discussion avec le client N°i*/
		// /*On écoute la connexion entrante*/
        char buff[BUFF_SIZE];
        int n = read(newsd, buff, BUFF_SIZE);

        if (n <= 0) {
            perror("Erreur lecture client");
            close(newsd);
            exit(1);
        }
         // /* nom du client */ 
        printf("Client connecté depuis le port %d : %s\n", ntohs(newsa.sin_port), buff);

        char *res_calc = calculatrice(buff, strlen(buff));// on reserve la bonne taille mémoire

        sleep(5);//simulation d'un traitement plus long pour montrer la parrallélisation

        write(newsd, res_calc, strlen(res_calc));// envoi de la réponse au bon client
        printf("Résultat envoyé : %s\n", res_calc); 

        free(res_calc);
        close(newsd); // fermeture de la connexion avec le client 
        exit(0); // fin du process
    } 
    else{
        close(newsd);   // on ferme le socket de la connexion si on ne rentre dans aucun des autres cas 
    }

        //script sans parrallélisation
        // /* Compteur du nombre de connexion */
        // i++;
        // /* nom du client */ 
        // printf("Connection N° %d sur le port %d...\nDebut de la discussion\n", i, ntohs(newsa.sin_port));
		// /*Début de la discussion avec le client N°i*/
		// /*On écoute la connexion entrante*/
		// n = read(newsd, buff, BUFF_SIZE);
		// printf("Réponse n°%d : %s\t n= %d \t taille de la répone : %lu\n ",i,buff,n,strlen(buff));
        // char* res_calc = (char*)malloc(strlen(buff)*sizeof(char));// on reserve la bonne taille mémoire
        
		// /*Traitement de la requête*/
        // res_calc = calculatrice(buff, strlen(buff)); // on applique le programme à la calculatrice    
        // sleep(5);
        // printf("Resultat calculette = %s\n",res_calc);
		// write(newsd, res_calc, strlen(res_calc));//envoi de la reponse
		// printf("Fin de l'envoi au client\n ");
        // free(res_calc);// on libère la mêmoire que l'on a alloué
        // close(newsd);
    
    }
    /* Fermeture du serveur. Never reached */
    close(sd);
    printf("Fin du serveur. Bye...\n");
    exit(0);
}