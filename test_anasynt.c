#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

//Lien vers les 2 analyseurs precedents
#include "anasynt.c"
#include "analex.c"

//Fonction test réalisant une analyse syntaxique sur une expression choisie
//Parametre expression : chaine de caracteres
//Affiche l'arbre syntaxique si l'analyse est réussie sinon une erreur
void test_synt(char* expression){
    //Gestion des caractères UTF-8
    setlocale(LC_ALL, "");

    //Création de la liste de lexemes
    char** lexemes = CreationListeLexeme(expression);
    
    //Analyse syntaxique à partir de cette liste de lexmes
    ASTNode* ast = analyseur_syntaxique(lexemes);


    //Affichage de l'arbre syntaxique
    printf("\nArbre Syntaxique de l'expression ");
    printf("%s : \n\n",expression);
    printAST(ast, 0);

    //On libère la mémoire de l'arbre syntaxique
    freeAST(ast);
    
}

//Fonction principale avec differentes règles de logique comme tests
int main() {

    //tests valides
    printf("\nTests valides : \n\n");
    test_synt("(p1⇒p2)→((¬p1)∨p2)");
    test_synt("(p1∨(p2∧p3))→((p1∨p2)∧(p1∨p3))");
    test_synt("(p1∨p2)→(p2∨p1)");
    test_synt("(p1∧p2)→(p2∧p1)");
    test_synt("(¬(p1∧p2))→((¬p1)∨(¬p2))");
    test_synt("(¬(p1∨p2))→((¬p1)∧(¬p2))");
    
    
    //tests invalides
    //A tester une par une car l'execution s'arrete dès lors qu'il y a une erreur sémantique
    //Empechant alors de tester toutes les expressions invalides à la suite
    printf("\nTests invalides : \n\n");
    
    test_synt("(p1⇒p2");//Parenthese fermante manquante
    //test_synt("p1⇒p2)");//Parenthese ouvrante manquante
  

    return 0;
}
