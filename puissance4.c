/*PROJET PG108 Puissance 4*/

#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<math.h>
#include<wchar.h>
#include<locale.h>

#define NB_colonnes 7
#define NB_rangees 6

char NomJoueur1[30];
char NomJoueur2[30];
char Joueur2Virtuel = 0;
int JoueurEnCours = 1;
char GrilleDeJeu[NB_rangees*NB_colonnes];

int indice(char r, char c){
    return r*NB_colonnes + c;
}

void ChangementDeJoueur(){  /*fonction qui permet de changer la valeur du joueur qui est en train de jouer*/
    if (JoueurEnCours==1){
        JoueurEnCours = 2;
    }
    else if( JoueurEnCours==2){
        JoueurEnCours= 1;
    }
}

int CalculeColonneAleatoire(char tableau[NB_rangees*NB_colonnes]){  /*fonction qui calcule une colonne aléatoire lorsque le joueur 2 est l'ordinateur*/
    int aleatoire;
    aleatoire = rand()%6; /*valeur aléatoire modulo 6 pour que la colonne soit bien dans la grille de jeu*/
    while (tableau[indice(0,aleatoire)] != 0){
        aleatoire = (aleatoire + 3)%6;
    }
    return aleatoire;
}

int init(char J1[30], char J2[30], char tableau[NB_rangees*NB_colonnes]){ /*fonction d'initialisation du jeu*/
    printf (" Nom du joueur 1:\n");
    fgets(J1, 30, stdin); /*récupération du nom du joueur 1*/
    printf("Nom du joueur 2:\n");
    fgets(J2, 30, stdin); /*récupération du nom du joueur 2*/
    if (J2[0] == '\n' || strlen(J2) == 0){ /*si il n'y a pas de joueur 2 le jeu se fait contre l'ordinateur*/
        Joueur2Virtuel = 1;
        printf("Jeu contre l'ordinateur\n");
    }
    for(int i = 0; i < NB_rangees*NB_colonnes; i++){
            tableau[i] = 0;
    }
return 0;
}

void display(char tableau[NB_rangees*NB_colonnes]){ /*fonction qui affiche la grille de jeu et la met à jour à chaque tour*/
    for (int i = 0; i < NB_rangees; i++){ 
        for (int j = 0; j < NB_colonnes; j++){ /*on parcourt les rangées et les colonnes en utilisant la fonction indice*/
            if(tableau[indice(i,j)]==0){ /*si la valeur dans le tableau est 0, on affiche un point dans la grille de jeu, ce qui correspond à une case vide*/
                printf("%c ", '|');
                printf("%c ", '.');
            }
            else if(tableau[indice(i,j)]==1){ /*si la valeur dans le tableau est 1, alors on place le jeton du joueur 1 qui est rouge*/
                printf("%c ", '|');
                printf("\033[1;31m");
                printf("%lc ", L'●'); /*jeton rouge avec UNICODE*/
                printf("\033[0m");
            }
            else if(tableau[indice(i,j)]==2){ /*si la valeur dans le tableau est 2, alors on place le jeton du joueur 2 qui est jaune*/
                printf("%c ", '|');
                printf("\033[1;33m");
                printf("%lc ", L'●'); /*jeton rouge avec UNICODE*/
                printf("\033[0m");
            }
            else{
                printf("%c ", '|'); /*si aucune des conditions n'est repectée à cause d'u ne potentielle erreur, on affiche un z dans la grille qui signale un problème*/
                printf("%c ", 'z');
            }
        }
        printf("|");
    printf("\n");
    }
    printf("C'est au joueur %d de jouer \n", JoueurEnCours);
}

int colonne_de_joueur(int JoueurEnCours, char tableau[NB_rangees*NB_colonnes]){ /*fonction qui met à jour la colonne où le joueur en cours à choisi de jouer*/
    char colonne_str[29];
    char colonne_str2[29];
    int colonne;
    if(Joueur2Virtuel == 1 && JoueurEnCours == 2){ /*condition pour que l'ordinateur joue si le joeur 2 est virutel*/
        colonne = CalculeColonneAleatoire(GrilleDeJeu);
        int i = NB_rangees - 1;
        while(tableau[i*NB_colonnes+colonne]!=0){
            i=i-1;
        }
        tableau[indice(i,colonne)]=JoueurEnCours;
    }
    else{
    printf ("Dans quelle colonne voulez-vous jouer\n");
    fgets(colonne_str, 29, stdin);
    colonne = atoi(colonne_str); 
    colonne = colonne - 1;   
    if (colonne<0 || colonne>6){
            printf ("Colonne invalide, dans quelle colonne voulez-vous jouer\n");
            fgets(colonne_str2, 29, stdin);
            colonne = atoi(colonne_str2);
    }
    if (tableau[colonne]==1 || tableau[colonne]==2){
        printf ("Colonne pleine, dans quelle colonne voulez-vous jouer\n");
        fgets(colonne_str2, 29, stdin);
        colonne = atoi(colonne_str2);
        colonne = colonne - 1; 
    } 
    int i = NB_rangees - 1;
    while(tableau[i*NB_colonnes+colonne]!=0){
            i=i-1;
    }
    tableau[indice(i,colonne)]=JoueurEnCours;
    }
    ChangementDeJoueur();
    printf("Joueur %d \n", JoueurEnCours);
    printf("\033[H\033[J");
return 0;
}

