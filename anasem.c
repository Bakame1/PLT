#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Définition des types de nSuds
typedef enum {
    NODE_PROP,      // Proposition
    NODE_AND,       // Opérateur AND (ET)
    NODE_OR,        // Opérateur OR (OU)
    NODE_NOT,       // Opérateur NOT (NON)
    NODE_IMP,       // Opérateur IMPLIQUE ()
    NODE_PROD       // Opérateur PRODUIT ()
} NodeType;

// Structure d'un nSud de l'AST
typedef struct ASTNode {
    NodeType type;             // Type de nSud
    char *value;               // Valeur pour les propositions
    struct ASTNode *left;      // Enfant gauche
    struct ASTNode *right;     // Enfant droit
} ASTNode;

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
    // Ajoutez d'autres propositions selon vos besoins
}

// Fonction pour libérer la mémoire des propositions valides
void free_valid_props_memory() {
    for (int i = 0; i < prop_count; i++) {
        free(valid_props[i]);
    }
}

// Fonction pour créer un nSud AST
ASTNode* create_ast_node(NodeType type, const char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("Erreur d'allocation mémoire pour ASTNode");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    if (value) {
        node->value = strdup(value);
        if (!node->value) {
            perror("Erreur d'allocation mémoire pour valeur de ASTNode");
            exit(EXIT_FAILURE);
        }
    } else {
        node->value = NULL;
    }
    node->left = node->right = NULL;
    return node;
}

// Fonction pour libérer l'AST
void free_ast(ASTNode *node) {
    if (node == NULL) return;
    free_ast(node->left);
    free_ast(node->right);
    if (node->value) free(node->value);
    free(node);
}

// Fonction pour lire l'AST à partir du fichier et reconstruire l'arbre
ASTNode* readAST(FILE *file, int current_depth, int *line_number) {
    char line[256];
    ASTNode *node = NULL;

    while (fgets(line, sizeof(line), file)) {
        (*line_number)++;
        // Calculer l'indentation (nombre de groupes de 4 espaces)
        int depth = 0;
        int i = 0;
        while (line[i] == ' ' && line[i+1] == ' ' && line[i+2] == ' ' && line[i+3] == ' ') {
            depth++;
            i += 4;
        }

        // Enlever le caractère de nouvelle ligne
        line[strcspn(line, "\n")] = '\0';

        // Si la profondeur est moins que current_depth, retourner
        if (depth < current_depth) {
            fseek(file, -strlen(line) - 1, SEEK_CUR); // Revenir en arrière
            (*line_number)--;
            break;
        }

        // Si la profondeur est plus grand que current_depth, c'est une erreur
        if (depth > current_depth) {
            fprintf(stderr, "Erreur sémantique: Indentation incorrecte à la ligne %d.\n", *line_number);
            exit(EXIT_FAILURE);
        }

        // Créer le nSud
        if (strncmp(line + i, "PROP ", 5) == 0) {
            char *prop = line + i + 5;
            node = create_ast_node(NODE_PROP, prop);
        }
        else if (strcmp(line + i, "AND") == 0) {
            node = create_ast_node(NODE_AND, NULL);
        }
        else if (strcmp(line + i, "OR") == 0) {
            node = create_ast_node(NODE_OR, NULL);
        }
        else if (strcmp(line + i, "NOT") == 0) {
            node = create_ast_node(NODE_NOT, NULL);
        }
        else if (strcmp(line + i, "IMPLIQUE") == 0) {
            node = create_ast_node(NODE_IMP, NULL);
        }
        else if (strcmp(line + i, "PRODUIT") == 0) {
            node = create_ast_node(NODE_PROD, NULL);
        }
        else {
            fprintf(stderr, "Erreur sémantique: Type de nSud inconnu '%s' à la ligne %d.\n", line + i, *line_number);
            exit(EXIT_FAILURE);
        }

        // Lire les enfants
        // Pour les opérateurs binaires, lire deux enfants
        if (node->type == NODE_AND || node->type == NODE_OR ||
            node->type == NODE_IMP || node->type == NODE_PROD) {
            node->left = readAST(file, current_depth + 1, line_number);
            node->right = readAST(file, current_depth + 1, line_number);
        }
        // Pour les opérateurs unaire, lire un enfant
        else if (node->type == NODE_NOT) {
            node->right = readAST(file, current_depth + 1, line_number);
        }

        return node;
    }

    return node;
}

// Fonction pour reconstruire l'AST depuis le fichier
ASTNode* reconstructAST(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier AST");
        exit(EXIT_FAILURE);
    }

    int line_number = 0;
    ASTNode *root = readAST(file, 0, &line_number);

    fclose(file);
    return root;
}

// Fonction pour effectuer l'analyse sémantique
void semantic_analysis(ASTNode *node) {
    if (node == NULL) return;

    // Effectuer l'analyse sémantique sur les enfants avant de traiter le nSud courant
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
            fprintf(stderr, "Erreur sémantique: Type de nSud inconnu.\n");
            exit(EXIT_FAILURE);
    }
}

int main() {
    // Initialiser les propositions valides
    initialize_valid_props();

    // Définir les fichiers AST pour les tests
    const char *valid_ast_file = "ast_valid.txt";
    const char *invalid_ast_file = "ast_invalid.txt";

    // Exemple d'AST Valide
    FILE *file_valid = fopen(valid_ast_file, "w");
    if (!file_valid) {
        perror("Erreur lors de la création du fichier AST valide");
        return EXIT_FAILURE;
    }
    fprintf(file_valid, "IMPLIQUE\n");
    fprintf(file_valid, "    PRODUIT\n");
    fprintf(file_valid, "        OR\n");
    fprintf(file_valid, "            PROP p1\n");
    fprintf(file_valid, "            PROP p2\n");
    fprintf(file_valid, "        OR\n");
    fprintf(file_valid, "            PROP p2\n");
    fprintf(file_valid, "            PROP p1\n");
    fclose(file_valid);

    // Exemple d'AST Invalide (proposition p3 non valide)
    FILE *file_invalid = fopen(invalid_ast_file, "w");
    if (!file_invalid) {
        perror("Erreur lors de la création du fichier AST invalide");
        return EXIT_FAILURE;
    }
    fprintf(file_invalid, "IMPLIQUE\n");
    fprintf(file_invalid, "    PRODUIT\n");
    fprintf(file_invalid, "        OR\n");
    fprintf(file_invalid, "            PROP p1\n");
    fprintf(file_invalid, "            PROP p3\n"); // p3 n'est pas valide
    fprintf(file_invalid, "        OR\n");
    fprintf(file_invalid, "            PROP p2\n");
    fprintf(file_invalid, "            PROP p1\n");
    fclose(file_invalid);

    // Tableau des fichiers AST à analyser
    const char *ast_files[] = { valid_ast_file, invalid_ast_file };
    int num_files = sizeof(ast_files) / sizeof(ast_files[0]);

    for (int i = 0; i < num_files; i++) {
        printf("\n--- Analyse du fichier AST : %s ---\n", ast_files[i]);
        ASTNode *ast = reconstructAST(ast_files[i]);

        // Effectuer l'analyse sémantique
        printf("Début de l'analyse sémantique...\n");
        semantic_analysis(ast);
        printf("Aucune erreur sémantique détectée.\n");

        // Libérer la mémoire de l'AST
        free_ast(ast);
    }

    // Libérer la mémoire des propositions valides
    free_valid_props_memory();

    // Supprimer les fichiers AST de test
    remove(valid_ast_file);
    remove(invalid_ast_file);

    return EXIT_SUCCESS;
}
