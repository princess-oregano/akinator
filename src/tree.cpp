#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "tree.h"

static void
tree_resize(node_t **nodes, int cap)
{
        if (cap <= 0) {
                fprintf(stderr, "Invalid cap.\n");
        }
        node_t *tmp_nodes = (node_t *) realloc(*nodes, 
                            (size_t) cap * sizeof(node_t));

        if (tmp_nodes == nullptr) {
                fprintf(stderr, "Error: Couldn't allocate memory "
                                "for nodes.\n");
                return;
        } 

        *nodes = tmp_nodes;
}

void
tree_ctor(tree_t *tree, int cap)
{
        tree_resize(&tree->nodes, cap);

        for (int i = tree->cap; i < cap; i++) {
                memset(&tree->nodes[i].data, DATA_POISON, sizeof(data_t));
                tree->nodes[i].left = nullptr;
                tree->nodes[i].right = nullptr;
                tree->nodes[i].next = i + 1; 
        }

        tree->nodes[cap - 1].next = -1;

        tree->cap = cap;
        tree->free = 0;
}

void
node_init(tree_t *tree, node_t **parent, data_t data)
{
        assert(tree);
        
        if (tree->free == tree->cap - 1)
                tree_resize(&tree->nodes, tree->cap * 2);  

        node_bound(parent, &tree->nodes[tree->free]);

        tree->nodes[tree->free].data = data;
        tree->free = tree->nodes[tree->free].next;
}

void
node_bound(node_t **parent, node_t *node)
{
        assert(parent);
        assert(node);

        if (*parent != nullptr) {
                fprintf(stderr, "Warning: pointer is already initialized.\n"
                                "Bounding may lead to loss of data.\n");
        }

        *parent = node;
}

void
node_dtor(tree_t *tree, node_t *node)
{
        assert(tree);
        assert(node);

        memset(&node->data, DATA_POISON, sizeof(data_t));

        node->left = nullptr;
        node->right = nullptr;

        node->next = tree->free;
        tree->free = (int) (node - tree->nodes);
}

