#ifndef UI_H
#define UI_H

#include <stdio.h>
#include "akinator.h"

const char * const EXIT = "я ухожу";
const char *const AGREE = "да";
const char *const DISAGREE = "нет";

enum ans_t {
        ANS_FALSE = 0,
        ANS_TRUE = 1,
        ANS_LONG = 2,
        ANS_EXIT = 3,
};

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
// Asks user a quastion.
int
ask(char **buf);

#endif // UI_H

