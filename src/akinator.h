#ifndef AKINATOR_H
#define AKINATOR_H

#include <stdio.h>
#include <sys/stat.h>
#include "tree.h"

// Special codes for 'unusual' sutuations to use as 
// return values or markers in functions.
enum ak_codes {
        AK_EXIT   = 0,
        AK_NORMAL = 1,
        AK_TRUE   = 2,
        AK_FALSE  = 3,
        AK_ERROR  = 4,
};

struct file_t {
        FILE  *stream = nullptr;
        struct stat stats = {};
};

// Creates a basic tree for akinator.
int
ak_start(tree_t *tree, FILE *stream);
// Builds tree from a given database file.
int
ak_restore(tree_t *tree, const char *filename);
// Saves tree to a database file.
int
ak_save(tree_t *tree, const char *filename);
// Guesses an object in a tree by asking questions to user.
void
ak_guess(tree_t *tree, const char *filename);
// Gives a definition of an object.
int
ak_define(tree_t *tree, FILE *stream);
// Compares two objects.
void
ak_compare(tree_t *tree, FILE *stream);
// Frees all allocated for nodes data memory.
void
ak_free(tree_t *tree, int pos);

#endif // AKINATOR_H

