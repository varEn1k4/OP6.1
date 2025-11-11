#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>

float isItValid(char textOutput[], float min, float max);
float **allocationMatrix(unsigned short amount);
float *allocationNumbers(unsigned short amount);
void freeMatrix(float **matrix, unsigned short n);
void inputInRow(float **matrix, float *numbers, unsigned short rowNumber, unsigned short amount);
int neededCalculations(float **a, float *b, float *x, unsigned short amount, float epsilon);


int main(void) {
    unsigned short amount = 0;
    float epsilon = 0;
    char exitE = 0;

    printf("This is program for system of linear algebraic equation\n");
    printf("Your input must satisfy the conditions:\n");
    printf("1. Element a[i][i] do not equal 0\n");
    printf("2. Module of a[i][i] must be greater than sum of all elements in a row [i] (except a[i][i])\n");

    do {
        amount = (unsigned short)isItValid("Enter number of equations (2-7): ", 2, 7);
        epsilon = isItValid("Enter accuracy (epsilon): ", 1e-4f, 1.0f);

        float **matrix = allocationMatrix(amount);
        if (!matrix) {
            printf("Memory allocation error\n");
            continue;
        }

        float *numbers = allocationNumbers(amount);
        if (!numbers) {
            printf("Memory allocation error\n");
            freeMatrix(matrix, amount);
            continue;
        }

        float *x = allocationNumbers(amount);
        if (!x) {
            printf("Memory allocation error\n");
            freeMatrix(matrix, amount);
            free(numbers);
            continue;
        }

        for (int i = 0; i < amount; i++) {
            inputInRow(matrix, numbers, i, amount);
        }


        int iterations = neededCalculations(matrix, numbers, x, amount, epsilon);
        if (iterations > 0) {
            printf("Converged in %d iterations\n", iterations);
            printf("Solution:\n");
            for (int i = 0; i < amount; i++) {
                printf("x%d = %.5f\n", i+1, x[i]);
            }
        } else {
            printf("Did not converge\n");
        }

        freeMatrix(matrix, amount);
        free(numbers);
        free(x);
        printf("Press 1 and ENTER to end; just Enter to continue");
        exitE = getch();
    } while (exitE != '1');
    return 0;
}


float isItValid(char textOutput[], float min, float max) {
    float number = 0;
    char extra = 0;
    int validInput = 1;
    do {
        printf("%s", textOutput);
        unsigned short result = scanf(" %10f%c", &number, &extra);

        if (result !=2 || extra != '\n') {
            printf("ERROR: Invalid input. Please enter ONLY a number\n");
            while (getchar() != '\n'){}
            continue;
        }

        if (number < min || number > max) {
            printf("ERROR: Your value is not correct. Please enter a new number\n");
            continue;
        }
        validInput = 0;
    } while (validInput);
    return number;
}

float **allocationMatrix(unsigned short amount) {
    float **matrix = malloc(amount * sizeof(float*));
    if (matrix == NULL) {
        printf("Memory Error");
        return NULL;
    }

    for (int i = 0; i < amount; i++) {
        matrix[i] = (float*)malloc(amount * sizeof(float));
        if (matrix[i] == NULL) {
            printf("Memory Error");
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            return NULL;
        }
    }
    return matrix;
}

float *allocationNumbers(unsigned short amount) {
    return malloc(amount * sizeof(float));
}

void freeMatrix(float **matrix, unsigned short n) {
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

int neededCalculations(float **a, float *b, float *x, unsigned short amount, float epsilon) {
    float *xp = allocationNumbers(amount);
    float *xCalc = allocationNumbers(amount);

    if (!xp || !xCalc) {
        free(xp);
        free(xCalc);
        return -1;
    }

    int iteration = 1;
    int maxIterations = 10000;

    for (int i = 0; i < amount; i++) {
        xp[i] = b[i] / a[i][i];
    }

    do {
        for (int i = 0; i < amount; i++) {
            float sum = 0.f;
            for (int j = 0; j < amount; j++) {
                if (j != i) {
                    sum += a[i][j] * xp[j];
                }
            }
            xCalc[i] = (b[i] - sum) / a[i][i];
        }

        float maxDelta = 0.f;
        for (int i = 0; i < amount; i++) {
            float delta = fabsf(xCalc[i] - xp[i]);
            if (delta > maxDelta) {
                maxDelta = delta;
            }
        }

        if (maxDelta < epsilon) {
            for (int i = 0; i < amount; i++) {
                x[i] = xCalc[i];
            }
            free(xp);
            free(xCalc);
            return iteration;
        }

        for (int i = 0; i < amount; i++) {
            xp[i] = xCalc[i];
        }
        iteration++;
    } while (iteration < maxIterations);
    free(xp);
    free(xCalc);
    return -1;
}


void inputInRow(float **matrix, float *numbers, unsigned short rowNumber, unsigned short amount) {
    int isRowValid = 0;
    do {
        float sum = 0.f;

        for (int j = 0; j < amount; j++) {
            char symbols[10];
            sprintf(symbols, "a[%d][%d]: ", rowNumber+1, j+1);
            matrix[rowNumber][j] = isItValid(symbols, -100.f, 100.0f);
        }
        char symbols[7];
        sprintf(symbols, "b[%d]: ", rowNumber+1);
        numbers[rowNumber] = isItValid(symbols, -100.f, 100.0f);

        if (fabsf(matrix[rowNumber][rowNumber]) < 1e-4) {
            printf("Your number is too small or equal 0. Enter a new one\n");
            continue;
        }

        for (int j = 0; j < amount; j++) {
            if (j != rowNumber) {
                sum += fabsf(matrix[rowNumber][j]);
            }
        }

        if (fabsf(matrix[rowNumber][rowNumber]) < sum) {
            printf("Your row doesn't satisfy the second condition\n");
            continue;
        }
        isRowValid = 1;
    }while (!isRowValid);
}