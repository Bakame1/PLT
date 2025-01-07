#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include "anasynt.c"
#include "analex.c"


void test_synt(char* expression){
    // Définir la locale pour gérer les caractères UTF-8 correctement
    setlocale(LC_ALL, "");

    // Exemple d'expression avec les symboles Unicode, incluant la flèche double
    char** lexemes = CreationListeLexeme(expression);
    ASTNode* ast = analyseur_syntaxique(lexemes);


    // Afficher l'AST
    printf("\nArbre Syntaxique de l'expression ");
    printf("%s : \n\n",expression);
    printAST(ast, 0);

    // Libérer la mémoire de l'AST
    freeAST(ast);
    
}

int main() {
    test_synt("(p1⇒p2)→((¬p1)∨p2)");
    test_synt("(p1∨(p2∧p3))→((p1∨p2)∧(p1∨p3))");
    test_synt("(p1∨p2)→(p2∨p1)");
    test_synt("(p1∧p2)→(p2∧p1)");
    test_synt("(¬(p1∧p2))→((¬p1)∨(¬p2))");
    test_synt("(¬(p1∨p2))→((¬p1)∧(¬p2))");
    test_synt("(¬(¬p1))→p1");
    

    return 0;
}
