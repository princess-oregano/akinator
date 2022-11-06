#include <stdio.h>
#include <stdlib.h>
#include "akinator.h"
#include "tree.h"
#include "log.h"

int
main()
{
        open_log("log.html");
        tree_t tree {};

        tree_ctor(&tree, 100);

        ak_guess(&tree);

        tree_dtor(&tree);

        close_log();

        return 0;
}

