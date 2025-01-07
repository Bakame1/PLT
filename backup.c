#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

// Définition des types de nSuds
typedef enum {
    NODE_PROP,      // Proposition
    NODE_AND,       // Opérateur AND (ET)
    NODE_OR,        // Opérateur OR (OU)
    NODE_NOT,       // Opérateur NOT (NON)
    NODE_IMP,       // Opérateur IMPLIQUE (Ò)
    NODE_PROD       // Opérateur PRODUIT ()
} NodeType;

// Structure d'un nSud de l'AST
typedef struct ASTNode {
    NodeType type;             // Type de nSud
    char *value;               // Valeur pour les propositions
    struct ASTNode *left;      // Enfant gauche
    struct ASTNode *right;     // Enfant droit
} ASTNode;



// Fonction pour afficher une erreur et quitter
void error(const char *msg, int pos) {
    fprintf(stderr, "Erreur de syntaxe à la position %d: %s\n", pos, msg);
    exit(EXIT_FAILURE);
}

// Fonction pour créer un nSud proposition
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

// Fonction pour créer un nSud opérateur
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
const char* getCurrentLexeme() {
    return lexemes[current];
}

// Fonction pour avancer au lexème suivant
void advance_lexeme() {
    if (lexemes[current] != NULL) {
        current++;
    }
}

// ----------------------
// Fonctions de Parsing
// ----------------------

// Forward declarations
ASTNode* parse_expr();
ASTNode* parse_implication();
ASTNode* parse_or_expr();
ASTNode* parse_and_expr();
ASTNode* parse_not_expr();
ASTNode* parse_primary();

// expr ::= implication
ASTNode* parse_expr() {
    return parse_implication();
}

// implication ::= or_expr ( "Op(IMPLIQUE)" implication | "Op(PRODUIT)" implication )?
ASTNode* parse_implication() {
    ASTNode *left = parse_or_expr();

    while (lexemes[current] &&
          (strcmp(lexemes[current], "Op(IMPLIQUE)") == 0 || strcmp(lexemes[current], "Op(PRODUIT)") == 0)) {
        if (strcmp(lexemes[current], "Op(IMPLIQUE)") == 0) {
            advance_lexeme();
            ASTNode *right = parse_implication(); // Implication est associatif à droite
            left = createOpNode(NODE_IMP, left, right);
        }
        else if (strcmp(lexemes[current], "Op(PRODUIT)") == 0) {
            advance_lexeme();
            ASTNode *right = parse_implication(); // Traitement similaire
            left = createOpNode(NODE_PROD, left, right);
        }
    }

    return left;
}

// or_expr ::= and_expr ( "Op(OU)" and_expr )*
ASTNode* parse_or_expr() {
    ASTNode *left = parse_and_expr();

    while (lexemes[current] && strcmp(lexemes[current], "Op(OU)") == 0) {
        advance_lexeme();
        ASTNode *right = parse_and_expr();
        left = createOpNode(NODE_OR, left, right);
    }

    return left;
}

// and_expr ::= not_expr ( "Op(ET)" not_expr )*
ASTNode* parse_and_expr() {
    ASTNode *left = parse_not_expr();

    while (lexemes[current] && strcmp(lexemes[current], "Op(ET)") == 0) {
        advance_lexeme();
        ASTNode *right = parse_not_expr();
        left = createOpNode(NODE_AND, left, right);
    }

    return left;
}

// not_expr ::= "Op(NON)" not_expr | primary
ASTNode* parse_not_expr() {
    if (lexemes[current] && strcmp(lexemes[current], "Op(NON)") == 0) {
        printf("Found 'Op(NON)' at lexeme %d\n", current);
        advance_lexeme();
        ASTNode *operand = parse_not_expr();
        return createOpNode(NODE_NOT, NULL, operand);
    } else {
        return parse_primary();
    }
}

// primary ::= "Prop(x)" | "PO" expr "PF"
ASTNode* parse_primary() {
    if (!lexemes[current]) {
        error("Expression inattendue à la fin de l'entrée", current);
    }


    if (strncmp(lexemes[current], "Prop(", 5) == 0) {
        // Extraire le nom de la proposition (par exemple, "p1" de "Prop(p1)")
        char *prop = strdup(&lexemes[current][5]); // "Prop(p1)" -> "p1)"
        if (!prop) {
            perror("Erreur d'allocation mémoire pour prop");
            exit(EXIT_FAILURE);
        }
        size_t len = strlen(prop);
        if (len < 1 || prop[len - 1] != ')') {
            free(prop);
            error("Format de proposition invalide", current);
        }
        prop[len - 1] = '\0'; // Retirer la parenthèse fermante
        ASTNode *node = createPropNode(prop);
        free(prop);
        advance_lexeme();
        return node;
    } else if (strcmp(lexemes[current], "PO") == 0) {
        // Parenthèse ouvrante
        advance_lexeme();
        ASTNode *node = parse_expr();
        if (!lexemes[current] || strcmp(lexemes[current], "PF") != 0) {
            error("Parenthèse fermante manquante", current);
        }
        advance_lexeme();
        return node;
    } else {
        error("Proposition ou parenthèse ouvrante attendue", current);
        return NULL; // Inutile, mais évite les warnings
    }
}

// Fonction principale du parser
ASTNode* parse() {
    ASTNode *root = parse_expr();

    if (lexemes[current] != NULL) {
        error("Lexème inattendu après la fin de l'expression", current);
    }

    return root;
}

// ----------------------
// Fonctions d'Affichage et de Libération de l'AST
// ----------------------

// Fonction pour afficher l'AST (in-order traversal avec indentation)
void printAST(ASTNode *node, int depth) {
    if (node == NULL) return;

    // Indentation basée sur la profondeur
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    // Afficher le type de nSud
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

    // Afficher les enfants
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

