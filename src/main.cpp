#include <stdio.h>
#include "tree.h"

int
main()
{
        node_t root {};
        node_t node1 {};
        node_t node2 {};

        node_bound(&root.left, &node1);
        node_bound(&root.right, &node2);

        node_init(&node1, 3);
        node_init(&node2, 4);
        node_init(&root, 1);

        printf("root.left = %d; root.right = %d\n", root.left->data, root.right->data);
        printf("node1 = %d; node2 = %d\n", node1.data, node2.data);
        return 0;
}

