#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/mman.h>
#include "akinator.h"
#include "tree.h"
#include "tree_dump.h"
#include "log.h"

//////////////////////////////////GUESSER///////////////////////////////////////

// Inserts a node to a tree with given data.
static void
ak_insert(tree_t *tree, int pos, FILE *stream)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        fprintf(stream, "Что Вы имели в виду?\n");

        char *data = nullptr;
        size_t data_size = 0;

        data_size = (size_t) getline(&data, &data_size, stdin);
        data[data_size - 1] = '\0';

        fprintf(stream, "Какое свойство отличает %s от %s\n",
                         data, tree->nodes[pos].data);

        char *differ = nullptr;
        size_t differ_size = 0;

        differ_size = (size_t) getline(&differ, &differ_size, stdin);
        differ[differ_size - 1] = '\0';

        node_insert(tree, &tree->nodes[pos].right, tree->nodes[pos].data);
        node_insert(tree, &tree->nodes[pos].left, data);
        tree->nodes[pos].data = differ;

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

// Prompts a question.
static bool
ak_try(tree_t *tree, int pos, FILE *stream)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        bool ret_val = false;

        fprintf(stream, "Это %s?\n", tree->nodes[pos].data);

        char *answer = nullptr;
        size_t answer_size = 0;

        answer_size = (size_t) getline(&answer, &answer_size, stdin);
        answer[answer_size - 1] = '\0';

        if (strcmp(answer, ANS_AGREE) == 0)
                ret_val = true;
        else if (strcmp(answer, ANS_DISAGREE) == 0)
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
                if (ak_try(tree, *pos, stdout)) {
                        fprintf(stream, "Я знала!\n");
                        guessed = true;
                } else {
                        ak_insert(tree, *pos, stdout);
                        fprintf(stderr, "Ну и ладно...\n"
                                        "В следующий раз постараюсь лучше...\n");
                        *pos = tree->root;
                }
        } else {
                ak_ask(tree, pos, stream);
        }

        log("Exiting %s.\n", __PRETTY_FUNCTION__);

        return guessed;
}

static void
ak_start(tree_t *tree, FILE *stream)
{
        fprintf(stream, "Введите первое свойство\n");

        char *data = nullptr;
        size_t data_size = 0;

        data_size = (size_t) getline(&data, &data_size, stdin);
        data[data_size - 1] = '\0';

        node_insert(tree, &tree->root, data);

        fprintf(stream, "Введите объект, соответствущий свойству\n");

        char *obj1 = nullptr;
        size_t obj1_size = 0;

        obj1_size = (size_t) getline(&obj1, &obj1_size, stdin);
        obj1[obj1_size - 1] = '\0';

        node_insert(tree, &tree->nodes[tree->root].left, obj1);

        fprintf(stream, "Введите объект, НЕ соответствущий свойству\n");

        char *obj2 = nullptr;
        size_t obj2_size = 0;

        obj2_size = (size_t) getline(&obj2, &obj2_size, stdin);
        obj2[obj2_size - 1] = '\0';

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

        ak_start(tree, stdout);

        int pos = tree->root;
        while(!ak_take_guess(tree, &pos, stderr)) {
                include_graph(tree_graph_dump(tree));
        }

        ak_save(tree, "save.txt");

        ak_free(tree, tree->root);

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

////////////////////////////////END_GUESSER/////////////////////////////////////

//////////////////////////////////SAVE_RESTORE//////////////////////////////////

static void
get_file(const char *filename, file_t *file, const char *mode)
{
        if ((file->stream = fopen(filename, mode)) == nullptr) {
                fprintf(stderr, "Error: Couldn't open %s.\n", filename);

                return;
        }

        if (stat(filename, &file->stats) != 0) {
                fprintf(stderr, "Error: Coudn't get stats of %s.\n", filename);
                return;
        }
}

static void
read_file(char *buffer, file_t *file)
{
        assert(buffer);
        assert(file);

        buffer = (char *) mmap(NULL, (size_t) file->stats.st_size, PROT_READ,
                               MAP_PRIVATE, fileno(file->stream), 0);

        if (buffer == MAP_FAILED) {

                fprintf(stderr, "Error: Couldn't allocate memory.\n");
                return;
        }
}

static void
print_node(tree_t *tree, int pos, FILE *stream, int level)
{
        assert(tree);
        assert(stream);

        if (pos < 0)
                return;

        for (int i = 0; i < level; i++)
                fprintf(stream, "        ");

        level++;

        fprintf(stream, "{%s", tree->nodes[pos].data);

        if (tree->nodes[pos].left  == -1 &&
            tree->nodes[pos].right == -1) {
                fprintf(stream, "}\n");
        } else {
                fprintf(stream, "\n");
                print_node(tree, tree->nodes[pos].left,  stream, level);
                print_node(tree, tree->nodes[pos].right, stream, level);
                for (int i = 0; i < level - 1; i++)
                        fprintf(stream, "        ");
                fprintf(stream, "}\n");
        }
}

void
ak_save(tree_t *tree, const char *filename)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        file_t file;
        get_file(filename, &file, "w");

        setvbuf(file.stream, nullptr, _IOFBF, (size_t) file.stats.st_blksize);

        print_node(tree, tree->root, file.stream, 0);

        fclose(file.stream);
}

void
af_restore(tree_t *tree, const char *filename)
{
        file_t file;
        get_file(filename, &file, "r");

        char *buffer = nullptr;
        read_file(buffer, &file);
}

///////////////////////////////END_SAVE_RESTORE/////////////////////////////////
