#pragma once

/** 
* The Node data-structure represents a search tree.
* It contains metadata and objects representing:
*   * The depth of the tree. (how far the node is from the root node of the tree)
*   * A pointer to the board configuration at this node
*   * A pointer to the parent node (the previous node)
*   * A queue of nodes representing the child nodes of this node.
*/
typedef struct Node Node;
struct Node {
    unsigned int depth;             
    Board* board;       
    Node* parent;       
    NodeQueue* children; 
};

/**
* Creates a new node and returns it's address in memory
*/
Node* NewNode(unsigned int depth, Board* board, Node* parent) {
    Node* newNode = malloc(sizeof(Node));
    if (newNode) {
        newNode->depth = depth;
        newNode->board = board;
        newNode->parent = parent;
        newNode->children = NULL;
    }
    return newNode;
}

/**
* Retrieves the queue of child nodes of the given node address.
* It keeps track of the parent node's last move and checks to see which board configurations
* can be made in a a given direction.
*/
NodeQueue* GetChildNodes(Node* n_parent) {
    NodeQueue* nq_children = NULL; // Node queue representing valid child nodes of n_parent
    Board* b_tmp = NULL;           // Placeholder board used with valid node.
    Node* n_child = NULL;          // The child node to be created. 

    // Check if a valid move can be made given the parent's last move. 
    if (n_parent->board->move != BELOW && (b_tmp = NewBoardIfValid(n_parent->board, ABOVE))) {
        // Create a child node one level deeper than the parent node.
        n_child = NewNode(n_parent->depth + 1, b_tmp, n_parent);
        // Append this node to the parent node.
        PushNode(n_child, &n_parent->children);
        // Also append this node to the list of children relevant to this node.
        PushNode(n_child, &nq_children);
    }
    if (n_parent->board->move != ABOVE && (b_tmp = NewBoardIfValid(n_parent->board, BELOW))) {
        // Create a child node one level deeper than the parent node.
        n_child = NewNode(n_parent->depth + 1, b_tmp, n_parent);
        // Append this node to the parent node.
        PushNode(n_child, &n_parent->children);
        // Also append this node to the list of children relevant to this node.
        PushNode(n_child, &nq_children);
    }
    if (n_parent->board->move != RIGHT && (b_tmp = NewBoardIfValid(n_parent->board, LEFT))) {
        // Create a child node one level deeper than the parent node.
        n_child = NewNode(n_parent->depth + 1, b_tmp, n_parent);
        // Append this node to the parent node.
        PushNode(n_child, &n_parent->children);
        // Also append this node to the list of children relevant to this node.
        PushNode(n_child, &nq_children);
    }
    if (n_parent->board->move != LEFT && (b_tmp = NewBoardIfValid(n_parent->board, RIGHT))) {
        // Create a child node one level deeper than the parent node.
        n_child = NewNode(n_parent->depth + 1, b_tmp, n_parent);
        // Append this node to the parent node.
        PushNode(n_child, &n_parent->children);
        // Also append this node to the list of children relevant to this node.
        PushNode(n_child, &nq_children);
    }

    return nq_children;
}

// TODO: Might move this
/* Used to deallocate memory of a NodeQueue */
void FreeQueue(Node* n_current) {
    if (n_current->children == NULL) {
        free(n_current->board);
        free(n_current);
        return;
    }

    QueueNode* gn_current = n_current->children->qn_head;
    QueueNode* qn_next;

    while (gn_current) {
        qn_next = gn_current->qn_next;
        FreeQueue(gn_current->n_current);
        gn_current = qn_next;
    }

    free(n_current->children);
    free(n_current);
}

int GetDepthCost(Node* node) {
    return node->depth;
}