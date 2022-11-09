#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "tree.h"
#include "log.h"

char TREE_DATA_POISON[] = "nothing";

static void
tree_resize(tree_t *tree, int new_cap)
{
        log("Entered %s.\n", __PRETTY_FUNCTION__);

        if (new_cap <= 0) {
                fprintf(stderr, "Invalid cap.\n");
                log("Error: Invalid capacity.\n"
                    "Exiting %s.\n", __PRETTY_FUNCTION__);

                return;
        }

        tree_node_t *tmp_nodes = (tree_node_t *) realloc(tree->nodes,
                            (size_t) new_cap * sizeof(tree_node_t));

        if (tmp_nodes == nullptr) {
                fprintf(stderr, "Error: Couldn't allocate memory "
                                "for nodes.\n");
                log("Error: Couldn't allocate memory for nodes.\n"
                    "Exiting %s.\n", __PRETTY_FUNCTION__);
                return;
        }

        tree->nodes = tmp_nodes;

        for (int i = tree->cap; i < new_cap; i++) {
                tree->nodes[i].data = TREE_DATA_POISON;
                tree->nodes[i].left = -1;
                tree->nodes[i].right = -1;
                tree->nodes[i].next_free = i + 1;
        }

        tree->nodes[tree->cap].next_free = tree->cap + 1;
        tree->cap = new_cap;

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

void
tree_ctor(tree_t *tree, int cap)
{
        log("Entered %s.\n", __PRETTY_FUNCTION__);

        tree_resize(tree, cap);

        tree->free = 0;

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

void
node_insert(tree_t *tree, int *parent, tree_data_t data)
{
        log("Entered %s.\n", __PRETTY_FUNCTION__);

        assert(tree); 
        assert(parent);
        assert(*parent >= -1);

        tree->nodes[tree->free].data = data;
        node_bound(parent, tree->free);
        tree->free = tree->nodes[tree->free].next_free;

        if (tree->free >= tree->cap) {
                log("Free = %d, capacity = %d\n", tree->free, tree->cap);
                log("Resizing...\n");
                tree_resize(tree, tree->cap * 2);
        }

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

void
node_bound(int *parent, int node)
{
        log("Entered %s.\n", __PRETTY_FUNCTION__);

        assert(parent);

        if (*parent != -1 && node != 0) {
                log("Warning: pointer is already initialized.\n"
                    "Bounding may lead to loss of data.n");
                fprintf(stderr, "Warning: pointer is already initialized.\n"
                                "Bounding may lead to loss of data.\n");
        }

        *parent = node;

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

void
node_remove(tree_t *tree, int *pos)
{
        log("Entered %s.\n", __PRETTY_FUNCTION__);

        assert(tree);

        tree->nodes[*pos].data = TREE_DATA_POISON;

        if (tree->nodes[*pos].left != -1)
                node_remove(tree, &tree->nodes[*pos].left);

        if (tree->nodes[*pos].right != -1)
                node_remove(tree, &tree->nodes[*pos].right);

        tree->nodes[*pos].left = -1;
        tree->nodes[*pos].right = -1;

        tree->nodes[*pos].next_free = tree->free;
        tree->free = *pos;

        *pos = -1;

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

void
tree_dtor(tree_t *tree)
{
        node_remove(tree, &tree->root);
        free(tree->nodes);

        tree->root = -1;
        tree->cap = -1;
        tree->free = -1;
}

