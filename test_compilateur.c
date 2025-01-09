#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include "analex.c"     //Pour CreationListeLexeme
#include "anasynt.c"    //Pour ASTNode, analyseur_syntaxique, printAST, freeAST
#include "anasem.c"     //Pour analyseur_semantique (ou semantic_analysis),
                        //initialize_valid_props, free_valid_props_memory

int main() {

    initialize_valid_props();
    setlocale(LC_ALL, "");

    //Saisie dans la console 
    char input[256];
    printf("Entrez une proposition logique :\n");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        fprintf(stderr, "Erreur de lecture.\n");
        return EXIT_FAILURE;
    }
    input[strcspn(input, "\n")] = '\0';

    //Analyse lexicale
    char** lexemes = CreationListeLexeme(input);
    if (!lexemes) {
        fprintf(stderr, "Erreur : L'analyse lexicale a échoué.\n");
        return EXIT_FAILURE;
    }

    //Comptage des lexemes
    int lexeme_count = 0;
    while (lexemes[lexeme_count]) {
        lexeme_count++;
    }

    //Analyse syntaxique
    ASTNode* ast = analyseur_syntaxique(lexemes);
    if (!ast) {
        fprintf(stderr, "Erreur : L'analyse syntaxique a échoué.\n");
        return EXIT_FAILURE;
    }

    //On affiche l'AST si l'analyse a réussie
    printAST(ast, 0);

    //Analyse sémantique
    analyseur_semantique(ast);
    
    return EXIT_SUCCESS;
}
