#include <stdio.h>
#include <stdlib.h>
#include "tree_dump.h"
#include "tree.h"
#include "log.h"

static void
init_rec(tree_t *tree, int *parent, data_t data, int depth)
{
        if (depth == 0)
                return;

        tree_graph_dump(tree);

        node_insert(tree, parent, data);

        init_rec(tree, &tree->nodes[*parent].left,  data, depth - 1);
        init_rec(tree, &tree->nodes[*parent].right, data, depth - 1);
}

int
main()
{
        open_log("log.html");

        tree_t tree;
        int cap = 100;

        tree_ctor(&tree, cap);

        init_rec(&tree, &tree.root, 5, 3);

        tree_graph_dump(&tree);

        node_remove(&tree, &tree.nodes[tree.root].left);

        tree_graph_dump(&tree);

        tree_dtor(&tree);

        close_log();

        return 0;
}

