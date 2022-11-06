#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "akinator.h"
#include "tree.h"

size_t DATA_SIZE = 100;
size_t ANS_SIZE = 5;


// Inserts a node to a tree with given data.
static void
ak_insert(tree_t *tree, int pos)
{
        fprintf(stderr, "What did you mean?\n"); 

        char *data = (char *) calloc (DATA_SIZE, sizeof(char));

        getline(&data, &DATA_SIZE, stdin);

        node_insert(tree, &tree->nodes[pos].right, tree->nodes[pos].data);
        node_insert(tree, &tree->nodes[pos].left, data);

        fprintf(stderr, "What property differs %s from %s\n", 
        tree->nodes[tree->nodes[pos].left].data, 
        tree->nodes[tree->nodes[pos].right].data);

        getline(&data, &DATA_SIZE, stdin);
        tree->nodes[pos].data = data;
}

// Prompts a question.
static bool
ak_try(tree_t *tree, int pos, FILE *stream)
{
        bool ret_val = false;

        fprintf(stream, "Is it %s?\n", tree->nodes[pos].data);

        char *answer = (char *) calloc(ANS_SIZE, sizeof(char));

        getline(&answer, &ANS_SIZE, stdin);

        if (strcmp(answer, "yes") == 0)
                ret_val = true;
        else if (strcmp(answer, "no") == 0)
                ret_val = false;
        else
                fprintf(stderr, "Invalid input.\n");

        free(answer);
        return ret_val;
}

// Asks a user question about an object to navigate the tree.
static void
ak_ask(tree_t *tree, int *pos, FILE *stream)
{
        assert(tree);

        if (ak_try(tree, *pos, stream))
                *pos = tree->nodes[*pos].left;
        else
                *pos = tree->nodes[*pos].right;
}

static bool
ak_take_guess(tree_t *tree, int pos, FILE *stream)
{
        bool guessed = false;

        assert(tree);
        if (pos < 0) {
                fprintf(stderr, "Invalid pos value.\n");
                return false;
        }

        if (tree->nodes[pos].left  == -1 &&
            tree->nodes[pos].right == -1) {
                if (ak_try(tree, pos, stderr)) {
                        fprintf(stream, "I knew that!\n");
                        guessed = true;
                } else {
                        ak_insert(tree, pos);
                }
        } else {
                ak_ask(tree, &pos, stream);
        }

        return guessed;
}

void
ak_guess(tree_t *tree)
{
        assert(tree);

        int pos = tree->root;

        while(!ak_take_guess(tree, pos, stderr)) 
                ;
}
