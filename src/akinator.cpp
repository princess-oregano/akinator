#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "akinator.h"
#include "tree.h"
#include "tree_dump.h"
#include "log.h"

size_t DATA_SIZE = 100;
size_t ANS_SIZE = 5;


// Inserts a node to a tree with given data.
static void
ak_insert(tree_t *tree, int pos)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        fprintf(stderr, "What did you mean?\n");

        char *data = (char *) calloc (DATA_SIZE, sizeof(char));

        getline(&data, &DATA_SIZE, stdin);
        data[strlen(data) - 1] = '\0';

        node_insert(tree, &tree->nodes[pos].right, tree->nodes[pos].data);
        node_insert(tree, &tree->nodes[pos].left, data);

        fprintf(stderr, "What property differs %s from %s\n",
        tree->nodes[tree->nodes[pos].left].data,
        tree->nodes[tree->nodes[pos].right].data);

        char *differ = (char *) calloc (DATA_SIZE, sizeof(char));

        getline(&differ, &DATA_SIZE, stdin);
        differ[strlen(differ) - 1] = '\0';
        tree->nodes[pos].data = differ;

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

// Prompts a question.
static bool
ak_try(tree_t *tree, int pos, FILE *stream)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        bool ret_val = false;

        fprintf(stream, "Is it %s?\n", tree->nodes[pos].data);

        char *answer = (char *) calloc(ANS_SIZE, sizeof(char));

        getline(&answer, &ANS_SIZE, stdin);
        answer[strlen(answer) - 1] = '\0';

        if (strcmp(answer, "yes") == 0)
                ret_val = true;
        else if (strcmp(answer, "no") == 0)
                ret_val = false;
        else
                fprintf(stderr, "Invalid input.\n");

        free(answer);

        log("Exiting %s.\n", __PRETTY_FUNCTION__);

        return ret_val;
}

// Asks a user question about an object to navigate the tree.
static void
ak_ask(tree_t *tree, int *pos, FILE *stream)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        assert(tree);

        if (ak_try(tree, *pos, stream))
                *pos = tree->nodes[*pos].left;
        else
                *pos = tree->nodes[*pos].right;

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

static bool
ak_take_guess(tree_t *tree, int *pos, FILE *stream)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        bool guessed = false;

        assert(tree);
        if (*pos < 0) {
                fprintf(stderr, "Invalid *pos value.\n");
                return false;
        }

        if (tree->nodes[*pos].left  == -1 &&
            tree->nodes[*pos].right == -1) {
                if (ak_try(tree, *pos, stderr)) {
                        fprintf(stream, "I knew that!\n");
                        guessed = true;
                } else {
                        ak_insert(tree, *pos);
                        fprintf(stderr, "Fine!\nI'll try better next time.\n");
                        *pos = tree->root;
                }
        } else {
                ak_ask(tree, pos, stream);
        }

        return guessed;

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

static void
ak_start(tree_t *tree)
{
        fprintf(stderr, "Enter the first property\n");

        char *data = (char *) calloc(100, sizeof(char));
        if (data == nullptr) {
                fprintf(stderr, "Couldn't allocate memory.\n");
        }

        getline(&data, &DATA_SIZE, stdin);
        data[strlen(data) - 1] = '\0';

        node_insert(tree, &tree->root, data);

        fprintf(stderr, "Enter first two objects.\n");

        char *obj1 = (char *) calloc(100, sizeof(char));
        if (obj1 == nullptr) {
                fprintf(stderr, "Couldn't allocate memory.\n");
        }

        getline(&obj1, &DATA_SIZE, stdin);
        obj1[strlen(obj1) - 1] = '\0';
        node_insert(tree, &tree->nodes[tree->root].left, obj1);

        char *obj2 = (char *) calloc(100, sizeof(char));
        if (obj2 == nullptr) {
                fprintf(stderr, "Couldn't allocate memory.\n");
        }

        getline(&obj2, &DATA_SIZE, stdin);
        obj2[strlen(obj2) - 1] = '\0';
        node_insert(tree, &tree->nodes[tree->root].right, obj2);

        include_graph(tree_graph_dump(tree));
}

// Frees all allocated for nodes data memory.
static void
ak_free(tree_t *tree, int pos)
{
        if (tree->nodes[pos].left != -1)
                ak_free(tree, tree->nodes[pos].left);

        if (tree->nodes[pos].right != -1)
                ak_free(tree, tree->nodes[pos].right);

        free(tree->nodes[pos].data);
}

void
ak_guess(tree_t *tree)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        assert(tree);

        ak_start(tree);

        int pos = tree->root;
        while(!ak_take_guess(tree, &pos, stderr)) {
                include_graph(tree_graph_dump(tree));
        }

        ak_free(tree, tree->root);

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

