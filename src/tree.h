#ifndef TREE_H
#define TREE_H

typedef int data_t;

struct node_t {
        data_t data {};
        node_t *left = nullptr;
        node_t *right = nullptr;
};

struct tree_t {
        node_t *root;
};

// Constructs node.
void 
node_ctor(node_t *node);
// Initializes node.
void
node_init(node_t *node, data_t data);
// Bounds two nodes.
void
node_bound(node_t **parent, node_t *node);
// Unbounds two nodes.
void
node_unbound(node_t **parent, node_t *node);
// Finds node in a tree.
void
node_find(node_t *node);
// Dectructs node.
void
node_dtor(node_t *node);

#endif // TREE_H

