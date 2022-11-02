#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tree.h"

void 
node_ctor(node_t *node)
{
        if (node != nullptr) {
                fprintf(stderr, "Warning: pointer to node "
                                "is already initialized\n");
        }

        node_t *tmp_node = (node_t *) calloc(1, sizeof(node_t));
        if (tmp_node == nullptr) {
                fprintf(stderr, "Couldn't allocate memory for node.\n");
                return;
        }

        node = tmp_node;
}

void
node_init(node_t *node, data_t data)
{
        assert(node);

        node->data = data;
}

void
node_bound(node_t **parent, node_t *node)
{
        assert(node);

        *parent = node;
}

void
node_unbound(node_t **parent, node_t *node)
{
        assert(node);

        *parent = nullptr;
}

void
node_dtor(node_t *node)
{
        assert(node);

        free(node);
}

