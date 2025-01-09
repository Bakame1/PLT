#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include "anasynt.h"


//Structure utilisée pour encapsuler l'état du parseur lors de l'analyse syntaxique
//essentiel pour suivre l'état du parseur et pour coordonner l'analyse syntaxique des lexèmes
typedef struct {
    char **lexemes;         //lexèmes générés par l'analyse lexicale
    int current;            //indice du lexème en cours
} ParserState;

//Fonction pour afficher les erreurs de syntaxes
//Parametre msg : une chaine de caracteres
//Parametre pos : un entier
//On affiche un message d'erreur de syntaxe et la position de l'erreur et on quitte le programme
void error(const char *msg, int pos) {
    fprintf(stderr, "Erreur de syntaxe à la position %d: %s\n", pos, msg);
    exit(EXIT_FAILURE);
}


//Fonction de creation d'un noeud proposition
//Parametre prop : chaine de caracteres
ASTNode* createPropNode(const char *prop) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }
    node->type = NODE_PROP;
    node->value = strdup(prop); // On copie la valeur de la proposition
    node->left = node->right = NULL; // Cela veut dire que qu'il n'y a pas d'enfants pour une proposition
    return node;
}

//Fonction de creation d'un noeud opérateur
//Parametre type : NodeType
//Parametre left : ASTNode (enfant gauche)
//Parametre right: ASTNode (enfant droit)
ASTNode* createOpNode(NodeType type, ASTNode *left, ASTNode *right) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->value = NULL;
    node->left = left;  
    node->right = right; 
    return node;
}

//Pour avancer au lexème suivant et on s'assure qu'on ne dépasse pas la fin de la liste des lexèmes
//Parametre state : ParserState
void advance_lexeme(ParserState *state) {
    if (state->lexemes[state->current] != NULL) {
        state->current++;
    }
}

// ----------------------
/*
Fonctions de Parsing :
   Ici notre objectif est de prendre la séquence des lexèmes et déterminer si elle suit les règles
   de telle sorte qu'on peut après construire notre arbre à la fin
*/
// ----------------------

//on commence par référencer les fonctions de parsing :
ASTNode* parse_expr(ParserState *state);
ASTNode* parse_implication(ParserState *state);
ASTNode* parse_or_expr(ParserState *state);
ASTNode* parse_and_expr(ParserState *state);
ASTNode* parse_not_expr(ParserState *state);
ASTNode* parse_primary(ParserState *state);

//La règle de grammaire BNF : expr ::= implication
//Parametre state : ParserState
ASTNode* parse_expr(ParserState *state) {
    return parse_implication(state);
}

/*
 Fonction : parse_implication
 Ici, on veut gérer l'associativité à droite des opérateurs d'implication
 On parse donc les opérateurs IMPLIQUE (⇒) et PRODUIT (→).
 La règle de grammaire correspondante est : implication ::= or_expr ( "Op(IMPLIQUE)" implication | "Op(PRODUIT)" implication )?
*/
//Parametre state : ParserState
ASTNode* parse_implication(ParserState *state) {
    ASTNode *left = parse_or_expr(state);
    
    //Gestion des implications consécutives :
    while (state->lexemes[state->current] &&
           (strcmp(state->lexemes[state->current], "Op(IMPLIQUE)") == 0 ||
            strcmp(state->lexemes[state->current], "Op(PRODUIT)") == 0)) {
        if (strcmp(state->lexemes[state->current], "Op(IMPLIQUE)") == 0) {
            advance_lexeme(state);
            ASTNode *right = parse_implication(state); //L'implication est associative à droite
            left = createOpNode(NODE_IMP, left, right);
        } else if (strcmp(state->lexemes[state->current], "Op(PRODUIT)") == 0) {
            advance_lexeme(state);
            ASTNode *right = parse_implication(state); //De meme pour le Produit
            left = createOpNode(NODE_PROD, left, right);
        }
    }

    return left;
}

/*
 Fonction : parse_or_exp
 Ici, on veut gérer l'associativité à gauche pour l'opérateur OR
 La règle de grammaire correspondante est : or_expr ::= and_expr ( "Op(OU)" and_expr )
*/
//Parametre state : ParserState
ASTNode* parse_or_expr(ParserState *state) {
    ASTNode *left = parse_and_expr(state);

    while (state->lexemes[state->current] && strcmp(state->lexemes[state->current], "Op(OU)") == 0) {
        advance_lexeme(state);
        ASTNode *right = parse_and_expr(state);
        left = createOpNode(NODE_OR, left, right);
    }

    return left;
}

/*
 Fonction : parse_and_exp
 Ici, on veut gérer l'associativité à gauche pour l'opérateur AND
 La règle de grammaire correspondante est : and_expr ::= not_expr ( "Op(ET)" not_expr )
 */
