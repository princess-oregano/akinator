#include <stdio.h>
#include <assert.h>
#include "akinator.h"

// Asks a user question about an object.
void
ak_ask(tree_t *tree, int *pos)
{
        assert(tree);
        assert(pos);

        printf("%s?\n", tree->nodes[*pos].data);
}

void
ak_guess(tree_t *tree)
{
        assert(tree);

        int pos = 0;
        for (pos = tree->root; &tree->nodes[pos] != nullptr; ) {
                ak_ask(tree, &pos);
        }
}
