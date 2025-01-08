#ifndef ANASEM_H
#define ANASEM_H

#include "anasynt.h"

//Analyse semantique
void analyseur_semantique(ASTNode* ast);

//Initialisation des propositions considérées comme valides
void initialize_valid_props(void);
//Libérer la memoire allouée par les propositions valides
void free_valid_props_memory(void);

#endif

