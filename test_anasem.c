#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "analex.c"
#include "anasynt.c"
#include "anasem.c"


//Fonction de test réalisant l'analyse sémantique sur une expression donnée
//Parametre expression : chaine de caracteres
//Affiche le résultat de l'analyse sémantique :
//Affiche l'arbre de l'analyse syntaxique et dit si l'analyse sémantique a réussie ou non
void test_sem(const char *expression) {
    printf("\n=== Test de l'analyse sémantique pour l'expression : %s ===\n", expression);

    //Analyse léxicale : création de la liste de lexemes
    char** lexemes = CreationListeLexeme(expression);
    if (!lexemes) {
        fprintf(stderr, "Erreur : Analyse lexicale échouée pour l'expression : %s\n", expression);
        return;
    }

    //Analyse syntaxique : géneration d'un arbre syntaxique
    ASTNode* ast = analyseur_syntaxique(lexemes);
    if (!ast) {
        fprintf(stderr, "Erreur : Analyse syntaxique échouée pour l'expression : %s\n", expression);
        return;
    }

    //Affichage de l'arbre syntaxique avant l'analyse sémantique
    printf("AST avant l'analyse sémantique :\n");
    printAST(ast, 0);

    //Analyse sémantique : vérification de la validité de l'arbre syntaxique
    printf("\nRésultat de l'analyse sémantique :\n");
    analyseur_semantique(ast);

    printf("Analyse sémantique réussie pour l'expression : %s\n", expression);

    //On libère la mémoire allouée à l'arbre syntaxique
    freeAST(ast);
}

//Fonction principale pour exécuter les tests
int main() {
    //Gestion des caractères UTF-8
    setlocale(LC_ALL, "");

    //Initialiser les propositions valides
    initialize_valid_props();

    //Tests avec des expressions valides
    test_sem("(p1⇒p2)→((¬p1)∨p2)");
    test_sem("(p1∨(p2∧p3))→((p1∨p2)∧(p1∨p3))");
    test_sem("(p1∧p2)→(p2∧p1)");

    //Tests avec des expressions invalides
    //A tester une par une car l'execution s'arrete dès lors qu'il y a une erreur sémantique
    //Empechant alors de tester toutes les expressions invalides
    
    test_sem("(p4∧p1)"); // Proposition invalide : p4
    //test_sem("(p1∧)");   // Opérateur AND sans opérande droit
    //test_sem("¬");       // Opérateur NOT sans opérande
    //test_sem("(p1⇒)");   // Opérateur IMPLIQUE sans opérande droit
    //test_sem("(∧p1)");   // Opérateur AND sans opérande gauche

    //On libère la mémoire allouée aux propositions valides
    free_valid_props_memory();

    return 0;
}
