#ifndef UI_H
#define UI_H

#include <stdio.h>
#include "akinator.h"

struct game_save_t {
        char *filename = nullptr;
        FILE* stream = nullptr;
};

// Prints hello message.
void
print_hello();
// Opens save or creates a new one.
void
open_save(tree_t *tree, game_save_t *save);
// Closes save.
void
close_save(tree_t *tree, game_save_t *save);

#endif // UI_H

