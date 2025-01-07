#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Définition des types de codes de la machine virtuelle
typedef enum {
    VM_NOP,    // Pas d'opération
    VM_PUSH,   // Empile une valeur sur la pile
    VM_POP,    // Retire une valeur de la pile
    VM_AND,    // Opération AND (ET logique)
    VM_OR,     // Opération OR (OU logique)
    VM_NOT,    // Opération NOT (NON logique)
    VM_IMP,    // Opération IMPLIQUE
    VM_PRINT   // Affiche une valeur
} VMOpcode;

// Structure d'une instruction de la machine virtuelle
typedef struct {
    VMOpcode opcode;   // Opération à effectuer
    int operand;       // Opérande associé (si nécessaire)
} VMInstruction;

// Définition de la pile de la machine virtuelle
#define STACK_SIZE 256
int vm_stack[STACK_SIZE];
int stack_top = -1;

// Programme de la machine virtuelle (instructions)
#define PROGRAM_SIZE 1024
VMInstruction program[PROGRAM_SIZE];
int program_counter = 0; // Compteur d'instructions

// Ajouter une instruction au programme
void add_instruction(VMOpcode opcode, int operand) {
    if (program_counter >= PROGRAM_SIZE) {
        fprintf(stderr, "Erreur : Programme dépasse la taille maximale.\n");
        exit(EXIT_FAILURE);
    }
    program[program_counter++] = (VMInstruction){opcode, operand};
}

// Empile une valeur sur la pile (convertit en logique -1 pour vrai, 0 pour faux)
void vm_push(int value) {
    if (stack_top >= STACK_SIZE - 1) {
        fprintf(stderr, "Erreur : Pile débordée.\n");
        exit(EXIT_FAILURE);
    }
    vm_stack[++stack_top] = (value != 0) ? -1 : 0; // Convertir en logique -1 pour vrai, 0 pour faux
}

// Dépile une valeur de la pile
int vm_pop() {
    if (stack_top < 0) {
        fprintf(stderr, "Erreur : Pile vide.\n");
        exit(EXIT_FAILURE);
    }
    return vm_stack[stack_top--];
}

// Fonction pour exécuter le programme de la machine virtuelle
void execute_program() {
    for (int pc = 0; pc < program_counter; pc++) {
        VMInstruction instr = program[pc];
        switch (instr.opcode) {
            case VM_NOP:
                break; // Pas d'opération
            case VM_PUSH:
                vm_push(instr.operand);
                break;
            case VM_POP:
                vm_pop();
                break;
            case VM_AND: {
                int b = vm_pop();
                int a = vm_pop();
                vm_push((a != 0 && b != 0) ? -1 : 0); // a AND b
                break;
            }
            case VM_OR: {
                int b = vm_pop();
                int a = vm_pop();
                vm_push((a != 0 || b != 0) ? -1 : 0); // a OR b
                break;
            }
            case VM_NOT: {
                int a = vm_pop();
                vm_push((a == 0) ? -1 : 0); // NOT a
                break;
            }
            case VM_IMP: {
                int b = vm_pop();
                int a = vm_pop();
                vm_push((a == 0 || b != 0) ? -1 : 0); // a ⇒ b
                break;
            }
            case VM_PRINT: {
                int value = vm_pop();
                printf("Résultat : %s\n", (value == -1) ? "VRAI" : "FAUX");
                break;
            }
            default:
                fprintf(stderr, "Erreur : Opcode inconnu %d\n", instr.opcode);
                exit(EXIT_FAILURE);
        }
    }
}

