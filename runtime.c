#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//On commence par définir les différents codes d'opérations que la machine virtuelle peut exécuter.
typedef enum {
    VM_NOP,    //Pas d'opération
    VM_PUSH,   //Empile une valeur sur la pile
    VM_POP,    //Retire une valeur de la pile
    VM_AND,    //Opération ET logique
    VM_OR,     //Opération OU logique
    VM_NOT,    //Opération NON logique
    VM_IMP,    //Opération IMPLIQUE
    VM_PRINT   //Affiche une valeur
} VMOpcode;

//L'instruction que la machine virtuelle doit exécuter.
//Chaque instruction contient le type d'opération à effectuer ainsi que son opérande associé
typedef struct {
    VMOpcode opcode;   //Opération à effectuer
    int operand;       //Opérande associé
} VMInstruction;

//Pour stocker les valeurs logiques pendant l'exécution des instructions, on utilisera une pile avec une taille maximale
#define STACK_SIZE 256
int vm_stack[STACK_SIZE];
int stack_top = -1;

//La série d'instructions que la machine virtuelle va exécuter avec une taille maximale également :
#define PROGRAM_SIZE 1024
VMInstruction program[PROGRAM_SIZE];
int program_counter = 0; //Compteur d'instructions indiquant à chaque fois l'instruction à exécuter.


/*Ici, on ajoute une instruction au programme de la machine virtuelle.
     On vérifie que le programme n'excède pas la taille maximale.
     Ensuite on insère l'instruction dans le tableau 'program' et on incrémente le 'program_counter'.
*/
//Parametre opcode : VMOpcode
//Parametre operand : entier
void add_instruction(VMOpcode opcode, int operand) {
    if (program_counter >= PROGRAM_SIZE) {
        fprintf(stderr, "Erreur : Programme dépasse la taille maximale.\n");
        exit(EXIT_FAILURE);
    }
    program[program_counter++] = (VMInstruction){opcode, operand};
}


/*On empile une valeur sur la pile de la machine virtuelle.
    On convertit la valeur en logique :
    - -1 représente VRAI
    - 0 représente FAUX
 */
//Parametre value : entier
void vm_push(int value) {
    if (stack_top >= STACK_SIZE - 1) {
        fprintf(stderr, "Erreur : Pile débordée.\n");
        exit(EXIT_FAILURE);
    }
    vm_stack[++stack_top] = (value != 0) ? -1 : 0; //Conversion en logique (-1 pour vrai, 0 pour faux)
}

//On dépile une valeur de la pile de la machine virtuelle
int vm_pop() {
    if (stack_top < 0) {
        fprintf(stderr, "Erreur : Pile vide.\n");
        exit(EXIT_FAILURE);
    }
    return vm_stack[stack_top--];
}

//On exécute le programme de la machine virtuelle :
void execute_program() {
    for (int pc = 0; pc < program_counter; pc++) {
        VMInstruction instr = program[pc];
        switch (instr.opcode) {
            case VM_NOP://Pas d'opération à effectuer
                break; 
            case VM_PUSH://On empile l'opérande sur la pile
                vm_push(instr.operand);
                break;
            case VM_POP://On dépile une valeur de la pile
                vm_pop();
                break;
            case VM_AND: { //On dépile 2 valeurs, on effectue un ET logique et on empile le résultat
                int b = vm_pop();
                int a = vm_pop();
                vm_push((a != 0 && b != 0) ? -1 : 0); //a ET b
                break;
            }
            case VM_OR: { //Pareil, on dépile 2 valeurs, on effectue un OU logique et on empile le résultat
                int b = vm_pop();
                int a = vm_pop();
                vm_push((a != 0 || b != 0) ? -1 : 0); //a OU b
                break;
            }
            case VM_NOT: {
                int a = vm_pop();
                vm_push((a == 0) ? -1 : 0); //NOT a
                break;
            }
            case VM_IMP: {
                int b = vm_pop();
                int a = vm_pop();
                vm_push((a == 0 || b != 0) ? -1 : 0); //a ⇒ b
                break;
            }
            case VM_PRINT: {  //On dépile la valeur, et en fonction on va afficher VRAI ou FAUX
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


