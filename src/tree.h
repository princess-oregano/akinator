#ifndef TREE_H
#define TREE_H

typedef int data_t;

const int CAP = 100;
const data_t DATA_POISON = 0xBE;
const int    NEXT_POISON = 0xAE;

struct node_t {
        data_t data {};
        int left = -1;
        int right = -1;
        int next = -1;
};

struct tree_t {
        node_t *nodes = nullptr;
        int root = -1;
        int cap = 0;
        int free = 0;
};

// Constructs tree with 'cap' free unbounded nodes.
void
tree_ctor(tree_t *tree, int cap);
// Initializes node.
void
node_init(tree_t *tree, int *ptr, data_t data);
// Bounds two nodes.
void
node_bound(int *parent, int node);
// Finds node in a tree.
void
node_find(node_t *node);
// Dectructs node.
void
node_dtor(tree_t *tree, int pos);

#endif // TREE_H

