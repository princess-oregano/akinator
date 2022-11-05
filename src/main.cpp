#include <stdio.h>
#include <stdlib.h>
#include "tree_dump.h"
#include "tree.h"
#include "log.h"

int
main()
{
        open_log("log.html");

        tree_t tree;
        int cap = 100;

        tree_ctor(&tree, cap);

        node_init(&tree, &tree.root, 3);
        node_init(&tree, &tree.nodes[tree.root].left, 4);
        node_init(&tree, &tree.nodes[tree.root].right, 2);
        node_init(&tree, &tree.nodes[tree.nodes[tree.root].right].left, 56);

        fprintf(stderr, "root = %d, root.left = %d, root.right = %d\n",
                tree.nodes[tree.root].data,
                tree.nodes[tree.nodes[tree.root].left].data,
                tree.nodes[tree.nodes[tree.root].right].data);

        tree_graph_dump(&tree);

        node_dtor(&tree, tree.nodes[tree.root].left);
        node_dtor(&tree, tree.nodes[tree.root].right);
        node_dtor(&tree, tree.root);

        free(tree.nodes);


        close_log();
        return 0;
}

