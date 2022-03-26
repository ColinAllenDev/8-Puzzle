#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>

#include "Board.h"
#include "Queue.h"
#include "Node.h"

#define MAX_NODES 50000

/* The 'list' data structure representing the path to the goal*/
typedef struct Path {
    Move move;
    struct Path* next;
} Path;

/* The search algorithm with the relevant path and metadata */
typedef struct Algorithm {
    unsigned int NodesVisited;
    unsigned int MovesPerformed;
    double ComputationTime;

    Path* path;
} Algorithm;

/* Breadth-First Search Implementation */
Algorithm* BFS(Board* b_init, Board* b_goal) {
    Algorithm* a_tmp = malloc(sizeof(Algorithm));
    a_tmp->MovesPerformed = 0;
    a_tmp->NodesVisited = 0;
    NodeQueue* queue = NULL;
    NodeQueue* children = NULL;
    Node* node = NULL;

    // Begin computation timer
    clock_t c_timer_begin = clock();

    // Push the first node into the queue
    PushNode(NewNode(0, b_init, NULL), &queue);
    
    // Stored in order to deallocate the tree from memory later
    Node* n_root = queue->qn_head->n_current;

    // While there is a node in the queue to be processed...
    while (queue->n_count > 0) {
        node = PopNode(&queue);

        // BFS consumes a lot of memory, some random configurations are unsolvable with a given amount of memory.
        // If the the random configuration is impossible to solve given the amount of memory we have,
        // end the program and try again
        if (a_tmp->NodesVisited >= MAX_NODES) {
            printf("The board is unsolvable with this configuration... Please try Again...");
            exit(EXIT_FAILURE);
        }

        // Check if the tail node's board configuration is equal to the goal board
        if (AreBoardsEqual(node->board, b_goal)) {
            break;
        }
        
        // Get children of current node
        children = GetChildNodes(node);
        // Increment counter of nodes visited
        a_tmp->NodesVisited++;

        // Push child node queue into main queue to be processed
        PushQueue(&children, queue);
    }
    
    // End computation timer
    clock_t c_timer_end = clock();

    // Get the ComputationTime in milliseconds
    a_tmp->ComputationTime = (double)(c_timer_end - c_timer_begin) / CLOCKS_PER_SEC;

    // Get the path from the n_root node to the goal node
    Path* p_head = NULL;
    Path* p_tmp = NULL;

    while (node) {
        p_tmp = malloc(sizeof(Path));
        p_tmp->move = node->board->move;
        p_tmp->next = p_head;
        p_head = p_tmp;

        a_tmp->MovesPerformed++;
        node = node->parent;
    }

    // Remove the n_root node from the move counter
    --a_tmp->MovesPerformed;
    
    // Set the final path to the member variable of the algorithm path
    a_tmp->path = p_head;

    // Deallocate tree from memory
    FreeQueue(n_root);

    return a_tmp;
}

/* Uniform-Cost Search Implementation */
Algorithm* UCS(Board* b_init, Board* b_goal) {
    Algorithm* a_tmp = malloc(sizeof(Algorithm));
    a_tmp->MovesPerformed = 0;
    a_tmp->NodesVisited = 0;
    NodeQueue* queue = NULL;
    NodeQueue* children = NULL;
    Node* node = NULL;

    // Begin computation timer
    clock_t c_timer_begin = clock();
    
    // Push the first node into the queue
    PushNode(NewNode(0, b_init, NULL), &queue);

    // Stored in order to deallocate the tree from memory later
    Node* n_root = queue->qn_head->n_current;

    // While there are nodes in the queue to process
    while (queue->n_count > 0) {
        // Pop node from end of the queue
        node = PopNode(&queue);

        // Check if the tail node's board configuration is equal to the goal board
        if (AreBoardsEqual(node->board, b_goal)) {
            break;
        }

        // If not, get all child nodes of the current node
        children = GetChildNodes(node);
        // And increment the counter for nodes visisted
        a_tmp->NodesVisited++;

        // Push children to queue in order of depth
        PushQueue_Priority(&children, queue);
    }

    // End computation timer
    clock_t c_timer_end = clock();

    // Get the ComputationTime in seconds
    a_tmp->ComputationTime = (c_timer_end - c_timer_begin) / (CLOCKS_PER_SEC);

    // Get the path from the n_root node to the goal node
    Path* p_head = NULL;
    Path* p_tmp = NULL;

    // Iterate through the node and append relevant data
    while (node) {
        p_tmp = malloc(sizeof(Path));
        p_tmp->move = node->board->move;
        p_tmp->next = p_head;
        p_head = p_tmp;

        a_tmp->MovesPerformed++;
        node = node->parent;
    }

    // Remove the n_root node from the move counter
    --a_tmp->MovesPerformed;

    // Set the final path to the member variable of the algorithm path
    a_tmp->path = p_head;

    // Deallocate tree from memory
    FreeQueue(n_root);

    return a_tmp;
}

/** Simulated Annealing Implementation **/
double ManhattanDistance(Board* b, Board* b_goal) {
    double cost = 0;       // Total heuristic for given board

    int x_curr, y_curr;     // Coordinates of initial board
    int x_goal, y_goal;     // Coordinates of goal board
    int x_delta, y_delta;   // Change in these values

    // Iterate through each cell
    for (x_curr = 0; x_curr < 3; ++x_curr) {
        for (y_curr = 0; y_curr < 3; ++y_curr) {
            // Find corrisponding goal cell
            for (x_goal = 0; x_goal < 3; ++x_goal) {
                for (y_goal = 0; y_goal < 3; ++y_goal) {
                    if (b->config[x_curr][y_curr] == b_goal->config[x_goal][y_goal]) {
                        // 'Faux' absolute value of difference.
                        x_delta = (x_curr - x_goal < 0) ? x_goal - x_curr : x_curr - x_goal;
                        y_delta = (y_curr - y_goal < 0) ? y_goal - y_curr : y_curr - y_goal;
                        cost += x_delta + y_delta;
                    }
                }
            }
        }
    }

    return cost;
}

