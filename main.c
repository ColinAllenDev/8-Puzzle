#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include "Board.h"
#include "Queue.h"
#include "Node.h"
#include "Algorithm.h"

int main(void) {
    srand(time(NULL));

    Board b_init; // Initial board
    Board b_goal; // Goal board

    NewBoard(&b_init, false, false); // Create a random inital board.
    NewBoard(&b_goal, true, false); // Create the goal board.

    // Print initial board configuration
    printf("- Initial Board Configuration -\n");
    PrintBoard(&b_init);

    // Print the goal board configuration
    printf("- Goal Configuration -\n");
    PrintBoard(&b_goal);

    /* Search Algorithms */
    /* Breadth - First Search(BFS)
    printf("--- BREADTH FIRST SEARCH ---\n");
    Algorithm* A_BFS;
    A_BFS = BFS(&b_init, &b_goal);
    PrintAlgorithm(A_BFS);
    FreeAlgorithm(&A_BFS);
    */
    /* Uniform - Cost Search(UCS)
    printf("--- UNIFORM COST SEARCH ---\n");
    Algorithm* A_UCS;
    A_UCS = UCS(&b_init, &b_goal);
    PrintAlgorithm(A_UCS);
    FreeAlgorithm(&A_UCS);
    */

    /* Simulated Annealing (SA) */
    printf("--- SIMULATED ANNEALING ---\n");
    Algorithm* A_SA;
    A_SA = SA(&b_init, &b_goal);

    return 0;
}
