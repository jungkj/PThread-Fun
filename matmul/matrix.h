#ifndef MATRIX_H
#define MATRIX_H

typedef struct _matrix
{
    int **data;
    int num_rows;
    int num_cols;
} matrix;

matrix *alloc_matrix(int rows, int cols);
matrix *read_matrix(char *fname);
matrix *multiply_matrix(matrix *m1, matrix *m2);
void print_matrix(matrix *m);
void free_matrix(matrix *m);

#endif