Node* NewNodeFromRandom(Node* n_curr) {
    bool success = false;
    
    Node* n_new = NULL;
    Board* b_new = NULL;
    while (!success) {
        // Create next node out of random move
        b_new = NewBoardIfValid(n_curr->board, rand() % 4);

        if (b_new != NULL) {
            // Check against our last move
            if (b_new->move == ABOVE && n_curr->board->move != BELOW) {
                success = true;
                break;
            }
            else if (b_new->move == BELOW && n_curr->board->move != ABOVE) {
                success = true;
                break;
            }
            else if (b_new->move == LEFT && n_curr->board->move != RIGHT) {
                success = true;
                break;
            }
            else if (b_new->move == RIGHT && n_curr->board->move != LEFT) {
                success = true;
                break;
            }
        } else {
            continue;
        }
    }

    n_new = NewNode(0, b_new, n_curr);
    return n_new;
}

double d_rand() {
    double r = (double)rand() / (double)RAND_MAX;
    return r;
}

/* Cooling Method - Linear Cooling */
void LinearCooling(double* T, double T_step) {
    *T -= T_step;
}

Algorithm* SA(Board* b_init, Board* b_goal) {
    Algorithm* a_tmp = malloc(sizeof(Algorithm));
    a_tmp->MovesPerformed = 0;
    a_tmp->NodesVisited = 0;

    // Initial node based on the initial board configuration
    Node* n_curr = NewNode(0, b_init, NULL);
    
    // List of nodes we will use in the end to display metrics
    NodeQueue* nq_final = NULL;
    PushNode(n_curr, &nq_final);

    // Begin computation timer
    clock_t c_timer_begin = clock();

    double T_max = 2.00;        // Our max temperature value
    double T_min = -2.00;       // Our min temperature value
    double T_step = 0.000001;   // Our 'Cooling Schedule'
    for (double T = T_max; T > T_min; LinearCooling(&T, T_step)) {
        printf("Temperature: %f \n", T);

        // Check if the current node's board configuration is equal to the goal board
        if (AreBoardsEqual(n_curr->board, b_goal)) {
            break;
        }

        // Create new board configuration (node) out of random move
        Node* n_new = NewNodeFromRandom(n_curr);
        PrintMove(n_new->board);
        PrintBoard(n_new->board);
        
        // Calculate heuristic values of each and get the difference
        double H_current = ManhattanDistance(n_curr->board, b_goal);
        double H_new = ManhattanDistance(n_new->board, b_goal);
        printf("Current Manhattan Distance: %f \n", H_current);
        printf("New Manhattan Distance: %f \n", H_new);

        double H_delta = H_new - H_current;

        // Determine if the swap between boards in 'good' or not
        printf("Probabilistic Function: %f\n", exp(-H_delta/T));
        if (H_delta < 0) {
            n_curr = n_new;
            PushNode(n_new, &nq_final);
        }
        else if (exp((-H_delta) / T) > d_rand()) {
            n_curr = n_new;
            PushNode(n_new, &nq_final);
        }
        else {
            continue;
        }
    }

    // End computation timer
    clock_t c_timer_end = clock();

    // Get the ComputationTime in seconds
    a_tmp->ComputationTime = (c_timer_end - c_timer_begin) / (CLOCKS_PER_SEC);

    // Get the number of moves performed from the final queue of nodes
    a_tmp->MovesPerformed = nq_final->n_count;

    // Final Output
    printf("\n==== Results ====================================\n");
    printf("Computation Time: %f Seconds\n", a_tmp->ComputationTime);
    printf("Moves Performed: %i\n", a_tmp->MovesPerformed);

    return a_tmp;
}


/* Prints a visual representation of the results of an algorithm */
void PrintAlgorithm(Algorithm* algo) {
    // Create Whitespace
    printf("\n");
    // Print Data
    printf("Computation Time: %f Seconds\n", algo->ComputationTime);
    printf("Nodes Visited: %i\n", algo->NodesVisited);
    printf("== Moves Performed: %i ==========================\n", algo->MovesPerformed);

    // Print Moves
    char* MoveStr[4] = { "ABOVE", "BELOW", "LEFT", "RIGHT" };
    int index = 1;
    for (algo->path = algo->path->next; algo->path; algo->path = algo->path->next, ++index) {
        if (algo->path->move == 0 || algo->path->move == 1)
            printf("Move %i: Moved element from %s the empty space\n", index, MoveStr[algo->path->move]);
        else
            printf("Move %i: Moved element %s of the empty space\n", index, MoveStr[algo->path->move]);
    }
    printf("=================================================\n");
}

void FreeAlgorithm(Algorithm** algo) {
    Path* p_next;

    while ((*algo)->path) {
        // Store next node in path
        p_next = (*algo)->path->next;
        // Deallocate from memory
        free((*algo)->path);
        // Point to next node in path
        (*algo)->path = p_next;
    }
    
    // Set path to NULL
    (*algo)->path = NULL;

    // Free algorithm object from memory
    free(*algo);
}