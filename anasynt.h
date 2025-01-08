#ifndef ANASYNT_H
#define ANASYNT_H

//Strucuture des noeuds d'un arbre syntaxique
typedef enum {
    NODE_PROP,
    NODE_AND,
    NODE_OR,
    NODE_NOT,
    NODE_IMP,
    NODE_PROD
} NodeType;

//Structure de l'arbre syntaxique
typedef struct ASTNode {
    NodeType type;
    char *value;
    struct ASTNode *left;
    struct ASTNode *right;
} ASTNode;

//Analyse syntaxique
ASTNode* analyseur_syntaxique(char** lexemes);

//Fonction d'affichage des arbres syntaxiques
void printAST(ASTNode* node, int depth);

//Fonction pour libérer la mémoire allouée par un arbre syntaxique
void freeAST(ASTNode* node);

#endif

