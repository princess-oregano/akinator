#ifndef AKINATOR_H
#define AKINATOR_H

#include "tree.h"

const char *const ANS_AGREE = "yes";
const char *const ANS_DISAGREE = "no";

// Builds tree from a given database file.
void
ak_restore(char *filename);
// Finds an object in a tree.
void
ak_guess(tree_t *tree);
// Gives a definition of an object.
void
ak_define(tree_t *tree);
// Compares two objects.
void
ak_compare(tree_t *tree);
// Saves tree to a database file.
void
ak_save(tree_t *tree);

#endif // AKINATOR_H

