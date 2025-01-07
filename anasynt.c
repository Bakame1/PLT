#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

// Déclaration des différents types de nœuds
typedef enum {
    NODE_PROP,      // Proposition
    NODE_AND,       // Opérateur AND (ET)
    NODE_OR,        // Opérateur OR (OU)
    NODE_NOT,       // Opérateur NOT (NON)
    NODE_IMP,       // Opérateur IMPLIQUE (⇒)
    NODE_PROD       // Opérateur PRODUIT (→)
} NodeType;

// Structure d'un nœud de l'AST
typedef struct ASTNode {
    NodeType type;             // Type de nœud
    char *value;               // Valeur pour les propositions
    struct ASTNode *left;      // Enfant gauche
    struct ASTNode *right;     // Enfant droit
} ASTNode;

// Structure pour encapsuler les données de parsing
typedef struct {
    char **lexemes;            // Liste des lexèmes
    int current;               // Indice courant dans la liste des lexèmes
} ParserState;

// Fonction pour afficher une erreur et quitter
void error(const char *msg, int pos) {
    fprintf(stderr, "Erreur de syntaxe à la position %d: %s\n", pos, msg);
    exit(EXIT_FAILURE);
}

// Fonction pour créer un nœud proposition
ASTNode* createPropNode(const char *prop) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }
    node->type = NODE_PROP;
    node->value = strdup(prop); // Copier la valeur
    node->left = node->right = NULL;
    return node;
}

// Fonction pour créer un nœud opérateur
ASTNode* createOpNode(NodeType type, ASTNode *left, ASTNode *right) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->value = NULL; // Les opérateurs n'ont pas de valeur
    node->left = left;
    node->right = right;
    return node;
}

// Fonction pour obtenir le lexème actuel
const char* getCurrentLexeme(ParserState *state) {
    return state->lexemes[state->current];
}

// Fonction pour avancer au lexème suivant
void advance_lexeme(ParserState *state) {
    if (state->lexemes[state->current] != NULL) {
        state->current++;
    }
}

// ----------------------
// Fonctions de Parsing
// ----------------------

// Forward declarations
ASTNode* parse_expr(ParserState *state);
ASTNode* parse_implication(ParserState *state);
ASTNode* parse_or_expr(ParserState *state);
ASTNode* parse_and_expr(ParserState *state);
ASTNode* parse_not_expr(ParserState *state);
ASTNode* parse_primary(ParserState *state);

// expr ::= implication
ASTNode* parse_expr(ParserState *state) {
    return parse_implication(state);
}

// implication ::= or_expr ( "Op(IMPLIQUE)" implication | "Op(PRODUIT)" implication )?
ASTNode* parse_implication(ParserState *state) {
    ASTNode *left = parse_or_expr(state);

    while (state->lexemes[state->current] &&
           (strcmp(state->lexemes[state->current], "Op(IMPLIQUE)") == 0 ||
            strcmp(state->lexemes[state->current], "Op(PRODUIT)") == 0)) {
        if (strcmp(state->lexemes[state->current], "Op(IMPLIQUE)") == 0) {
            advance_lexeme(state);
            ASTNode *right = parse_implication(state); // Implication est associatif à droite
            left = createOpNode(NODE_IMP, left, right);
        } else if (strcmp(state->lexemes[state->current], "Op(PRODUIT)") == 0) {
            advance_lexeme(state);
            ASTNode *right = parse_implication(state); // Traitement similaire
            left = createOpNode(NODE_PROD, left, right);
        }
    }

    return left;
}

// or_expr ::= and_expr ( "Op(OU)" and_expr )*
ASTNode* parse_or_expr(ParserState *state) {
    ASTNode *left = parse_and_expr(state);

    while (state->lexemes[state->current] && strcmp(state->lexemes[state->current], "Op(OU)") == 0) {
        advance_lexeme(state);
        ASTNode *right = parse_and_expr(state);
        left = createOpNode(NODE_OR, left, right);
    }

    return left;
}

// and_expr ::= not_expr ( "Op(ET)" not_expr )*
ASTNode* parse_and_expr(ParserState *state) {
    ASTNode *left = parse_not_expr(state);

    while (state->lexemes[state->current] && strcmp(state->lexemes[state->current], "Op(ET)") == 0) {
        advance_lexeme(state);
        ASTNode *right = parse_not_expr(state);
        left = createOpNode(NODE_AND, left, right);
    }

    return left;
}

// not_expr ::= "Op(NON)" not_expr | primary
ASTNode* parse_not_expr(ParserState *state) {
    if (state->lexemes[state->current] && strcmp(state->lexemes[state->current], "Op(NON)") == 0) {
        advance_lexeme(state);
        ASTNode *operand = parse_not_expr(state);
        return createOpNode(NODE_NOT, NULL, operand);
    } else {
        return parse_primary(state);
    }
}

// primary ::= "Prop(x)" | "PO" expr "PF"
ASTNode* parse_primary(ParserState *state) {
    if (!state->lexemes[state->current]) {
        error("Expression inattendue à la fin de l'entrée", state->current);
    }

    if (strncmp(state->lexemes[state->current], "Prop(", 5) == 0) {
        // Extraire le nom de la proposition (par exemple, "p1" de "Prop(p1)")
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
        prop[len - 1] = '\0'; // Retirer la parenthèse fermante
        ASTNode *node = createPropNode(prop);
        free(prop);
        advance_lexeme(state);
        return node;
    } else if (strcmp(state->lexemes[state->current], "PO") == 0) {
        advance_lexeme(state);
        ASTNode *node = parse_expr(state);
        if (!state->lexemes[state->current] || strcmp(state->lexemes[state->current], "PF") != 0) {
            error("Parenthèse fermante manquante", state->current);
        }
        advance_lexeme(state);
        return node;
    } else {
        error("Proposition ou parenthèse ouvrante attendue", state->current);
        return NULL; // Inutile, mais évite les warnings
    }
}

// Fonction pour afficher l'AST (in-order traversal avec indentation)
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

    printAST(node->left, depth + 1);
    printAST(node->right, depth + 1);
}

// Fonction pour libérer l'AST
void freeAST(ASTNode *node) {
    if (node == NULL) return;
    freeAST(node->left);
    freeAST(node->right);
    if (node->value) free(node->value);
    free(node);
}

// Fonction globale pour tester l'analyse syntaxique
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
