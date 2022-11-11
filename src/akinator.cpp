#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <sys/mman.h>
#include "stack.h"
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

        fprintf(stream, "И что же Вы имели в виду?\n");

        char *data = nullptr;
        size_t data_size = 0;

        data_size = (size_t) getline(&data, &data_size, stdin);
        data[data_size - 1] = '\0';

        fprintf(stream, "Какое свойство отличает %s от %s?\n",
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
                        fprintf(stream, "Я знала это с самого начала.\n");
                        guessed = true;
                } else {
                        ak_insert(tree, *pos, stdout);
                        fprintf(stderr, "Ладно...\n"
                                        "Я запомню.\n");
                        *pos = tree->root;
                }
        } else {
                ak_ask(tree, pos, stream);
        }

        log("Exiting %s.\n", __PRETTY_FUNCTION__);

        return guessed;
}

void
ak_free(tree_t *tree, int pos)
{
        if (tree->nodes[pos].left != -1)
                ak_free(tree, tree->nodes[pos].left);

        if (tree->nodes[pos].right != -1)
                ak_free(tree, tree->nodes[pos].right);

        free(tree->nodes[pos].data);
}

void
ak_guess(tree_t *tree, const char *filename)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        assert(tree);

        int pos = tree->root;
        while(!ak_take_guess(tree, &pos, stderr)) {
                include_graph(tree_graph_dump(tree));
        }

        ak_save(tree, filename);

        ak_free(tree, tree->root);

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

////////////////////////////////END_GUESSER/////////////////////////////////////

//////////////////////////////////SAVE_RESTORE//////////////////////////////////

void
ak_start(tree_t *tree, FILE *stream)
{
        fprintf(stream, "Скажи мне первое свойство\n");

        char *data = nullptr;
        size_t data_size = 0;

        data_size = (size_t) getline(&data, &data_size, stdin);
        data[data_size - 1] = '\0';

        node_insert(tree, &tree->root, data);

        fprintf(stream, "Какой объект обладает этим свойством?\n");

        char *obj1 = nullptr;
        size_t obj1_size = 0;

        obj1_size = (size_t) getline(&obj1, &obj1_size, stdin);
        obj1[obj1_size - 1] = '\0';

        node_insert(tree, &tree->nodes[tree->root].left, obj1);

        fprintf(stream, "А какой нет?\n");

        char *obj2 = nullptr;
        size_t obj2_size = 0;

        obj2_size = (size_t) getline(&obj2, &obj2_size, stdin);
        obj2[obj2_size - 1] = '\0';

        node_insert(tree, &tree->nodes[tree->root].right, obj2);

        include_graph(tree_graph_dump(tree));
}

static int
get_delim_buf(char **line, int delim, char *buffer)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        int count = 0;
        for (count = 0; buffer[count] != delim; count++)
                ;

        *line = (char *) calloc (count + 1, sizeof(char));
        if (*line == nullptr) {
                log("Couldn't allocate memory.\n");
                return 0;
        }

        for (int i = 0; i < count; i++) {
                (*line)[i] = buffer[i];
        }

        return count;

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

static void
get_file(const char *filename, file_t *file, const char *mode)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        if ((file->stream = fopen(filename, mode)) == nullptr) {
                fprintf(stderr, "Error: Couldn't open %s.\n", filename);

                return;
        }

        if (stat(filename, &file->stats) != 0) {
                fprintf(stderr, "Error: Coudn't get stats of %s.\n", filename);
                return;
        }

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

static void
read_file(char **buffer, file_t *file)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        assert(file);

        *buffer = (char *) mmap(NULL, (size_t) file->stats.st_size, PROT_READ,
                               MAP_PRIVATE, fileno(file->stream), 0);

        if (*buffer == MAP_FAILED) {
                fprintf(stderr, "Error: Couldn't allocate memory.\n");
                return;
        }

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

static void
print_node(tree_t *tree, int pos, FILE *stream, int level)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        assert(tree);
        assert(stream);

        if (pos < 0)
                return;

        for (int i = 0; i < level; i++)
                fprintf(stream, "        ");

        level++;

        fprintf(stream, "{\'%s\'", tree->nodes[pos].data);

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

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
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

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

