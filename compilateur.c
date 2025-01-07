// compilateur.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Définition des types de nSuds (répétition pour cohérence avec anasem.c)
typedef enum {
    NODE_PROP,      // Proposition
    NODE_AND,       // Opérateur AND (ET)
    NODE_OR,        // Opérateur OR (OU)
    NODE_NOT,       // Opérateur NOT (NON)
    NODE_IMP,       // Opérateur IMPLIQUE ()
    NODE_PROD       // Opérateur PRODUIT ()
} NodeType;

// Structure d'un nSud de l'AST (répétition pour cohérence avec anasem.c)
typedef struct ASTNode {
    NodeType type;             // Type de nSud
    char *value;               // Valeur pour les propositions
    struct ASTNode *left;      // Enfant gauche
    struct ASTNode *right;     // Enfant droit
} ASTNode;

// Structure pour une entrée de la table des symboles
typedef struct {
    char name[50];                             // Nom de l'opérateur (e.g., "ET", "NON")
    int length;                                // Longueur du nom
    int ascii_codes[10];                       // Codes ASCII des caractères du nom
    int num_params;                            // Nombre de paramètres requis
    void (*func)(ASTNode *, ASTNode *);        // Pointeur vers la fonction correspondante
} SymbolTableEntry;

// Définir une table des symboles avec une capacité maximale
#define SYMBOL_TABLE_SIZE 100
SymbolTableEntry symbol_table[SYMBOL_TABLE_SIZE];
int symbol_table_count = 0;

// Définir la machine virtuelle
#define VM_SIZE 100
int virtual_machine[VM_SIZE];
int vm_count = 0;

// Prototypes des fonctions des opérateurs
void Et(ASTNode *left, ASTNode *right);
void Non(ASTNode *operand, ASTNode *unused);
void Ou(ASTNode *left, ASTNode *right);
void Implique(ASTNode *left, ASTNode *right);

// Prototypes des fonctions d'initialisation
void add_symbol(const char *name, int num_params, void (*func)(ASTNode *, ASTNode *));
void initialize_symbol_table();
void initialize_virtual_machine();

// Implémentation des opérateurs logiques

// Opérateur ET
void Et(ASTNode *left, ASTNode *right) {
    printf("Effectuer ET entre '%s' et '%s'\n", left->value, right->value);
    // Implémentation de la logique ET (à compléter selon les besoins)
}

// Opérateur NON
void Non(ASTNode *operand, ASTNode *unused) {
    printf("Effectuer NON sur '%s'\n", operand->value);
    // Implémentation de la logique NON (à compléter selon les besoins)
}

// Opérateur OU
void Ou(ASTNode *left, ASTNode *right) {
    printf("Effectuer OU entre '%s' et '%s'\n", left->value, right->value);
    // Implémentation de la logique OU (à compléter selon les besoins)
}

// Opérateur IMPLIQUE
void Implique(ASTNode *left, ASTNode *right) {
    printf("Effectuer IMPLIQUE entre '%s' et '%s'\n", left->value, right->value);
    // Implémentation de la logique IMPLIQUE (à compléter selon les besoins)
}

// Fonction pour ajouter une entrée à la table des symboles
void add_symbol(const char *name, int num_params, void (*func)(ASTNode *, ASTNode *)) {
    if (symbol_table_count >= SYMBOL_TABLE_SIZE) {
        fprintf(stderr, "Erreur: La table des symboles est pleine.\n");
        exit(EXIT_FAILURE);
    }

    strncpy(symbol_table[symbol_table_count].name, name, sizeof(symbol_table[symbol_table_count].name) - 1);
    symbol_table[symbol_table_count].name[sizeof(symbol_table[symbol_table_count].name) - 1] = '\0';

    symbol_table[symbol_table_count].length = strlen(name);

    for (int i = 0; i < symbol_table[symbol_table_count].length && i < 10; i++) {
        symbol_table[symbol_table_count].ascii_codes[i] = (int)name[i];
    }
    // Remplir le reste avec 0 si nécessaire
    for (int i = symbol_table[symbol_table_count].length; i < 10; i++) {
        symbol_table[symbol_table_count].ascii_codes[i] = 0;
    }

    symbol_table[symbol_table_count].num_params = num_params;
    symbol_table[symbol_table_count].func = func;

    symbol_table_count++;
}

// Fonction pour initialiser la table des symboles avec les opérateurs logiques
void initialize_symbol_table() {
    add_symbol("ET", 2, Et);
    add_symbol("NON", 1, Non);
    add_symbol("OU", 2, Ou);
    add_symbol("IMPLIQUE", 2, Implique);
    // Ajoutez d'autres opérateurs si nécessaire
}

// Fonction pour initialiser la machine virtuelle
void initialize_virtual_machine() {
    // Exemple de codes CFA pour les opérateurs (à ajuster selon les besoins)
    // ET -> 0, NON -> 1, OU -> 2, IMPLIQUE -> 3
    virtual_machine[0] = 0; // ET
    virtual_machine[1] = 1; // NON
    virtual_machine[2] = 2; // OU
    virtual_machine[3] = 3; // IMPLIQUE

    vm_count = 4; // Mise à jour du compteur
}

// Fonction pour trouver un symbole dans la table des symboles
int find_symbol(const char *name) {
    for (int i = 0; i < symbol_table_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            return i;
        }
    }
    return -1; // Symbole non trouvé
}
