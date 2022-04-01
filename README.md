# 8-Puzzle
The 8-Puzzle problem statement is as follows:

> Given some random 3x3 board of cells, each with a corresponding number attached and one empty cell, find the sequence of moves required to reach a specificed *goal* board.

In order to solve the 8-puzzle problem using Simulated Annealing, we must start with an initial board state $B_{init}$. We also start with a set max temperature $T_{max}$. We can iterate over a set number of steps until either:
- The goal state has been reached
- The temperature $T = T_{min}$
- We have run out of computational power

Within the main loop, we will observe the hueristic cost of each cell in the grid and set this as the overall hueristic cost of the current state. We will then get a new state that is created by making a random valid move and compare its heuristic cost with the current state's cost.$$\Delta H = H(B_{new}) - H(B_{current})$$
- If the cost of the new state is less than the cost of the current state, we set the new state as our current state.
- If the cost of the new state is greater than the cost of the current state:
	- Is our probability function $e^{\frac{-\Delta H}{T}} > rand(0,1)$? If so, set $B_{current} = B_{new}$.
	- If not, $B_{current}$ remains and we repeat the loop again.

For each iteration, we must lower the temperature $T$ by some amount. This can be linearly or be some other degree we decide upon. When the value of $T$ is large, as it will be initially, many 'bad' moves can be made in order to let the board *explore*. This prevents us from finding a goal that is our *local minimum* (the minimum amount of moves to achieve a 'local' goal, such as a board configuration that is close to but not our actual goal configuration) and instead allows us to eventually find our *global minimum* (the actual goal configuration).

## Implementation
The program is implemented using the **C** programming language for full control over the data structures and pointers required.

#### 1. Create a Random Initial State
In this initial stage we are creating a random board configuration that our algorithm will attempt to solve. Here, we can implement a method for making sure the board is solveable, if that is something we want to do.

```cpp
/* The data-structure representing a board configuration. 
* It also contains an enum representing the last move used to create this configuration.
* It also contains an array representing the current configuration.
*/
typedef struct Board {
    Move move;           
    int config[3][3];
} Board;

/* Create a new board with a random initial configuration */
void NewBoard(Board* b, bool isGoal, bool isRandom) {
    int initArr[] = { 8, 2, 3, 1, 4, 6, 5, 0, 7 };
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

```

#### 2. Define our Base Cases
When iterating through our loop, we must define some **Base Cases** in which our loop will exit. 
- The first being when our temperature $T = T_{min}$. This can be in the skeleton of our loop as long as we define a certain $T_{step}$ value that will determine how much $T$ decrements for each iteration.
- The second being when $B_{init} = B_{goal}$, which will mean we have successfully found our goal state. This could be done by evaluating if the cost of $H(B_{current}) = 0$
- The last will be when we run out of *time*. This is to preserve computational power and keep the program from crashing from using too much memory. 

```cpp
// Iterate until tempurature reaches its minimum value using linear cooling method
for (double T = T_max; T > T_min; LinearCooling(&T, T_step)) {
        // Check if the current board configuration is equal to the goal board
        if (AreBoardsEqual(n_curr->board, b_goal)) {
            break;
        }
    
    ...
```

#### 2. Choose a Cooling Function
For this example I will be using a starting tempurate $T_{max} = +2$. I will be attempting to use a **Linear Cooling Function**, which will simply decrease the temperature by some constant $T_{step}$ until it reaches $T_{min} = -2$. $$\Delta T = T - T_{step}$$Another possible strategy is a **Random Cooling Function**, which decreases temperature $T_{max}$ by some constant $T_{step}$ and multiplies it by a random value from (0-1) to get our new tempurature.$$\Delta T=rand(0,1) \times (T-T_{step})$$
```cpp
/* Cooling Method - Linear Cooling */
void LinearCooling(double* T, double T_step) {
    *T -= T_step;
}

/* Cooling Method - Random Cooling */
void LinearCooling(double* T, double T_step) {
    *T = d_rand() * (T - T_step);
}
```

#### 3.  Choose a Cost Function $H()$
To determine wether or not the generated state is better than the previous one, we must write a cost function. For this heuristic function, we will use the **Manhattan Distance** as the output of this cost function. This function looks at the 'straight-line' distance from each of the cells in the initial state to each of the corresponding cells in the goal state.

We can do this by, for each cell in the grid, applying the following formula: $$H(x,y) = \vert x_{initial} - x_{goal}\vert + \vert y_{initial} - y_{goal}\vert$$
we could also attempt this using **Euclidean Distance**, which looks at the distance from each cell *diagonally* from it's corresponding goal cell. The formula for this is as follows: $$H(x,y) = \sqrt{(\vert x_{initial} - x_{goal}\vert)^2+(\vert y_{initial} - y_{goal}\vert)^2}$$
```cpp
/** Manhattan Distance - Simulated Annealing Implementation **/
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
```

#### 4. Applying the Probability Function $e^{\frac{-\Delta H}{T}}$
This function is at the heart of the **Simulated Annealing** algorithm. When deciding whether or not to make the randomly selected board our new board, we first observe whether or not the hueristic value (manhattan distance) of our new board is greater than that of our current board.  

This is done by observing whether or not the value of $(\Delta H = H_{new} - H_{current}) < 0$. If this is true, we assign the new board as our current board any move on to the next iteration.

If the hueristic value of our new board is greater than that of our current board, we must use our probability function: $$e^{\frac{-\Delta H}{T}} > rand(0,1)$$This function is important because it uses the changing value of $T$, our tempurature, to decide if a *worse move* can still be made.

```cpp
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
```

#### 4. Define our Data Structures
For this project, we will have **Nodes** that contain the following data:
- The node's board configuration
- The heurisitc cost of this node's board
- A pointer to it's parent node
- A pointer to it's child node

We will use a **NodeQueue** object to store the list of moves performed.

The **Board** data structure will simply contain a 3x3, 2d-array of values representing the cells in our grid. It will also contain the *last move used to create this configuration*.

When we iterate through our main loop, at each iteration we want to select a new 'random' move. This move will be 'psuedo-random' as it is designed with the restriction that it cannot be the same move that it just performed (a cell that moves down cannot move up in the next turn). This helps us avoid getting stuck in loops and find the goal faster.

```cpp
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

```