// Restores node from description from the buffer.
static void
build_node(tree_t *tree, char *buffer, int *pos)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        assert(tree);
        assert(buffer);

        char *data = nullptr;
        char ch = '\0';
        static int i = 0;

        for ( ; isspace(ch = buffer[i]); i++)
                ;

        if (ch == '{') {
                if ((ch = buffer[++i]) != '\'') {
                        log("Invalid usage.\n");
                        return;
                }

                i++;
                i += get_delim_buf(&data, '\'', &buffer[i]) + 1;

                node_insert(tree, pos, data);

                build_node(tree, buffer, &tree->nodes[*pos].left);
                build_node(tree, buffer, &tree->nodes[*pos].right);

                for ( ; isspace(ch = buffer[i]); i++)
                        ;

                if (ch == '}') {
                        i++;
                        return;
                }
        }
        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

void
ak_restore(tree_t *tree, const char *filename)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        file_t file;
        get_file(filename, &file, "r");

        char *buffer = nullptr;
        read_file(&buffer, &file);

        build_node(tree, buffer, &tree->root);

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

///////////////////////////////END_SAVE_RESTORE/////////////////////////////////

/////////////////////////////////DEFINER////////////////////////////////////////

// Finds leaf with data and fills definition stack.
static void
ak_find(tree_t *tree, char *data, int pos, stack_t *def, bool *found)
{
        if (tree->nodes[pos].left == -1 &&
            tree->nodes[pos].right == -1) {
                if (strcmp(tree->nodes[pos].data, data) == 0) {
                        stack_push(def, pos);
                        *found = true;
                }

                return;
        }

        if (*found == true)
                return;

        ak_find(tree, data, tree->nodes[pos].left, def, found);
        ak_find(tree, data, tree->nodes[pos].right, def, found);
        if (*found == true)
                stack_push(def, pos);
}

static void
ak_print_def(tree_t *tree, char *data, stack_t *def, FILE *stream)
{
        if (def->size == 0)
                return;
        int pos = 0;
        stack_pop(def, &pos);
        fprintf(stream, "%s:\n", data);
        while (def->size > 0) {
                int new_pos = 0;
                stack_pop(def, &new_pos);
                if (tree->nodes[pos].right == new_pos)
                        fprintf(stream, "не ");

                fprintf(stream, "%s\n", tree->nodes[pos].data);

                pos = new_pos;
        }
}

void
ak_define(tree_t *tree, char *data, FILE *stream)
{
        stack_t def = {};
        stack_ctor(&def, 20, VAR_INFO(def));

        bool found = false;
        ak_find(tree, data, tree->root, &def, &found);

        ak_print_def(tree, data, &def, stream);

        stack_dtor(&def);
}

///////////////////////////////END_DEFINER//////////////////////////////////////

/////////////////////////////////COMPARATOR/////////////////////////////////////

static void
ak_print_comp(tree_t *tree, stack_t *def1, stack_t *def2,
              char *obj1, char *obj2, FILE *stream)
{
        int pos1 = 0;
        int pos2 = 0;

        stack_pop(def1, &pos1);
        stack_pop(def2, &pos2);

        fprintf(stream, "Общие свойства:\n");

        int new_pos1 = 0;
        int new_pos2 = 0;
        while (def1->size > 0 && def2->size > 0) {
                stack_pop(def1, &new_pos1);
                stack_pop(def2, &new_pos2);

                if (new_pos1 != new_pos2)
                        break;

                if (tree->nodes[pos1].right == new_pos1)
                        fprintf(stream, "не ");

                fprintf(stream, "%s\n", tree->nodes[pos1].data);

                pos1 = new_pos1;
                pos2 = new_pos2;
        }

        stack_push(def1, new_pos1);
        stack_push(def2, new_pos2);
        stack_push(def1, pos1);
        stack_push(def2, pos2);

        fprintf(stream, "Различные свойства:\n");

        ak_print_def(tree, obj1, def1, stream);
        ak_print_def(tree, obj2, def2, stream);

}

void
ak_compare(tree_t *tree, char *obj1, char *obj2, FILE *stream)
{
        stack_t def1 = {};
        stack_t def2 = {};
        stack_ctor(&def1, 10, VAR_INFO(def1));
        stack_ctor(&def2, 10, VAR_INFO(def2));

        bool found = false;
        ak_find(tree, obj1, tree->root, &def1, &found);
        found = false;
        ak_find(tree, obj2, tree->root, &def2, &found);

        ak_print_comp(tree, &def1, &def2, obj1, obj2, stream);

        stack_dtor(&def1);
        stack_dtor(&def2);
}

//////////////////////////////END_COMPARATOR////////////////////////////////////

