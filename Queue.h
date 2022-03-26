#pragma once

/* Forward Declarations */
typedef struct Node Node;
typedef struct NodeQueue NodeQueue;

/** A node to be processed in the NodeQueue.
* It holds a pointer to the current node being proccessed
* It also holds pointers to the next and previous nodes in the queue
*/
typedef struct QueueNode {
    Node* n_current;
    struct QueueNode* qn_prev; 
    struct QueueNode* qn_next;
} QueueNode;

/** The queue in which nodes will be processed, this will be used when traversing the tree. 
* It holds a counter representing the number of nodes in the queue
* It also holds pointers to the first and last nodes currently in the queue
*/
struct NodeQueue {
    unsigned int n_count;    
    QueueNode* qn_head;            
    QueueNode* qn_tail;            
};

/* Pushes a node to the queue. */
void PushNode(Node* node, NodeQueue** const nq) {
    // Placeholder queuenode
    QueueNode* qn = malloc(sizeof(QueueNode));

    // Set the placehold  QueueNode to the node we're pushing
    qn->n_current = node;

    // If the NodeQueue exists but is empty...
    if (*nq && (*nq)->n_count == 0) {
        // Set the head and tail of the queue to the QueueNode
        (*nq)->qn_head = qn;
        (*nq)->qn_tail = qn;
        // Set the next and previous to null
        qn->qn_next = NULL;
        qn->qn_prev = NULL;
        // Incremement the node counter
        (*nq)->n_count++;
        return;
    }
    // Or if the NodeQueue does not exist
    if (*nq == NULL) {
        // Create it!
        *nq = malloc(sizeof(NodeQueue));
        // Initialize it's values and set the tail to the QueueNode
        (*nq)->n_count = 0;
        (*nq)->qn_head = NULL;
        (*nq)->qn_tail = qn;
    }
    // If it does exist and it is not empty...
    else {
        (*nq)->qn_head->qn_prev = qn;
    }
    // Push the node to the front of the queue
    qn->qn_next = (*nq)->qn_head;
    qn->qn_prev = NULL;
    (*nq)->qn_head = qn;

    // Increment node counter
    (*nq)->n_count++;

    return;
}

/* Pops a node from the queue. */
Node* PopNode(NodeQueue** const nq) {
    // If the queue is already empty, return.
    if (!*nq || (*nq)->n_count == 0) return NULL;

    // The node to be popped
    Node* n_pop = (*nq)->qn_tail->n_current;
    // The node 'behind' it
    QueueNode* qn_prev = (*nq)->qn_tail->qn_prev;

    // Deallocate node from memory
    free((*nq)->qn_tail);
    
    // If the queue now has only 1 element...
    if ((*nq)->n_count == 1) {
        // Set the head to NULL
        (*nq)->qn_head = NULL;
    }
    else {
        // Set the next QueueNode to NULL
        qn_prev->qn_next = NULL;
    }

    // Set the tail node of the NodeQueue to the previous QueueNode
    (*nq)->qn_tail = qn_prev;
    // Decrement the node counter
    (*nq)->n_count--;

    // Return a pointer to the popped node
    return n_pop;
}

/** Pushes a queue of nodes to be processed (nq_source) into a destination queue (nq_dest). */
void PushQueue(NodeQueue** nq_source, NodeQueue* nq_dest) {
    // If the head of the source node doesn't exist, or it is equal to the destination node, return.
    if (!(*nq_source)->qn_head || (*nq_source)->qn_head == nq_dest->qn_head) return;
    

    // If the sourc queue is empty...
    if (!nq_dest->n_count) {
        // set the destination queue's front and back pointers to the source's.
        nq_dest->qn_head = (*nq_source)->qn_head;
        nq_dest->qn_tail = (*nq_source)->qn_tail;
    }
    else {
        // Connect the source queue to the destination queue
        (*nq_source)->qn_tail->qn_next = nq_dest->qn_head;
        nq_dest->qn_head->qn_prev = (*nq_source)->qn_tail;
        nq_dest->qn_head = (*nq_source)->qn_head;
    }

    // Increment the node counter by the number of nodes in the source queue.
    nq_dest->n_count += (*nq_source)->n_count;

    // Deallocate the source queue from memory
    free(*nq_source);
    *nq_source = NULL;
}

int GetDepthCost(Node*);
/* Pushes a queue of nodes to a destination queue based ordererd by the depth (path cost from the current node to the root node). */
void PushQueue_Priority(NodeQueue** nq_source, NodeQueue* nq_dest) {
    if (!*nq_source || !nq_dest || !(*nq_source)->qn_head || (*nq_source)->qn_head == nq_dest->qn_head) {
        return;
    }

    // If the queue is currently empty
    if (!nq_dest->n_count) {
        PushNode(PopNode(nq_source), &nq_dest); // Push the tail node of the source queue into the destination queue
    } 

    QueueNode* qn_source;   // Source node
    QueueNode* qn_dest;     // Destination node
    Node* n_tmp;            // Placeholder node;

    while ((qn_source = (*nq_source)->qn_head)) {
        qn_dest = nq_dest->qn_head;

        // Iterate until the depth (cost) of the source node is less than that of the node in the destination queue
        while (qn_dest && GetDepthCost(qn_source->n_current) < GetDepthCost(qn_dest->n_current)) {
            qn_dest = qn_dest->qn_next;
        }

        // Set a placeholder QueueNode equal to the next node in the source queue
        QueueNode* qn_tmp = qn_source->qn_next;

        // If the destination QueueNode does not exist...
        if (!qn_dest) {
            // Append the source QueueNode to the end of the destination node
            nq_dest->qn_tail->qn_next = qn_source;
            qn_source->qn_prev = nq_dest->qn_tail;
            qn_source->qn_next = NULL;
            nq_dest->qn_tail = qn_source;
        } else {
            // If the previous queuenode exists in the desination queue...
            if (qn_dest->qn_prev) {
                // Swap these QueueNodes
                qn_source->qn_prev = qn_dest->qn_prev;
                qn_source->qn_next = qn_dest;
                qn_dest->qn_prev->qn_next = qn_source;
                qn_dest->qn_prev = qn_source;
            } else {
                // Append the destination node to the source queue
                qn_source->qn_next = nq_dest->qn_head;
                qn_source->qn_prev = NULL;
                nq_dest->qn_head->qn_prev = qn_source;
                nq_dest->qn_head = qn_source;
            }
        }

        // Set the placeholder queuenode as the head of the source queue 
        (*nq_source)->qn_head = qn_tmp;
        // Decrement the source node counter
        (*nq_source)->n_count--;
        // Increment the destination node counter
        nq_dest->n_count++;
    }

    // Deallocate Memory
    free(*nq_source);
    *nq_source = NULL;
}