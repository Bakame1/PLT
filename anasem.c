#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "anasem.h"


//On initialise une liste des propositions valides avec une taille maximale
#define MAX_PROPS 100
char* valid_props[MAX_PROPS];
int prop_count = 0;

//Fonction permettant d'ajouter une proposition valide à liste des propositions valides
//Parametre prop : chaine de caracteres
//Ajoute une proposition valide dans la liste de propositions valides s'il y a assez de place
void add_valid_prop(const char *prop) {
    //Verifier si le nombre max de propositions est atteint
    if (prop_count >= MAX_PROPS) {
        fprintf(stderr, "Erreur: Trop de propositions valides.\n");
        exit(EXIT_FAILURE);
    }
    valid_props[prop_count++] = strdup(prop);
}

//Fonction pour vérifier si une proposition est valide
//Parametre prop : une chaine de caracteres 
//Retourne 1 si la proposition est valide et 0 sinon
int is_valid_prop(const char *prop) {
    for (int i = 0; i < prop_count; i++) {
        if (strcmp(valid_props[i], prop) == 0) {
            return 1;
        }
    }
    return 0;
}

//Fonction pour initialiser les propositions valides
//Ajoute à la liste des propositions valides les propositions considérées comme valides
void initialize_valid_props() {
    add_valid_prop("p1");
    add_valid_prop("p2");
    add_valid_prop("p3");
}

//Fonction pour libérer la mémoire des propositions valides
//Libère la mémoire de chaque proposition
void free_valid_props_memory() {
    for (int i = 0; i < prop_count; i++) {
        free(valid_props[i]);
    }
    prop_count = 0;
}

//Fonction globale permettant de réaliser l'analyse sémantique
//Parametre node : ASTNode (arbre syntaxique)
void analyseur_semantique(ASTNode *node) {
    //Si le noeud est vide
    if (node == NULL) return;

    //On effetue l'analyse sémantique sur les enfants avant de traiter le noeud courant
    analyseur_semantique(node->left);
    analyseur_semantique(node->right);

    //Selon le type de noeud 
    //Seuls Prop(""),Op("ET"),Op("OU"),Op("IMPLIQUE") et Op("PRODUIT") sont des noeuds
    switch (node->type) {
        case NODE_PROP:
            //On vérifie si la proposition est valide
            if (!is_valid_prop(node->value)) {
                fprintf(stderr, "Erreur sémantique: Proposition invalide '%s'.\n", node->value);
                exit(EXIT_FAILURE);
            }
            break;
        case NODE_AND:
        case NODE_OR:
        case NODE_IMP:
        case NODE_PROD:
            //On vérifie que les opérateurs ont des opérandes non vides
            if (node->left == NULL || node->right == NULL) {
                fprintf(stderr, "Erreur sémantique: L'opérateur '%s' doit avoir deux opérandes.\n",
                        (node->type == NODE_AND) ? "AND" :
                        (node->type == NODE_OR) ? "OR" :
                        (node->type == NODE_IMP) ? "IMPLIQUE" : "PRODUIT");
                exit(EXIT_FAILURE);
            }
            break;
        case NODE_NOT:
            //On vérifie que l'opérateur NOT a un opérande non vide
            if (node->right == NULL) {
                fprintf(stderr, "Erreur sémantique: L'opérateur 'NOT' doit avoir un opérande.\n");
                exit(EXIT_FAILURE);
            }
            break;
        default:
            fprintf(stderr, "Erreur sémantique: Type de noeud inconnu.\n");
            exit(EXIT_FAILURE);
    }
}

//Fonction pour libérer la mémoire allouée à l'arbre syntaxique
//Libère la mémoire de chaque noeud
void free_ast(ASTNode *node) {
    if (node == NULL) return;
    free_ast(node->left);
    free_ast(node->right);
    if (node->value) free(node->value);
    free(node);
}