//Parametre state : ParserState
ASTNode* parse_and_expr(ParserState *state) {
    ASTNode *left = parse_not_expr(state);

    while (state->lexemes[state->current] && strcmp(state->lexemes[state->current], "Op(ET)") == 0) {
        advance_lexeme(state);
        ASTNode *right = parse_not_expr(state);
        left = createOpNode(NODE_AND, left, right);
    }

    return left;
}

/*
 Fonction : parse_not_expr
 Ici, on veut gérer la négation (opérateur NOT) de manière récursive
 La règle de grammaire correspondante est : not_expr ::= "Op(NON)" not_expr | primary
 */
//Parametre state : ParserState
ASTNode* parse_not_expr(ParserState *state) {
    if (state->lexemes[state->current] && strcmp(state->lexemes[state->current], "Op(NON)") == 0) {
        advance_lexeme(state);
        ASTNode *operand = parse_not_expr(state);
        return createOpNode(NODE_NOT, NULL, operand);
    } else {
        return parse_primary(state);
    }
}

/*
 Fonction : parse_primary
 Ici, on va créer des noeuds propopsitions et on va gérer les expressions entre parenthèses
 La règle de grammaire correspondante est : primary ::= "Prop(x)" | "PO" expr "PF"
 */
//Parametre state : ParserState
ASTNode* parse_primary(ParserState *state) {
    if (!state->lexemes[state->current]) {
        error("Expression inattendue à la fin de l'entrée", state->current);
    }
    //On vérifie si le lexème actuel est une proposition
    if (strncmp(state->lexemes[state->current], "Prop(", 5) == 0) {
        //On extrait le nom de la proposition (par exemple, "p1" de "Prop(p1)")
        char *prop = strdup(&state->lexemes[state->current][5]); // "Prop(p1)" -> "p1)"
        if (!prop) {
            perror("Erreur d'allocation mémoire pour prop");
            exit(EXIT_FAILURE);
        }
        size_t len = strlen(prop);
        if (len < 1 || prop[len - 1] != ')') {
            free(prop);
            error("Format de proposition invalide", state->current);
        }
        prop[len - 1] = '\0'; //on retire la parenthèse fermante
        ASTNode *node = createPropNode(prop);
        free(prop);
        advance_lexeme(state); //après avoir traité la proposition on passe au lexème suivant
        return node;

        //On vérifie si le lexème actuel est une parenthèse ouvrante
    } else if (strcmp(state->lexemes[state->current], "PO") == 0) {
        advance_lexeme(state);
        ASTNode *node = parse_expr(state); // on parse l'expression entre parenthèses
        //On vérifie si on a atteint la parenthèse fermante
        if (!state->lexemes[state->current] || strcmp(state->lexemes[state->current], "PF") != 0) {
            error("Parenthèse fermante manquante", state->current);
        }
        advance_lexeme(state); //Après la parenthèse fermante, on passe au lexème suivant
        return node;
    } else {
        error("Proposition ou parenthèse ouvrante attendue", state->current);
        return NULL; //Pour éviter les warnings de compilateur.
    }
}

//A la fin, on veut afficher l'arbre syntaxique. On utilisera ici un parcours prefixe pour l'affichage des noeuds.
//Parametre node : ASTNode
//Parametre depth : entier
void printAST(ASTNode *node, int depth) {
    if (node == NULL) return;

    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    switch (node->type) {
        case NODE_PROP:
            printf("Prop(%s)\n", node->value);
            break;
        case NODE_AND:
            printf("AND\n");
            break;
        case NODE_OR:
            printf("OR\n");
            break;
        case NODE_NOT:
            printf("NOT\n");
            break;
        case NODE_IMP:
            printf("IMPLIQUE\n");
            break;
        case NODE_PROD:
            printf("PRODUIT\n");
            break;
        default:
            printf("UNKNOWN NODE\n");
    }

    //On affiche récursivement les enfants gauche et droit
    printAST(node->left, depth + 1);
    printAST(node->right, depth + 1);
}

//On libère la mémoire allouée pour l'arbre
//Parametre node : ASTNode
void freeAST(ASTNode *node) {
    if (node == NULL) return;
    freeAST(node->left);
    freeAST(node->right);
    if (node->value) free(node->value);
    free(node);
}

//Fonction globale pour tester l'analyse syntaxique
//Parametre lexemes : liste de chaines de caracteres
//Retourne un arbre syntaxique si l'analyse réussit, sinon une erreur
ASTNode* analyseur_syntaxique(char** lexemes) {
    ParserState state;
    state.lexemes = lexemes;
    state.current = 0;

    ASTNode *ast = parse_expr(&state);

    if (state.lexemes[state.current] != NULL) {
        error("Lexème inattendu après la fin de l'expression", state.current);
    }

    return ast;
}

