#ifndef AKINATOR_H
#define AKINATOR_H

#include <stdio.h>
#include <sys/stat.h>
#include "tree.h"

const char *const ANS_AGREE = "yes";
const char *const ANS_DISAGREE = "no";

struct file_t {
        FILE  *stream = nullptr;
        struct stat stats = {};
};

// Builds tree from a given database file.
void
ak_restore(tree_t *tree, char *filename);
// Saves tree to a database file.
void
ak_save(tree_t *tree, const char *filename);
// Guesses an object in a tree by asking questions to user.
void
ak_guess(tree_t *tree);
// Gives a definition of an object.
void
ak_define(tree_t *tree);
// Compares two objects.
void
ak_compare(tree_t *tree);

#endif // AKINATOR_H

