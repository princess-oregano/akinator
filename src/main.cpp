#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

int
main()
{
        tree_t tree;
        int cap = 100;

        tree_ctor(&tree, cap);
        
        node_init(&tree, &tree.root, 3);
        node_init(&tree, &tree.root->left, 4);
        node_init(&tree, &tree.root->right, 2);

        fprintf(stderr, "root = %d, root.left = %d, root.right = %d\n", 
                tree.root->data, tree.root->left->data, tree.root->right->data);

        node_dtor(&tree, tree.root->left);
        node_dtor(&tree, tree.root->right);
        node_dtor(&tree, tree.root);

        free(tree.nodes);

        return 0;
}

