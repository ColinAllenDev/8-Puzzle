#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

/* The list of available moves relevant to the empty space. */
typedef enum Move { ABOVE, BELOW, LEFT, RIGHT, } Move;

/* The data-structure representing a board configuration. 
* It also contains an enum representing the last move used to create this configuration.
* It also contains an array representing the current configuration.
*/
typedef struct Board {
    Move move;           
    int config[3][3];
} Board;

void PrintMove(Board* b) {
    char* MoveStr[4] = { "ABOVE", "BELOW", "LEFT", "RIGHT" };
    printf("Move: %s\n", MoveStr[b->move]);
}

// Randomly shuffle an array representing the board configuration
void ShuffleBoard(int* arr) {
    for (int i = 0; i < 9 - 0; i++) {
        size_t j = i + rand() / (RAND_MAX / (9 - i) + 0);
        int t = arr[j];
        arr[j] = arr[i];
        arr[i] = t;
    }
}

/* Create a new board with a random initial configuration */
void NewBoard(Board* b, bool isGoal, bool isRandom) {
    int initArr[] = { 2, 8, 3, 1, 6, 4, 7, 0, 5 };
    int goalArr[] = { 1, 2, 3, 8, 0, 4, 7, 6, 5 };

    // Randomly shuffle the initial configuration
    if (isRandom) {
        ShuffleBoard(initArr);
    }

    // Populate spaces in board.
    int a = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (isGoal == false)
                b->config[i][j] = initArr[a++];
            else
                b->config[i][j] = goalArr[a++];
        }
    }
}

/* Create a new board configuration if the proposed move is valid. */
Board* NewBoardIfValid(Board* b_parent, Move move) {
    Board* b_tmp = malloc(sizeof(Board));

    int e_row, e_col; 
    
    // Look for empty cell in grid
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (b_parent->config[i][j] == 0) {
                e_row = i;
                e_col = j;
            }
            // Copy values from the parent configuration to the current configuration.
            b_tmp->config[i][j] = b_parent->config[i][j];
        }
    }

    // Check for valid moves based on the position of the empty space and the proposed move.
    // If found, swap the empty space and the corresponding space
    if (move == ABOVE && e_row - 1 >= 0) {
        int swap = b_tmp->config[e_row - 1][e_col];
        b_tmp->config[e_row - 1][e_col] = 0;
        b_tmp->config[e_row][e_col] = swap;
        b_tmp->move = ABOVE;
        return b_tmp;
    }
    else if (move == BELOW && e_row + 1 < 3) {
        int swap = b_tmp->config[e_row + 1][e_col];
        b_tmp->config[e_row + 1][e_col] = 0;
        b_tmp->config[e_row][e_col] = swap;
        b_tmp->move = BELOW;
        return b_tmp;
    }
    else if (move == LEFT && e_col - 1 >= 0) {
        int swap = b_tmp->config[e_row][e_col - 1];
        b_tmp->config[e_row][e_col - 1] = 0;
        b_tmp->config[e_row][e_col] = swap;
        b_tmp->move = LEFT;
        return b_tmp;
    }
    else if (move == RIGHT && e_col + 1 < 3) {
        int swap = b_tmp->config[e_row][e_col + 1];
        b_tmp->config[e_row][e_col + 1] = 0;
        b_tmp->config[e_row][e_col] = swap;
        b_tmp->move = RIGHT;
        return b_tmp;
    }

    // If the proposed move does not result in a valid board configuration,
    // deallocate memory and return NULL
    free(b_tmp);
    return NULL;
}

void PrintBoard(Board* b) {
    printf("===\n");
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (b->config[i][j] == 0) {
                printf(" ");
                continue;
            }

            printf("%i", b->config[i][j]);
        }
        printf("\n");
    }
    printf("===\n");
}

bool AreBoardsEqual(Board const* b_1, Board const* b_2) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (b_1->config[i][j] != b_2->config[i][j])
                return false;
        }
    }
    
    return true;
}