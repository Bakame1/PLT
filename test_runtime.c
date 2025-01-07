#include <stdio.h>
#include "runtime.c" // Inclure directement le runtime

// Fonction pour tester une expression logique avec la machine virtuelle
void test_runtime(const char *test_name, VMInstruction program[], int program_size) {
    printf("\n=== Test de la machine virtuelle : %s ===\n", test_name);

    // Charger le programme dans la machine virtuelle
    for (int i = 0; i < program_size; i++) {
        add_instruction(program[i].opcode, program[i].operand);
    }

    // Exécuter le programme
    execute_program();

    // Réinitialiser la machine virtuelle (vide le programme et la pile)
    program_counter = 0;
    stack_top = -1;
}

// Fonction principale pour exécuter les tests
int main() {
    printf("\n=== Tests de la machine virtuelle ===\n");

    // Test 1 : (1 AND 0) ⇒ (NOT 1 OR 1)
    VMInstruction test1[] = {
        {VM_PUSH, 1},   // Empiler 1 (vrai)
        {VM_PUSH, 0},   // Empiler 0 (faux)
        {VM_AND, 0},    // AND logique
        {VM_PUSH, 1},   // Empiler 1 (vrai)
        {VM_NOT, 0},    // NOT logique
        {VM_PUSH, 1},   // Empiler 1 (vrai)
        {VM_OR, 0},     // OR logique
        {VM_IMP, 0},    // IMPLIQUE logique
        {VM_PRINT, 0}   // Afficher le résultat
    };
    test_runtime("Test 1 : (1 AND 0) ⇒ (NOT 1 OR 1)", test1, sizeof(test1) / sizeof(test1[0]));

    // Test 2 : NOT (1 AND 1)
    VMInstruction test2[] = {
        {VM_PUSH, 1},   // Empiler 1 (vrai)
        {VM_PUSH, 1},   // Empiler 1 (vrai)
        {VM_AND, 0},    // AND logique
        {VM_NOT, 0},    // NOT logique
        {VM_PRINT, 0}   // Afficher le résultat
    };
    test_runtime("Test 2 : NOT (1 AND 1)", test2, sizeof(test2) / sizeof(test2[0]));

    // Test 3 : (0 OR 1) AND (NOT 0)
    VMInstruction test3[] = {
        {VM_PUSH, 0},   // Empiler 0 (faux)
        {VM_PUSH, 1},   // Empiler 1 (vrai)
        {VM_OR, 0},     // OR logique
        {VM_PUSH, 0},   // Empiler 0 (faux)
        {VM_NOT, 0},    // NOT logique
        {VM_AND, 0},    // AND logique
        {VM_PRINT, 0}   // Afficher le résultat
    };
    test_runtime("Test 3 : (0 OR 1) AND (NOT 0)", test3, sizeof(test3) / sizeof(test3[0]));

    // Test 4 : (1 AND (NOT 0)) OR (NOT 1)
    VMInstruction test4[] = {
        {VM_PUSH, 1},   // Empiler 1 (vrai)
        {VM_PUSH, 0},   // Empiler 0 (faux)
        {VM_NOT, 0},    // NOT logique
        {VM_AND, 0},    // AND logique
        {VM_PUSH, 1},   // Empiler 1 (vrai)
        {VM_NOT, 0},    // NOT logique
        {VM_OR, 0},     // OR logique
        {VM_PRINT, 0}   // Afficher le résultat
    };
    test_runtime("Test 4 : (1 AND (NOT 0)) OR (NOT 1)", test4, sizeof(test4) / sizeof(test4[0]));

    return 0;
}

