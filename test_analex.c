#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include "analex.c"

void test(char* expression){
    // Définir la locale pour gérer les caractères UTF-8 correctement
    setlocale(LC_ALL, "");

    // Exemple d'expression avec les symboles Unicode, incluant la flèche double
    char** lexemes = CreationListeLexeme(expression);

    printf("Liste des lexèmes de l'expression ");
    printf("%s : \n\n",expression);
    printf("[");
    
    for (int i = 0; lexemes[i] != NULL; i++) {
        printf("%s,", lexemes[i]);
        free(lexemes[i]); // Libérer chaque lexème après utilisation
    }
    printf("]\n\n");
    free(lexemes); // Libérer la liste des lexèmes


}


// Exemple d'utilisation
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
