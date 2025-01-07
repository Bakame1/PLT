#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Liste des propositions valides
#define MAX_PROPS 100
char* valid_props[MAX_PROPS];
int prop_count = 0;

// Fonction pour ajouter une proposition valide
void add_valid_prop(const char *prop) {
    if (prop_count >= MAX_PROPS) {
        fprintf(stderr, "Erreur: Trop de propositions valides.\n");
        exit(EXIT_FAILURE);
    }
    valid_props[prop_count++] = strdup(prop);
}

// Fonction pour vérifier si une proposition est valide
int is_valid_prop(const char *prop) {
    for (int i = 0; i < prop_count; i++) {
        if (strcmp(valid_props[i], prop) == 0) {
            return 1;
        }
    }
    return 0;
}

// Fonction pour initialiser les propositions valides
void initialize_valid_props() {
    add_valid_prop("p1");
    add_valid_prop("p2");
    add_valid_prop("p3");
    // Ajoutez d'autres propositions valides ici
}

// Fonction pour libérer la mémoire des propositions valides
void free_valid_props_memory() {
    for (int i = 0; i < prop_count; i++) {
        free(valid_props[i]);
    }
    prop_count = 0;
}

// Fonction pour effectuer l'analyse sémantique
void semantic_analysis(ASTNode *node) {
    if (node == NULL) return;

    // Effectuer l'analyse sémantique sur les enfants avant de traiter le nœud courant
    semantic_analysis(node->left);
    semantic_analysis(node->right);

    switch (node->type) {
        case NODE_PROP:
            // Vérifier si la proposition est valide
            if (!is_valid_prop(node->value)) {
                fprintf(stderr, "Erreur sémantique: Proposition invalide '%s'.\n", node->value);
                exit(EXIT_FAILURE);
            }
            break;
        case NODE_AND:
        case NODE_OR:
        case NODE_IMP:
        case NODE_PROD:
            // Vérifier que les opérateurs ont des opérandes valides (non NULL)
            if (node->left == NULL || node->right == NULL) {
                fprintf(stderr, "Erreur sémantique: L'opérateur '%s' doit avoir deux opérandes.\n",
                        (node->type == NODE_AND) ? "AND" :
                        (node->type == NODE_OR) ? "OR" :
                        (node->type == NODE_IMP) ? "IMPLIQUE" : "PRODUIT");
                exit(EXIT_FAILURE);
            }
            break;
        case NODE_NOT:
            // Vérifier que l'opérateur NOT a un opérande valide (non NULL)
            if (node->right == NULL) {
                fprintf(stderr, "Erreur sémantique: L'opérateur 'NOT' doit avoir un opérande.\n");
                exit(EXIT_FAILURE);
            }
            break;
        default:
            fprintf(stderr, "Erreur sémantique: Type de nœud inconnu.\n");
            exit(EXIT_FAILURE);
    }
}

// Fonction pour libérer l'AST
void free_ast(ASTNode *node) {
    if (node == NULL) return;
    free_ast(node->left);
    free_ast(node->right);
    if (node->value) free(node->value);
    free(node);
}
