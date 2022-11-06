#ifndef TREE_H
#define TREE_H

typedef char* data_t;

const char* const TREE_DATA_POISON = "Nothing.";

struct node_t {
        data_t data {};
        int left = -1;
        int right = -1;
        int next_free = -1;
};

struct tree_t {
        node_t *nodes = nullptr;
        int root = 0;
        int cap = 0;
        int free = 0;
};

// Constructs tree with 'cap' free unbounded nodes.
void
tree_ctor(tree_t *tree, int cap);
// Insert a node with given data to given position.
void
node_insert(tree_t *tree, int *parent, data_t data);
// Bounds two nodes.
void                                  // Think pls.
node_bound(int *parent, int node);
// Finds node in a tree.
void
node_find(node_t *node);
// Removes node and all children of it.
void
node_remove(tree_t *tree, int *pos);
// Destructs tree.
void
tree_dtor(tree_t *tree);

#endif // TREE_H

