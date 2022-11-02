#ifndef TREE_H
#define TREE_H

typedef int data_t;

const int CAP = 100;
const data_t DATA_POISON = 0xBE;
const int    NEXT_POISON = 0xAE;

struct node_t {
        data_t data {};
        node_t *left = nullptr;
        node_t *right = nullptr;
        int next = 0;
};

struct tree_t {
        node_t *nodes = nullptr;
        node_t *root = nullptr;
        int cap = 0;
        int free = 0;
};

// Constructs tree with 'cap' free unbounded nodes.
void
tree_ctor(tree_t *tree, int cap);
// Initializes node.
void
node_init(tree_t *tree, node_t **parent, data_t data);
// Bounds two nodes.
void
node_bound(node_t **parent, node_t *node);
// Finds node in a tree.
void
node_find(node_t *node);
// Dectructs node.
void
node_dtor(tree_t *tree, node_t *node);

#endif // TREE_H

