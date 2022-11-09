#include <stdio.h>
#include <stdlib.h>
#include "akinator.h"
#include "tree_dump.h"
#include "tree.h"
#include "log.h"

int
main()
{
        open_log("log.html");
        tree_t tree {};

        tree_ctor(&tree, 100);

        ak_restore(&tree, "save.txt");

        include_graph(tree_graph_dump(&tree));

        tree_dtor(&tree);

        close_log();

        return 0;
}

