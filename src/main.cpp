#include <stdio.h>
#include <stdlib.h>
#include "akinator.h"
#include "tree_dump.h"
#include "tree.h"
#include "log.h"
#include "UI.h"

int
main()
{
        open_log("log.html");
        game_save_t save = {};
        tree_t tree {};

        tree_ctor(&tree, 100);

        print_hello();
        open_save(&tree, &save);

        include_graph(tree_graph_dump(&tree));

        ak_define(&tree, (char *) "полина", stdout);

        ak_free(&tree, tree.root);
        tree_dtor(&tree);

        close_save(&tree, &save);
        close_log();

        return 0;
}

