#include <stdio.h>
#include <stdlib.h>
 
#include "matrix.h"

matrix *alloc_matrix(int rows, int cols)
{
    matrix *m = (matrix *)malloc(sizeof(matrix));
    if (m == NULL)
    {
        perror("malloc");
        exit(1);
    }

    m->num_rows = rows;
    m->num_cols = cols;

    m->data = (int **)malloc(m->num_rows*sizeof(int *));
    if (m->data == NULL)
    {
        perror("malloc");
        exit(1);
    }
    for (int r=0; r<m->num_rows; ++r)
    {
        m->data[r] = (int *)malloc(m->num_cols*sizeof(int));
        if (m->data[r] == NULL)
        {
          perror("malloc");
          exit(1);
        }
    }

    return m;
}

matrix *read_matrix(char *fname)
{
    FILE *mfile = fopen(fname, "r");
    if (mfile == NULL)
    {
        perror("fopen");
        exit(1);
    }

    int num_rows, num_cols;
    if (fscanf(mfile, "%d\n%d\n", &num_rows, &num_cols) != 2)
    {
        printf("Format error in %s\n", fname);
        exit(1);
    }
    if (num_rows < 1)
    {
        printf("Row value error in %s\n", fname);
        exit(1);
    }
    if (num_cols < 1)
    {
        printf("Column value error in %s\n", fname);
        exit(1);
    }

    matrix *m = alloc_matrix(num_rows, num_cols);

    for (int r=0; r<m->num_rows; ++r)
    {
        for (int c=0; c<m->num_cols; ++c)
        {
            if (fscanf(mfile, "%d", &m->data[r][c]) != 1)
            {
                printf("Format error in %s\n", fname);
                exit(1);
            }
        }
    }
    return m;
}

matrix *multiply_matrix(matrix *m1, matrix *m2)
{
    if (m1->num_cols != m2->num_rows)
    {
        printf("Matrix dimensions don't match!");
        exit(1);
    }

    matrix *res = alloc_matrix(m1->num_rows, m2->num_cols);

    for (int r=0; r<res->num_rows; ++r)
    {
        for (int c=0; c<res->num_cols; ++c)
        {
            res->data[r][c] = 0;
            for (int k=0; k<m1->num_cols; ++k)
            {
                res->data[r][c] += m1->data[r][k] * m2->data[k][c];
            }
        }
    }

    return res;
}

void print_matrix(matrix *m)
{
    int max = m->data[0][0];
    int min = m->data[0][0];
    for (int r=0; r<m->num_rows; ++r)
    {
        for (int c=0; c<m->num_cols; ++c)
        {
            if (max < m->data[r][c])
            {
                max = m->data[r][c];
            }
            if (min > m->data[r][c])
            {
                min = m->data[r][c];
            }
        }
    }
    int max_len = snprintf(NULL, 0, "%d", max);
    int min_len = snprintf(NULL, 0, "%d", min);
    int longest = max_len>min_len ? max_len : min_len;

    printf("%d\n%d\n", m->num_rows, m->num_cols);
    for (int r=0; r<m->num_rows; ++r)
    {
        for (int c=0; c<m->num_cols; ++c)
        {
            printf("%*d", longest+1, m->data[r][c]);
        }
        printf("\n");
    }
}

void free_matrix(matrix *m)
{
    for (int r=0; r<m->num_rows; ++r)
    {
        free(m->data[r]);
    }
    free(m->data);
    free(m);
}
