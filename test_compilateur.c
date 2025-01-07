#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compilateur.c"
#include "analex.c"
#include "anasynt.c"
#include "anasem.c"

// Prototypes des fonctions provenant des autres fichiers
extern char** CreationListeLexeme (const char* input);
extern ASTNode* analyseur_syntaxique(char** lexemes, int lexeme_count);
extern void semantic_analysis(ASTNode* ast);
extern void compiler_proposition(ASTNode* ast);

// Fonction pour libérer la mémoire allouée pour les lexèmes
void free_lexemes(char** lexemes) {
    if (lexemes) {
        for (int i = 0; lexemes[i] != NULL; i++) {
            free(lexemes[i]);
        }
        free(lexemes);
    }
}

int main() {
    char input[256];
    
    // Saisie de l'utilisateur
    printf("Entrez une proposition logique à compiler (ex: (p1 ∧ p2) ⇒ ¬p3):\n");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        fprintf(stderr, "Erreur de lecture de l'entrée.\n");
        return EXIT_FAILURE;
    }

    // Supprimer le caractère de nouvelle ligne si présent
    input[strcspn(input, "\n")] = '\0';

    // Étape 1 : Analyse lexicale
    printf("\n=== Étape 1 : Analyse lexicale ===\n");
    char** lexemes = analyseur_lexical(input);
    if (!lexemes) {
        fprintf(stderr, "Erreur : L'analyse lexicale a échoué.\n");
        return EXIT_FAILURE;
    }

    printf("Liste des lexèmes obtenus :\n");
    for (int i = 0; lexemes[i] != NULL; i++) {
        printf("%s ", lexemes[i]);
    }
    printf("\n");

    // Étape 2 : Analyse syntaxique
    printf("\n=== Étape 2 : Analyse syntaxique ===\n");
    int lexeme_count = 0;
    while (lexemes[lexeme_count] != NULL) {
        lexeme_count++;
    }

    ASTNode* ast = analyseur_syntaxique(lexemes, lexeme_count);
    if (!ast) {
        fprintf(stderr, "Erreur : L'analyse syntaxique a échoué.\n");
        free_lexemes(lexemes);
        return EXIT_FAILURE;
    }

    printf("Arbre syntaxique généré avec succès.\n");

    // Étape 3 : Analyse sémantique
    printf("\n=== Étape 3 : Analyse sémantique ===\n");
    analyseur_semantique(ast);
    printf("Analyse sémantique terminée avec succès.\n");

    // Étape 4 : Compilation
    printf("\n=== Étape 4 : Compilation dans la machine virtuelle ===\n");
    compiler_proposition(ast);
    printf("Compilation terminée avec succès.\n");

    // Libération de la mémoire
    free_lexemes(lexemes);

    return EXIT_SUCCESS;
}
