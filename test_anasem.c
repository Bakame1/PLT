#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "analex.c"
#include "anasynt.c"
#include "anasem.c"

// Fonction de test pour l'analyse sémantique
void test_semantic_analysis(const char *expression) {
    printf("\n=== Test de l'analyse sémantique pour l'expression : %s ===\n", expression);

    // Étape 1 : Analyse lexicale
    char** lexemes = CreationListeLexeme(expression);
    if (!lexemes) {
        fprintf(stderr, "Erreur : Analyse lexicale échouée pour l'expression : %s\n", expression);
        return;
    }

    // Étape 2 : Analyse syntaxique
    ASTNode* ast = analyseur_syntaxique(lexemes);
    if (!ast) {
        fprintf(stderr, "Erreur : Analyse syntaxique échouée pour l'expression : %s\n", expression);
        return;
    }

    // Afficher l'AST avant l'analyse sémantique
    printf("AST avant l'analyse sémantique :\n");
    printAST(ast, 0);

    // Étape 3 : Analyse sémantique
    printf("\nRésultat de l'analyse sémantique :\n");
    semantic_analysis(ast);

    printf("Analyse sémantique réussie pour l'expression : %s\n", expression);

    // Libération de la mémoire
    freeAST(ast);
}

// Fonction principale pour exécuter les tests
int main() {
    // Initialiser la gestion des caractères UTF-8
    setlocale(LC_ALL, "");

    // Initialiser les propositions valides
    initialize_valid_props();

    // Tests avec des expressions valides
    test_semantic_analysis("(p1⇒p2)→((¬p1)∨p2)");
    test_semantic_analysis("(p1∨(p2∧p3))→((p1∨p2)∧(p1∨p3))");
    test_semantic_analysis("(p1∧p2)→(p2∧p1)");
    test_semantic_analysis("(¬(¬p1))→p1");

    // Tests avec des expressions invalides
    test_semantic_analysis("(p4∧p1)"); // Proposition invalide : p4
    test_semantic_analysis("(p1∧)");   // Opérateur AND sans opérande droit
    test_semantic_analysis("¬");       // Opérateur NOT sans opérande
    test_semantic_analysis("(p1⇒)");   // Opérateur IMPLIQUE sans opérande droit
    test_semantic_analysis("(∧p1)");   // Opérateur AND sans opérande gauche

    // Libérer la mémoire des propositions valides
    free_valid_props_memory();

    return 0;
}
