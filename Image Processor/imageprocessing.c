// Copyright 2023 Ursu Vasile
#include <stdio.h>
#include <stdlib.h>
#include "imageprocessing.h"
#define CNT 3
#define MAX_CHAR 255

// Auxiliary function for image matrix allocation with allocation
// error check
int ***alloc_matrix(int N, int M) {
    int ***result = malloc(N * sizeof(int**));
    if (result == NULL) {
        printf("Allocation error\n");
        return NULL;
    }
    for (int i = 0; i < N; i++) {
        result[i] = malloc(M * sizeof(int*));
        if (result[i] == NULL) {
            printf("Allocation error\n");
            return NULL;
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            result[i][j] = malloc(CNT * sizeof(int));
            if (result[i][j] == NULL) {
                printf("Allocation error\n");
                return NULL;
            }
        }
    }
    return result;
}
// Auxiliary function for freeing allocated matrix memory
void free_matrix(int ***image, int N, int M) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            free(image[i][j]);
        }
    }
    for (int i = 0; i < N; i++) {
        free(image[i]);
    }
    free(image);
}

// Auxiliary function for allocating filters;
float **alloc_filter(int filter_size) {
    float **filter = (float**)malloc(filter_size * sizeof(float*));
    if (filter == NULL) {
        printf("Allocation error\n");
        return NULL;
    }
    for (int i = 0; i < filter_size; i++) {
        filter[i] = (float*)malloc(filter_size * sizeof(float));
            if (filter[i] == NULL) {
            printf("Allocation error\n");
            return NULL;
        }
    }
    return filter;
}

// Auxiliary function for freeing allocated filter memory
void free_filter(float **filter, int filter_size) {
    for (int i = 0; i < filter_size; i++) {
        free(filter[i]);
    }
    free(filter);
}

int ***flip_horizontal(int ***image, int N, int M) {
    int ***flip_matrix = alloc_matrix(N, M);
    if (flip_matrix == NULL) {
        printf("Allocation error\n");
        return NULL;
    }

    // Punem in ordine inversa elementele din image in imaginea rezultanta
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            for (int k = 0; k < CNT; k++) {
                flip_matrix[i][j][k] = image[i][M -j -1][k];
            }
        }
    }
    free_matrix(image, N, M);
    return flip_matrix;
}

int ***rotate_left(int ***image, int N, int M) {
    int ***rotate_matrix = alloc_matrix(M, N);
    if (rotate_matrix == NULL) {
        printf("Allocation error\n");
        return NULL;
    }

    // Punem elementele la pozitii incepand cu coltul stanga jos
    // iterand in sus si apoi la dreapta
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            for (int k = 0; k < CNT; k++) {
                rotate_matrix[M - j - 1][i][k] = image[i][j][k];
            }
        }
    }
    free_matrix(image, N, M);
    return rotate_matrix;
}

int minn(int a, int b) {
    return a < b ? a : b;
}

int ***crop(int ***image, int N, int M, int x, int y, int h, int w) {
    int ***crop_matrix = alloc_matrix(h, w);
    if (crop_matrix == NULL) {
        printf("Allocation error\n");
        return NULL;
    }

    // Citim doar elementele necesare din image si le punem in matricea target
    // de dimensiune mai mica
    for (int i = y; i < minn(y + h, N); i++) {
        for (int j = x; j < minn(x + w, M); j++) {
            for (int k = 0; k < CNT; k++) {
                crop_matrix[i - y][j - x][k] = image[i][j][k];
            }
        }
    }
    free_matrix(image, N, M);
    return crop_matrix;
}


