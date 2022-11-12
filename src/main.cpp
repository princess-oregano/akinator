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
        if (open_save(&tree, &save) == AK_EXIT) {
                ak_free(&tree, tree.root);
                tree_dtor(&tree);

                close_save(&tree, &save);
                close_log();

                fprintf(stdout, "Мы ведь даже не начали...\n");
                return 0;
        }

        include_graph(tree_graph_dump(&tree));

        int mode = 0;
        while (mode != ANS_EXIT) {
                mode = get_choice();
                switch (mode) {
                        case MODE_GUESS:
                                ak_guess(&tree, save.filename);
                                ak_free(&tree, tree.root);
                                tree_dtor(&tree);
                                close_save(&tree, &save);
                                close_log();
                                return 0;
                                break;
                        case MODE_DEFINE:
                                ak_define(&tree, stdout);
                                break;
                        case MODE_COMPARE:
                                ak_compare(&tree, stdout);
                                break;
                        case MODE_EXIT:
                                break;
                }
        }
        
        include_graph(tree_graph_dump(&tree));

        ak_free(&tree, tree.root);
        tree_dtor(&tree);

        close_save(&tree, &save);
        close_log();

        return 0;
}

