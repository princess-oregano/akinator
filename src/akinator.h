#ifndef AKINATOR_H
#define AKINATOR_H

#include <stdio.h>
#include <sys/stat.h>
#include "tree.h"

const char *const ANS_AGREE = "да";
const char *const ANS_DISAGREE = "нет";

struct file_t {
        FILE  *stream = nullptr;
        struct stat stats = {};
};

// Creates a basic tree for akinator.
void
ak_start(tree_t *tree, FILE *stream);
// Builds tree from a given database file.
void
ak_restore(tree_t *tree, const char *filename);
// Saves tree to a database file.
void
ak_save(tree_t *tree, const char *filename);
// Guesses an object in a tree by asking questions to user.
void
ak_guess(tree_t *tree, const char *filename);
// Gives a definition of an object.
void
ak_define(tree_t *tree, tree_data_t data, FILE *stream);
// Compares two objects.
void
ak_compare(tree_t *tree, char *obj1, char *obj2, FILE *stream);
// Frees all allocated for nodes data memory.
void
ak_free(tree_t *tree, int pos);

#endif // AKINATOR_H

