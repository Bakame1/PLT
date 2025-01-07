#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

//taille fini pour le nombre de lexemes
#define MAX_LEXEMES 100

//séquences UTF-8 pour les opérateurs logiques
#define ET_LOGIQUE "\xE2\x88\xA7"           // (U+2227)
#define OU_LOGIQUE "\xE2\x88\xA8"           //  (U+2228)
#define NON_LOGIQUE "\xC2\xAC"              //  (U+00AC)
#define PRODUIT "\xE2\x86\x92"              //  (U+2192)
#define IMPLICATION_DOUBLE "\xE2\x87\x92"    //  (U+21D2)

//fonction pour comparer une sous chaine avec une séquence UTF-8
int match_sequence(const char* chaine, int pos, const char* seq) {
    return strncmp(&chaine[pos], seq, strlen(seq)) == 0;
}

//Fonction creant la liste des lexèmes
char** CreationListeLexeme(const char* chaine) {
    //allouer dynamiquement la liste des lexemes
    char** ListeLexeme = malloc(MAX_LEXEMES * sizeof(char*));
    if (!ListeLexeme) {
        perror("echec de l'allocation memoire");
        exit(EXIT_FAILURE);
    }

    int n = strlen(chaine);
    int i = 0;
    int lex_idx = 0;

    if (n == 0) {
        perror("Chaine vide");
        exit(EXIT_FAILURE);
    }

    while (i < n) {
        //on verifie en premier les sequences UTF-8
        if (match_sequence(chaine, i, ET_LOGIQUE)) {
            ListeLexeme[lex_idx++] = strdup("Op(ET)");
            i += strlen(ET_LOGIQUE);
        }
        else if (match_sequence(chaine, i, OU_LOGIQUE)) {
            ListeLexeme[lex_idx++] = strdup("Op(OU)");
            i += strlen(OU_LOGIQUE);
        }
        else if (match_sequence(chaine, i, NON_LOGIQUE)) {
            ListeLexeme[lex_idx++] = strdup("Op(NON)");
            i += strlen(NON_LOGIQUE);
        }
        else if (match_sequence(chaine, i, IMPLICATION_DOUBLE)) {
            ListeLexeme[lex_idx++] = strdup("Op(IMPLIQUE)");
            i += strlen(IMPLICATION_DOUBLE);
        }
        else if (match_sequence(chaine, i, PRODUIT)) {
            ListeLexeme[lex_idx++] = strdup("Op(PRODUIT)");
            i += strlen(PRODUIT);
        }
        else {
            switch (chaine[i]) {
                case '(': //Parenthèse ouvrante
                    ListeLexeme[lex_idx++] = strdup("PO");
                    i++;
                    break;
                case ')': //Parenthèse fermante
                    ListeLexeme[lex_idx++] = strdup("PF");
                    i++;
                    break;
                default:
                    if (islower((unsigned char)chaine[i])) { //Pour une lettre minuscule
                        int start = i;
                        //On avance l'index pour capturer la proposition complete (lettre + chiffres eventuels)
                        while (i < n && (islower((unsigned char)chaine[i]) || isdigit((unsigned char)chaine[i]))) {
                            i++;
                        }
                        int len = i - start;
                        //On alloue suffisamment de memoire pour "Prop(" + identifiant + ")" + '\0'
                        char* res = malloc(6 + len + 1); // "Prop(" + identifiant + ")" + '\0'
                        if (!res) {
                            perror("Échec de l'allocation mémoire");
                            //On libere la mémoire allouee avant de quitter
                            for (int j = 0; j < lex_idx; j++) {
                                free(ListeLexeme[j]);
                            }
                            free(ListeLexeme);
                            exit(EXIT_FAILURE);
                        }
                        snprintf(res, 6 + len + 1, "Prop(%.*s)", len, &chaine[start]);
                        ListeLexeme[lex_idx++] = res;
                    }
                    else if (isspace((unsigned char)chaine[i])) {
                        //Ignorer les espaces
                        i++;
                    }
                    else {
                        fprintf(stderr, "Lexeme invalide dans la chaine: '%c'\n", chaine[i]);
                        //Liberer la memoire allouee avant de quitter
                        for (int j = 0; j < lex_idx; j++) {
                            free(ListeLexeme[j]);
                        }
                        free(ListeLexeme);
                        exit(EXIT_FAILURE);
                    }
                    break;
            }
        }

        //Verifier si la liste des lexèmes a atteint sa capacité maximale
        if (lex_idx >= MAX_LEXEMES - 1) {
            fprintf(stderr, "Nombre maximal de lexemes atteint\n");
            break;
        }
    }

    ListeLexeme[lex_idx] = NULL; //Terminer la liste des lexemes avec NULL

    return ListeLexeme;
}


