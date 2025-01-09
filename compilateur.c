#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compilateur.h"

//Structure d'une entrée dans la table des symboles
typedef struct {
    char name[50];                             //Nom de l'opérateur (exemple: "ET", "NON","OU"...)
    int length;                                //Longueur du nom
    int ascii_codes[10];                       //Codes ASCII des caractères du nom
    int num_params;                            //Nombre de paramètres requis
    void (*func)(ASTNode *, ASTNode *);        //Pointeur vers la fonction correspondante
} SymbolTableEntry;

//On définit une table des symboles avec une capacité maximale
#define SYMBOL_TABLE_SIZE 100
SymbolTableEntry symbol_table[SYMBOL_TABLE_SIZE];
int symbol_table_count = 0;

//On défini une machine virtuelle avec une capacité maximale
#define VM_SIZE 100
int virtual_machine[VM_SIZE];
int vm_count = 0;


//
//Implémentation des opérateurs logiques
//

//Opérateur ET
void Et(ASTNode *left, ASTNode *right) {
    printf("Effectuer ET entre '%s' et '%s'\n", left->value, right->value);
}

//Opérateur NON
void Non(ASTNode *operand, ASTNode *unused) {
    printf("Effectuer NON sur '%s'\n", operand->value);
}

//Opérateur OU
void Ou(ASTNode *left, ASTNode *right) {
    printf("Effectuer OU entre '%s' et '%s'\n", left->value, right->value);
}

//Opérateur IMPLIQUE
void Implique(ASTNode *left, ASTNode *right) {
    printf("Effectuer IMPLIQUE entre '%s' et '%s'\n", left->value, right->value);
}

//Fonction pour ajouter une entrée à la table des symboles
//Parametre name : chaine de caracteres
//Parametre num_params : entier
//Parametre func : fonction void
void add_symbol(const char *name, int num_params, void (*func)(ASTNode *, ASTNode *)) {

    //Vérification de la capacité de la table des symboles
    if (symbol_table_count >= SYMBOL_TABLE_SIZE) {
        fprintf(stderr, "Erreur: La table des symboles est pleine.\n");
        exit(EXIT_FAILURE);
    }

    //Copie du nom du symbole
    strncpy(symbol_table[symbol_table_count].name, name, sizeof(symbol_table[symbol_table_count].name) - 1);
    symbol_table[symbol_table_count].name[sizeof(symbol_table[symbol_table_count].name) - 1] = '\0';

    //Enregistrement de la longueur du nom
    symbol_table[symbol_table_count].length = strlen(name);

    //Stockage des codes ASCII
    for (int i = 0; i < symbol_table[symbol_table_count].length && i < 10; i++) {
        symbol_table[symbol_table_count].ascii_codes[i] = (int)name[i];
    }
    for (int i = symbol_table[symbol_table_count].length; i < 10; i++) {
        symbol_table[symbol_table_count].ascii_codes[i] = 0;
    }

    //Enregistrement du nombre de paramètres et de la fonction
    symbol_table[symbol_table_count].num_params = num_params;
    symbol_table[symbol_table_count].func = func;

    //Compteur
    symbol_table_count++;
}

// Fonction pour initialiser la table des symboles avec les opérateurs logiques
void initialize_symbol_table() {
    add_symbol("ET", 2, Et);
    add_symbol("NON", 1, Non);
    add_symbol("OU", 2, Ou);
    add_symbol("IMPLIQUE", 2, Implique);
}

//Fonction pour initialiser la machine virtuelle
void initialize_virtual_machine() {
    //Codes CFA utilisés :
    //ET -> 0, NON -> 1, OU -> 2, IMPLIQUE -> 3
    virtual_machine[0] = 0; //ET
    virtual_machine[1] = 1; //NON
    virtual_machine[2] = 2; //OU
    virtual_machine[3] = 3; //IMPLIQUE

    vm_count = 4; //Compteur
}

//Fonction pour trouver un symbole dans la table des symboles en la parcourant
//Parametre name : pointeur vers une chaîne de caractères
//Retourne un entier qui indique le résultat de la recherche,
//Si le symbole est trouvé, la fonction retourne l'index du symbole dans la table des symboles.
//Sinon elle retourne -1
int find_symbol(const char *name) {
    for (int i = 0; i < symbol_table_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            return i;//Symbole trouvé
        }
    }
    return -1; //Symbole non trouvé
}