int ***extend(int ***image, int N, int M, int rows, int cols, int new_R, int new_G, int new_B) {
    int ***extend_matrix = alloc_matrix(2 * rows + N, 2 * cols + M);
    if (extend_matrix == NULL) {
        printf("Allocation error\n");
        return NULL;
    }

    // Adaugam fasiile orizontale de sus apoi dedesubtul imaginii
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < 2 * cols + M; j++) {
            extend_matrix[i][j][0] = new_R;
            extend_matrix[i][j][1] = new_G;
            extend_matrix[i][j][2] = new_B;
        }
    }
    for (int i = N + rows; i < 2 * rows + N; i++) {
        for (int j = 0; j < 2 * cols + M; j++) {
            extend_matrix[i][j][0] = new_R;
            extend_matrix[i][j][1] = new_G;
            extend_matrix[i][j][2] = new_B;
        }
    }

    // Adaugam elementele ramase din fasiile verticale
    for (int i = rows; i < N + rows; i++) {
        for (int j = 0; j < cols; j++) {
            extend_matrix[i][j][0] = new_R;
            extend_matrix[i][j][1] = new_G;
            extend_matrix[i][j][2] = new_B;
        }
    }
    for (int i = rows; i < N + rows; i++) {
        for (int j = cols + M; j < 2 * cols + M; j++) {
            extend_matrix[i][j][0] = new_R;
            extend_matrix[i][j][1] = new_G;
            extend_matrix[i][j][2] = new_B;
        }
    }

    // Copiem imaginea la pozitia respectiva in imaginea noua
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            for (int k = 0; k < CNT; k++) {
                extend_matrix[i + rows][j + cols][k] = image[i][j][k];
            }
        }
    }
    free_matrix(image, N, M);
    return extend_matrix;
}

int ***paste(int ***image_dst, int N_dst, int M_dst, int *** image_src, int N_src, int M_src, int x, int y) {
    // Copiem direct elementele de la src la dst
    // Atentie !!! Daca incercam sa copiem elemente dintr-o imagine peste aceeasi imagine, pot aparea
    // artefacte nedorite, deoarece functia nu asigura datele prin intermediul unei imagini
    // intermediare

    for (int i = 0; i < minn(N_dst - y, N_src); i++) {
        for (int j = 0; j < minn(M_dst - x, M_src); j++) {
            for (int k = 0; k < CNT; k++) {
                image_dst[i + y][j + x][k] = image_src[i][j][k];
            }
        }
    }
    return image_dst;
}

// Functie auxiliara pentru clamping la tipul unsigned char
int shrink_to_char(int x) {
    if (x < 0) {
        return 0;
    }
    if (x > MAX_CHAR) {
        return MAX_CHAR;
    }
    return x;
}

int ***apply_filter(int ***image, int N, int M, float **filter, int filter_size) {
    int ***filter_matrix = alloc_matrix(N, M);
    if (filter_matrix == NULL) {
        printf("Allocation error\n");
        return NULL;
    }
    // Offset reprezinta 'raza' patratului din intermediul caruia folosim pixelii pentru calcul
    int offset = filter_size / 2;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            float R_val = 0;
            float G_val = 0;
            float B_val = 0;
            for (int i1 = i - offset; i1 <= i + offset; i1++) {
                for (int j1 = j - offset; j1 <= j + offset; j1++) {
                    // Daca pixelii sunt inafara imaginii ii ignoram, echivalent cu a  aduna 0
                    if (i1 >= 0 && j1 >= 0 && i1 < N && j1 < M) {
                        R_val += filter[i1 - i + offset][j1 - j + offset] * (float)image[i1][j1][0];
                        G_val += filter[i1 - i + offset][j1 - j + offset] * (float)image[i1][j1][1];
                        B_val += filter[i1 - i + offset][j1 - j + offset] * (float)image[i1][j1][2];
                    }
                }
            }

            // Dupa ce am calculat culorile pixelui nou, le atribuim
            filter_matrix[i][j][0] = shrink_to_char((int)R_val);
            filter_matrix[i][j][1] = shrink_to_char((int)G_val);
            filter_matrix[i][j][2] = shrink_to_char((int)B_val);
        }
    }
    free_matrix(image, N, M);
    return filter_matrix;
}