int detecte_plein(char tableau[NB_rangees*NB_colonnes], int JoueurEnCours){ /*fonction qui detecte lorsque la grille de jeu est remplie*/
    for (int i = 0; i < NB_rangees*NB_colonnes; i++){ /*on parcourt les cases de la première rangée et de chaque colonne, si il y a un 0 le jeu continu*/
        if (tableau[i] == 0){
         return 0;
        }
    }
    JoueurEnCours = 10; /*si chaque case est remplie alors le jeu s'arrête car la grille est pleine*/
    return 1;
}

int TestLigne(char tableau[NB_rangees*NB_colonnes]){ /*sous-fonction qui vérifie si 4 jetons de la même couleur sont en lignes*/
    for (int i = 0; i < NB_rangees; i++){
        int jetons = 1; /*compteur de jetons alignés*/
        for (int j = 0; j < NB_colonnes - 2; j++){
            if (tableau[indice(i,j)] == tableau[indice(i,j+1)] && tableau[indice(i,j)]!= 0){
                jetons++;
                if (jetons == 4){
                    return tableau[indice(i,j)]; /*la fonction renvoie le numéro du joueur qui a 4 jetons alignés*/
                }
            }
            else{
                jetons = 1;
            }
        }
    }
    return 0;
}

int TestColonne(char tableau[NB_rangees*NB_colonnes]){ /*sous-fonction qui vérifie si 4 jetons de la même couleur sont en colonne*/
    for (int j = 0; j < NB_colonnes; j++){
        int jetons = 1; /*compteur de jetons alignés*/
        for (int i = 0; i < NB_rangees - 1; i++){
        if (tableau[indice(i,j)] == tableau[indice(i+1,j)] && tableau[indice(i,j)]!= 0){
            jetons ++;
            if (jetons == 4){
                return tableau[indice(i,j)]; /*la fonction renvoie le numéro du joueur qui a 4 jetons alignés*/
            }
        }
        else{
            jetons = 1;
        }
        }
    }
    return 0;
}

int TestDiagPos(char tableau[NB_rangees*NB_colonnes]){ /*sous-fonction qui vérifie si 4 jetons de la même couleur sont en diagonale positive*/
    for (int j = 0; j < NB_colonnes - 3; j++){
        for (int i = 3; i < NB_rangees; i++){
            if ((tableau[indice(i,j)] == tableau[indice(i-1,j+1)]) && (tableau[indice(i-1,j+1)] == tableau[indice(i-2,j+2)]) && (tableau[indice(i-2,j+2)] == tableau[indice(i-3,j+3)]) && tableau[indice(i,j)]!= 0){
                return tableau[indice(i,j)];
            }
            else{
                ;
            }
        }
    }
    return 0;
}

int TestDiagNeg(char tableau[NB_rangees*NB_colonnes]){ /*sous-fonction qui vérifie si 4 jetons de la même couleur sont en diagonale négative*/
    for (int j = 0; j < NB_colonnes - 3; j++){
        for (int i = 0; i < NB_rangees - 3; i++){
            if ((tableau[indice(i,j)] == tableau[indice(i+1,j+1)]) && (tableau[indice(i+1,j+1)] == tableau[indice(i+2,j+2)]) && (tableau[indice(i+2,j+2)] == tableau[indice(i+3,j+3)]) && tableau[indice(i,j)]!= 0){
                return tableau[indice(i,j)];
            }
            else{
                ;
            }
        }
    }
    return 0;
}

int TestVainqueur(char tableau[NB_rangees*NB_colonnes]){ /*fonctions qui vérifie si les 4 sous-fonctions précédentes sont vérifiées au cours d'un tour*/
    if(TestColonne(GrilleDeJeu) != 0){
        printf("Le joueur %d a gagné la partie \n", TestColonne(GrilleDeJeu));
        return TestColonne(GrilleDeJeu);
    }
    else if(TestLigne(GrilleDeJeu) != 0){
        printf("Le joueur %d a gagné la partie \n", TestLigne(GrilleDeJeu));
        return TestLigne(GrilleDeJeu);
    }
    else if(TestDiagPos(GrilleDeJeu) != 0){
        printf("Le joueur %d a gagné la partie \n", TestDiagPos(GrilleDeJeu));
        return TestDiagPos(GrilleDeJeu);
    }
    else if(TestDiagNeg(GrilleDeJeu) != 0){
        printf("Le joueur %d a gagné la partie \n", TestDiagNeg(GrilleDeJeu));
        return TestDiagNeg(GrilleDeJeu);
    }
    else{
        ;
    }
    return 0; /*renvoie le numéro du joueur qui a gagné la partie*/
}



int main(){
    setlocale(LC_CTYPE,"");
    init(NomJoueur1, NomJoueur2, GrilleDeJeu);
    display(GrilleDeJeu);
    while(TestVainqueur(GrilleDeJeu) == 0){
        colonne_de_joueur(JoueurEnCours, GrilleDeJeu);
        display(GrilleDeJeu);
        if (detecte_plein(GrilleDeJeu, JoueurEnCours) == 1){
            printf(" La grille de jeu est pleine");
            exit(0);
        }
        else{
            TestVainqueur(GrilleDeJeu);
        }
    }
    exit(0);
    return 0;
}
