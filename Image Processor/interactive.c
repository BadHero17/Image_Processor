// Copyright 2023 Ursu Vasile
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imageprocessing.h"
#include "bmp.h"
#define MAX 100

int main() {
    // Initializam array-ul Loaded Images, Loaded Heights si Loaded Widths
    // la array-uri nule, la care urmeaza sa realocam dinamic memoria,
    // loadcnt reprezinta numarul curent de imagini incarcate
    int loadcnt = 0, ****LI = NULL, *LH = NULL, *LW = NULL;

    // Initializam array-ul Loaded Filters si Loaded Filter size
    // pentru filtre si marimile lor, la fel le realocam dinamic
    // loadfiltcnt - numarul de filtre incarcate curent
    int loadfiltcnt = 0, *LFsize = NULL;
    float ***LF = NULL;
    while (1) {
        char command[2];
        // citim comanda pe care urmeaza s-o executam
        scanf("%s", command);
        if (!strcmp(command, "e")) {
            // Stergem filtrele si imaginile ramase si le dealocam impreuna
            // cu array-urile lor de marimi
            while (loadcnt) {
                int index = 0;
                free_matrix(LI[index], LH[index], LW[index]);
                for (int i = index; i < loadcnt - 1; i++) {
                    LI[i] = LI[i + 1];
                    LH[i] = LH[i + 1];
                    LW[i] = LW[i + 1];
                }
                int ****temp = realloc(LI, (loadcnt - 1) * sizeof(int***));
                LI = temp;
                int *temp1 = realloc(LH, (loadcnt - 1) * sizeof(int));
                LH = temp1;
                temp1 = realloc(LW, (loadcnt - 1) * sizeof(int));
                LW = temp1;
                loadcnt--;
            }
            while (loadfiltcnt) {
                int index = 0;
                free_filter(LF[index], LFsize[index]);
                for (int i = index; i < loadfiltcnt - 1; i++) {
                    LF[i] = LF[i + 1];
                    LFsize[i] = LFsize[i + 1];
                }
                float ***temp = realloc(LF, (loadfiltcnt - 1) * sizeof(float**));
                LF = temp;
                int *temp1 = realloc(LFsize, (loadfiltcnt - 1) * sizeof(int));
                LFsize = temp1;
                loadfiltcnt--;
            }
            // Oprim executia loopului, deci si a programului
            break;
        } else if (!strcmp(command, "l")) {
            // Realocam elementele din arrayurile de imagini si marimi
            // la un array cu dimensiunea mai mare cu un element
            int ****temp = malloc((loadcnt + 1) * sizeof(int***));
            if (!temp) {
                printf("Allocation error\n");
                return -1;
            }
            for (int i = 0; i < loadcnt; i++) {
                temp[i] = LI[i];
            }
            free(LI);
            LI = temp;
            int *temp1 = malloc((loadcnt + 1) * sizeof(int));
            if (!temp1) {
                printf("Allocation error\n");
                return -1;
            }
            for (int i = 0; i < loadcnt; i++) {
                temp1[i] = LH[i];
            }
            free(LH);
            LH = temp1;
            int *temp2 = malloc((loadcnt + 1) * sizeof(int));
            if (!temp2) {
                printf("Allocation error\n");
                return -1;
            }
            for (int i = 0; i < loadcnt; i++) {
                temp2[i] = LW[i];
            }
            free(LW);
            LW = temp2;
            int N = 0, M = 0;
            char path[MAX];
            scanf("%d%d", &N, &M);
            scanf("%s", path);
            LI[loadcnt] = alloc_matrix(N, M);
            LH[loadcnt] = N;
            LW[loadcnt] = M;

            // Dupa ce am citit marimile si alocam loc pentru imagine,
            // incarcam imaginea in sine
            read_from_bmp(LI[loadcnt], N, M, path);
            loadcnt++;
        } else if (!strcmp(command, "s")) {
            int index = 0;
            char path[MAX] = "";
            scanf("%d", &index);
            scanf("%s", path);

            // Citim index si salvam imaginea de pe pozitia respectiva
            write_to_bmp(LI[index], LH[index], LW[index], path);
        } else if (!strcmp(command, "ah")) {
            int index = 0;
            scanf("%d", &index);

            // Aplicam horizontal flip pe imaginea de pe pozitia index
            LI[index] = flip_horizontal(LI[index], LH[index], LW[index]);
        } else if (!strcmp(command, "ar")) {
            int index = 0;
            scanf("%d", &index);

            // Aplicam rotate_left pe imaginea de pe pozitia index
            LI[index] = rotate_left(LI[index], LH[index], LW[index]);

            // Inversam marimile imaginii dupa rotatie
            int temp = LH[index];
            LH[index] = LW[index];
            LW[index] = temp;
        } else if (!strcmp(command, "ac")) {
            int index = 0, x = 0, y = 0, w = 0, h = 0;
            scanf("%d%d%d%d%d", &index, &x, &y, &w, &h);

            // Aplicam crop si modificam dimensiunile imaginii pentru procesarea ulterioara
            LI[index] = crop(LI[index], LH[index], LW[index], x, y, h, w);
            LH[index] = h;
            LW[index] = w;
        } else if (!strcmp(command, "ae")) {
            int index = 0, rows = 0, cols = 0, R = 0, G = 0, B = 0;
            scanf("%d%d%d%d%d%d", &index, &rows, &cols, &R, &G, &B);

            // Aplicam extend si marim dimensiunea imaginii
            LI[index] = extend(LI[index], LH[index], LW[index], rows, cols, R, G, B);
            LH[index] = 2 * rows + LH[index];
            LW[index] = 2 * cols + LW[index];
        } else if (!strcmp(command, "ap")) {
            // Citim index_sursa si index_destinatie, dupa care aplicam paste
            // imaginii de la destinatie pe imaginea de la sursa
            int i_d = 0, i_s = 0, x = 0, y = 0;
            scanf("%d%d%d%d", &i_d, &i_s, &x, &y);
            LI[i_d] = paste(LI[i_d], LH[i_d], LW[i_d], LI[i_s], LH[i_s], LW[i_s], x, y);
        } else if (!strcmp(command, "cf")) {
            int size = 0;
            scanf("%d", &size);

            // Alocam loc pentru un element de tip filtru in array si pentru marimea sa
            float ***temp = realloc(LF, (loadfiltcnt + 1) * sizeof(float**));
            if (!temp) {
                printf("Allocation error\n");
                return -1;
            }
            LF = temp;
            int *temp1 = realloc(LFsize, (loadfiltcnt + 1) * sizeof(int));
            if (!temp1) {
                printf("Allocation error\n");
                return -1;
            }
            LFsize = temp1;
            LFsize[loadfiltcnt] = size;

            // Alocam loc pentru filtru si il citim
            LF[loadfiltcnt] = alloc_filter(size);
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    scanf("%f", &LF[loadfiltcnt][i][j]);
                }
            }
            loadfiltcnt++;
        } else if (!strcmp(command, "af")) {
            // Citim indexul imaginii si indexul filtrului,
            // dupa care aplicam imaginii de la indexul citit filtrul de indexul citit
            int i_i = 0, i_f = 0;
            scanf("%d%d", &i_i, &i_f);
            LI[i_i] = apply_filter(LI[i_i], LH[i_i], LW[i_i], LF[i_f], LFsize[i_f]);
        } else if (!strcmp(command, "df")) {
            int index = 0;
            scanf("%d", &index);

            // Stergem filtrul de pe pozitia index
            // Mutam elementele de la dreapta de index cu o pozitie la stanga
            free_filter(LF[index], LFsize[index]);
            for (int i = index; i < loadfiltcnt - 1; i++) {
                LF[i] = LF[i + 1];
                LFsize[i] = LFsize[i + 1];
            }

            // Micsoram dimensiunea array-ului de filtre si a celui de marimi
            float ***temp = realloc(LF, (loadfiltcnt - 1) * sizeof(float**));
            LF = temp;
            int *temp1 = realloc(LFsize, (loadfiltcnt - 1) * sizeof(int));
            LFsize = temp1;
            loadfiltcnt--;
        } else if (!strcmp(command, "di")) {
            int index = 0;
            scanf("%d", &index);
            free_matrix(LI[index], LH[index], LW[index]);
            if (loadcnt == 1) {
                free(LI);
                free(LH);
                free(LW);
                loadcnt--;
                continue;
            }

            // Stergem imaginea de la index si mutam imaginile de la dreapta de ea
            // in array cu o pozitie la stanga
            int ****temp = malloc((loadcnt - 1) * sizeof(int***));
            for (int i = 0; i < index; i++) {
                temp[i] = LI[i];
            }
            for (int i = index; i < loadcnt - 1; i++) {
                temp[i] = LI[i + 1];
            }
            // Realocam arrayurile la altele cu o marime mai mica cu un element
            free(LI);
            LI = temp;
            int *temp1 = malloc((loadcnt - 1) * sizeof(int));
            for (int i = 0; i < index; i++) {
                temp1[i] = LH[i];
            }
            for (int i = index; i < loadcnt - 1; i++) {
                temp1[i] = LH[i + 1];
            }
            free(LH);
            LH = temp1;
            temp1 = malloc((loadcnt - 1) * sizeof(int));
            for (int i = 0; i < index; i++) {
                temp1[i] = LW[i];
            }
            for (int i = index; i < loadcnt - 1; i++) {
                temp1[i] = LW[i + 1];
            }
            free(LW);
            LW = temp1;
            loadcnt--;
        }
    }

    return 0;
}
