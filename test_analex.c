// Memes librairies que dans analex.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

//On relie au fichier analex.c pour réaliser tous les tests
#include "analex.c"


//Permet d'afficher un tableau de lexemes a partir d'une chaine de caractere quelconque en argument
//Parametre expression : chaine de caracteres
//Affiche la liste des lexemes de l'expression choisie
void test(char* expression){
    //Gestion des caractères UTF-8
    setlocale(LC_ALL, "");

    //Generation de la liste de lexemes grâce à la fonction de analex.c
    char** lexemes = CreationListeLexeme(expression);

    //Affichage de la liste des lexemes
    printf("Liste des lexèmes de l'expression ");
    printf("%s : \n\n",expression);
    printf("[");
    for (int i = 0; lexemes[i] != NULL; i++) {
        printf("%s,", lexemes[i]);
        free(lexemes[i]); //Libérer la mémoire utilisée par chaque lexeme
    }
    printf("]\n\n");
    
    //Libérer la mémoire utilisée par la liste des lexemes
    free(lexemes); 


}


//Fonction principale avec les differentes règles de logique provenant du fichier
int main() {

	test("(p1⇒p2)→((¬p1)∨p2)");
	test("(p1∨(p2∧p3))→((p1∨p2)∧(p1∨p3))");
	test("(p1∨p2)→(p2∨p1)");
	test("(p1∧p2)→(p2∧p1)");
	test("(¬(p1∧p2))→((¬p1)∨(¬p2))");
	test("(¬(p1∨p2))→((¬p1)∧(¬p2))");
	test("(¬(¬p1))→p1");
	
    	return 0;
    
}
