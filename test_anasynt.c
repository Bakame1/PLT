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
    int length = sizeof(lexemes) / sizeof(lexemes[0]);
    printf(length);

	/*
    // Parser la liste des lexèmes et créer l'AST
    ASTNode *ast = parse(lexemes,lexemes);

    // Afficher l'AST
    printf("\nArbre Syntaxique :\n");
    printAST(ast, 0);

    // Libérer la mémoire de l'AST
    freeAST(ast);*/
    
}

int main() {
    test_synt("(p1⇒p2)→((¬p1)∨p2)");

    return 0;
}
