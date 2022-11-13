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
#include "UI.h"

//////////////////////////////////GUESSER///////////////////////////////////////

// Inserts a node to a tree with given data.
static int
ak_insert(tree_t *tree, int pos, FILE *stream)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        int ans = 0;

        fprintf(stream, "И что же Вы имели в виду?\n");

        char *data = nullptr;

        while ((ans = ask(&data)) != ANS_LONG) {
                switch (ans) {
                        case ANS_TRUE:
                                free(data);
                                fprintf(stream, "Да?..\n");
                                break;
                        case ANS_FALSE:
                                free(data);
                                fprintf(stream, "Нет?..\n");
                                break;
                        case ANS_EXIT:
                                free(data);
                                fprintf(stream, "Я обиделась!\n");
                                return AK_EXIT;
                        case ANS_LONG:
                                break;
                        default:
                                assert(0 && "Invalid ans value.");
                }
        }

        fprintf(stream, "Какое свойство отличает %s от %s?\n",
                         data, tree->nodes[pos].data);

        char *differ = nullptr;

        while ((ans = ask(&differ)) != ANS_LONG) {
                switch (ans) {
                        case ANS_TRUE:
                                free(differ);
                                fprintf(stream, "Да?..\n");
                                break;
                        case ANS_FALSE:
                                free(differ);
                                fprintf(stream, "Нет?..\n");
                                break;
                        case ANS_EXIT:
                                free(differ);
                                free(data);
                                fprintf(stream, "Я обиделась!\n");
                                return AK_EXIT;
                        case ANS_LONG:
                                break;
                        default:
                                assert(0 && "Invalid ans value.");
                }
        }

        if (node_insert(tree, &tree->nodes[pos].right, tree->nodes[pos].data) != ERR_NO_ERR || 
            node_insert(tree, &tree->nodes[pos].left, data) != ERR_NO_ERR) {
                free(differ);
                free(data);
                fprintf(stream, "Спать хочется...\n");
                return AK_ERROR;
        }

        tree->nodes[pos].data = differ;

        log("Exiting %s.\n", __PRETTY_FUNCTION__);

        return AK_NORMAL;
}

// Asks user to confirm(or not) the final guess.
static int
ak_ask_final(tree_t *tree, int pos, FILE *stream)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        int ret_val = -1;

        char *data = nullptr;
        int ans = ANS_LONG;

        while (ans == ANS_LONG) {
                fprintf(stream, "Это %s?\n", tree->nodes[pos].data);
                ans = ask(&data);
                switch (ans) {
                        case ANS_TRUE:
                                ret_val = AK_TRUE;
                                free(data);
                                break;
                        case ANS_FALSE:
                                ret_val = AK_FALSE;
                                free(data);
                                break;
                        case ANS_EXIT:
                                fprintf(stream, "Вы просто не хотите "
                                                "признавать мою правоту!\n");
                                free(data);
                                return AK_EXIT;
                                break;
                        case ANS_LONG:
                                fprintf(stream, "Неужели сложно ответить "
                                                "да или нет?\n");
                                free(data);
                                break;
                        default:
                                assert(0 && "Invalid ans value.");
                }
        }

        log("Exiting %s.\n", __PRETTY_FUNCTION__);

        return ret_val;
}

// Asks a user question about an object to navigate the tree.
static int
ak_navigate(tree_t *tree, int *pos, FILE *stream)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        fprintf(stream, "Это %s?\n", tree->nodes[*pos].data);

        while (true) {
                char *data = nullptr;
                int ans = ask(&data);
                switch (ans) {
                        case ANS_TRUE:
                                *pos = tree->nodes[*pos].left;
                                free(data);
                                return AK_TRUE;
                                break;
                        case ANS_FALSE:
                                *pos = tree->nodes[*pos].right;
                                free(data);
                                return AK_FALSE;
                                break;
                        case ANS_EXIT:
                                fprintf(stream, "Я обиделась!\n");
                                free(data);
                                return AK_EXIT;
                                break;
                        case ANS_LONG:
                                fprintf(stream, "Неужели сложно ответить "
                                                "да или нет?\n");
                                free(data);
                                fprintf(stream, "Это %s?\n", 
                                                tree->nodes[*pos].data);
                                break;
                        default:
                                assert(0 && "Invalid ans value.");
                }
        }

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

static int
ak_take_final_guess(tree_t *tree, int *pos, FILE *stream)
{
        int insert_ret = 0;
        switch (ak_ask_final(tree, *pos, stdout)) {
                case AK_TRUE:
                        fprintf(stream, "Я знала это с самого начала.\n");
                        fprintf(stream, "Мне понравилась наша игра.\n"
                                        "Попробуем еще раз.\n");
                        *pos = tree->root;
                        break;
                case AK_FALSE:
                        insert_ret = ak_insert(tree, *pos, stdout);
                        if (insert_ret == AK_EXIT)
                                return AK_EXIT;
                        if (insert_ret == AK_ERROR)
                                return AK_ERROR;
                
                        fprintf(stderr, "Ладно...\n"
                                        "Я запомню.\n");
                        *pos = tree->root;
                        break;
                case AK_EXIT:
                        return AK_EXIT;
                        break;
                default:
                        assert(0 && "Invalid ask_final value.");
        }

        return AK_NORMAL;
}

static int
ak_take_guess(tree_t *tree, int *pos, FILE *stream)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        assert(tree);
        if (*pos < 0) {
                fprintf(stderr, "Invalid *pos value.\n");
                return AK_ERROR;
        }

        if (tree->nodes[*pos].left  == -1 &&
            tree->nodes[*pos].right ==-1) {
                if (ak_take_final_guess(tree, pos, stream) == AK_EXIT)
                        return AK_EXIT;
        } else {
                if (ak_navigate(tree, pos, stream) == AK_EXIT)
                        return AK_EXIT;
        }

        log("Exiting %s.\n", __PRETTY_FUNCTION__);

        return AK_NORMAL;
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

        while(ak_take_guess(tree, &pos, stderr) != AK_EXIT)
                ;

        ak_save(tree, filename);

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
}

////////////////////////////////END_GUESSER/////////////////////////////////////

//////////////////////////////////SAVE_RESTORE//////////////////////////////////

int
ak_start(tree_t *tree, FILE *stream)
{
        fprintf(stream, "Скажите мне первое свойство\n");

        int ans = 0;
        char *data = nullptr;
        while ((ans = ask(&data)) != ANS_LONG) {
                switch (ans) {
                        case ANS_TRUE:
                                free(data);
                                fprintf(stream, "Да?..\n");
                                break;
                        case ANS_FALSE:
                                free(data);
                                fprintf(stream, "Нет?..\n");
                                break;
                        case ANS_EXIT:
                                fprintf(stream, "Я обиделась!\n");
                                free(data);
                                return AK_EXIT;
                        case ANS_LONG:
                                break;
                        default:
                                assert(0 && "Invalid ans value.");
                }
        }

        fprintf(stream, "Какой объект обладает этим свойством?\n");

        char *obj1 = nullptr;
        while ((ans = ask(&obj1)) != ANS_LONG) {
                switch (ans) {
                        case ANS_TRUE:
                                free(obj1);
                                fprintf(stream, "Да?..\n");
                                break;
                        case ANS_FALSE:
                                free(obj1);
                                fprintf(stream, "Нет?..\n");
                                break;
                        case ANS_EXIT:
                                fprintf(stream, "Я обиделась!\n");
                                free(data);
                                free(obj1);
                                return AK_EXIT;
                        case ANS_LONG:
                                break;
                        default:
                                assert(0 && "Invalid ans value.");
                }
        }

        fprintf(stream, "А какой нет?\n");

        char *obj2 = nullptr;
        while ((ans = ask(&obj2)) != ANS_LONG) {
                switch (ans) {
                        case ANS_TRUE:
                                free(obj2);
                                fprintf(stream, "Да?..\n");
                                break;
                        case ANS_FALSE:
                                free(obj2);
                                fprintf(stream, "Нет?..\n");
                                break;
                        case ANS_EXIT:
                                fprintf(stream, "Я обиделась!\n");
                                free(data);
                                free(obj1);
                                free(obj2);
                                return AK_EXIT;
                        case ANS_LONG:
                                break;
                        default:
                                assert(0 && "Invalid ans value.");
                }
        }

        if (node_insert(tree, &tree->root, data) != ERR_NO_ERR ||
            node_insert(tree, &tree->nodes[tree->root].left, obj1) != ERR_NO_ERR ||
            node_insert(tree, &tree->nodes[tree->root].right, obj2) != ERR_NO_ERR) {
                free(data);
                free(obj1);
                free(obj2);
                fprintf(stream, "Спать хочется...\n");
                return AK_ERROR;
        }

        include_graph(tree_graph_dump(tree));

        return AK_NORMAL;
}

static int
get_delim_buf(char **line, int delim, char *buffer)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        size_t count = 0;
        for ( ; buffer[count] != delim; count++)
                ;

        *line = (char *) calloc (count + 1, sizeof(char));
        if (*line == nullptr) {
                log("Couldn't allocate memory.\n");
                return 0;
        }

        memcpy(*line, buffer, count);

        log("Exiting %s.\n", __PRETTY_FUNCTION__);
        
        return (int) count;
}

static int
get_file(const char *filename, file_t *file, const char *mode)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        if ((file->stream = fopen(filename, mode)) == nullptr) {
                log("Error: Couldn't open %s.\n", filename);

                return AK_ERROR;
        }

        if (stat(filename, &file->stats) != 0) {
                log("Error: Coudn't get stats of %s.\n", filename);
                return AK_ERROR;
        }

        log("Exiting %s.\n", __PRETTY_FUNCTION__);

        return AK_NORMAL;
}

static int
read_file(char **buffer, file_t *file)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        assert(file);

        *buffer = (char *) mmap(NULL, (size_t) file->stats.st_size, PROT_READ,
                               MAP_PRIVATE, fileno(file->stream), 0);

        if (*buffer == MAP_FAILED) {
                fprintf(stderr, "Error: Couldn't allocate memory.\n");
                return AK_ERROR;
        }

        log("Exiting %s.\n", __PRETTY_FUNCTION__);

        return AK_NORMAL;
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

int
ak_save(tree_t *tree, const char *filename)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        file_t file;
        if (get_file(filename, &file, "w") == AK_ERROR)
                return AK_ERROR;

        setvbuf(file.stream, nullptr, _IOFBF, (size_t) file.stats.st_blksize);

        print_node(tree, tree->root, file.stream, 0);

        fclose(file.stream);

        log("Exiting %s.\n", __PRETTY_FUNCTION__);

        return AK_NORMAL;
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
        static int i = 0; // questionable

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

int
ak_restore(tree_t *tree, const char *filename)
{
        log("Entering %s.\n", __PRETTY_FUNCTION__);

        file_t file;
        if (get_file(filename, &file, "r") == AK_ERROR)
                return AK_ERROR;

        char *buffer = nullptr;
        if (read_file(&buffer, &file) == AK_ERROR) {
                free(buffer);
                return AK_ERROR;
        }

        build_node(tree, buffer, &tree->root);

        log("Exiting %s.\n", __PRETTY_FUNCTION__);

        return AK_NORMAL;
}

///////////////////////////////END_SAVE_RESTORE/////////////////////////////////

/////////////////////////////////DEFINER////////////////////////////////////////

// Finds leaf with data and fills definition stack.
static bool
ak_find(tree_t *tree, char *data, int pos, stack_t *def, bool *found, int *err)
{
        if (*err != 0) {
                *found = false;
                return *found;
        }

        if (tree->nodes[pos].left == -1 &&
            tree->nodes[pos].right == -1) {
                if (strcmp(tree->nodes[pos].data, data) == 0) {
                        *err = (int) stack_push(def, pos);
                        *found = true;
                }

                return *found;
        }

        if (*found == true)
                return *found;

        ak_find(tree, data, tree->nodes[pos].left,  def, found, err);
        ak_find(tree, data, tree->nodes[pos].right, def, found, err);
        if (*found == true)
                *err = (int) stack_push(def, pos);

        return *found;
}

static int
ak_print_def(tree_t *tree, char *data, stack_t *def, FILE *stream)
{
        if (def->size == 0)
                return AK_NORMAL;
        int pos = 0;
        if (stack_pop(def, &pos) != 0)
                return AK_ERROR;
        fprintf(stream, "%s ", data);
        while (def->size > 0) {
                int new_pos = 0;
                if (stack_pop(def, &new_pos) != 0)
                        return AK_ERROR;
                if (tree->nodes[pos].right == new_pos)
                        fprintf(stream, "не ");

                fprintf(stream, "%s, ", tree->nodes[pos].data);

                pos = new_pos;
        }
        fprintf(stream, "\b\b.\n");

        return AK_NORMAL;
}

int
ak_define(tree_t *tree, FILE *stream)
{
        stack_t def = {};
        if (stack_ctor(&def, 20, VAR_INFO(def)) != 0)
                return AK_ERROR;

        char *data = nullptr;
        fprintf(stream, "Что мне описать?\n");

        int ans = 0;
        while (ans != ANS_LONG) {
                ans = ask(&data);
                switch(ans) {
                        case ANS_TRUE:
                                free(data);
                                fprintf(stream, "Да?..\n");
                                break;
                        case ANS_FALSE:
                                free(data);
                                fprintf(stream, "Нет?..\n");
                                break;
                        case ANS_EXIT:
                                free(data);
                                fprintf(stream, "Хочу играть!\n");
                                break;
                        case ANS_LONG:
                                break;
                        default:
                                assert(0 && "Invalid ans value.");
                }
        }

        bool found = false;
        int err = 0;
        if (ak_find(tree, data, tree->root, &def, &found, &err) == false) {
                if (err != 0)
                        fprintf(stream, "Немного спать захотелось...\n");
                else
                        fprintf(stream, "Это... Превзошло мои ожидания.\n"
                                        "Я не знаю, что Вы имели в виду.\n");

                free(data);
                stack_dtor(&def);

                return AK_NORMAL;
        }

        ak_print_def(tree, data, &def, stream);

        free(data);

        stack_dtor(&def);

        return AK_NORMAL;
}

///////////////////////////////END_DEFINER//////////////////////////////////////

/////////////////////////////////COMPARATOR/////////////////////////////////////

static int
ak_print_comp(tree_t *tree, stack_t *def1, stack_t *def2, 
              char *obj1, char *obj2, FILE *stream)
{
        int pos1 = tree->root;
        int pos2 = tree->root;

        if (stack_pop(def1, &pos1) != 0 ||
            stack_pop(def2, &pos2) != 0)
                return AK_ERROR;

        int new_pos1 = -1;
        int new_pos2 = -1;
        if (stack_pop(def1, &new_pos1) != 0 ||
            stack_pop(def2, &new_pos2) != 0)
                return AK_ERROR;

        if (stack_push(def1, new_pos1) != 0 ||
            stack_push(def2, new_pos2) != 0)
                return AK_ERROR;

        if (new_pos1 != new_pos2)
                fprintf(stream, "Совсем не похожи!\n");
        else {
                fprintf(stream, "Каждый из них\n");
                while (def1->size > 0 && def2->size > 0) {
                        if (stack_pop(def1, &new_pos1) != 0 ||
                            stack_pop(def2, &new_pos2) != 0)
                                return AK_ERROR;

                        if (new_pos1 != new_pos2)
                                break;

                        if (tree->nodes[pos1].right == new_pos1)
                                fprintf(stream, "не ");

                        fprintf(stream, "%s\n", tree->nodes[pos1].data);

                        pos1 = new_pos1;
                        pos2 = new_pos2;
                }
                if (stack_push(def1, new_pos1) != 0 ||
                    stack_push(def2, new_pos2) != 0||
                    stack_push(def1, pos1) != 0 ||
                    stack_push(def2, pos2) != 0)
                        return AK_ERROR;

                fprintf(stream, "Но при этом\n");

        }
        
        if (ak_print_def(tree, obj1, def1, stream) != AK_NORMAL ||
            ak_print_def(tree, obj2, def2, stream) != AK_NORMAL)
                return AK_ERROR;

        return AK_NORMAL;
}

int
ak_compare(tree_t *tree, FILE *stream)
{
        stack_t def1 = {};
        stack_t def2 = {};
        if (stack_ctor(&def1, 10, VAR_INFO(def1)) != 0 ||
            stack_ctor(&def2, 10, VAR_INFO(def2)) != 0)
                return AK_ERROR;

        char *obj1 = nullptr;
        char *obj2 = nullptr;

        fprintf(stream, "Кого мне сравнить?\n");

        int ans = 0;
        while (ans != ANS_LONG) {
                ans = ask(&obj1);
                switch(ans) {
                        case ANS_TRUE:
                                free(obj1);
                                fprintf(stream, "Да?..\n");
                                break;
                        case ANS_FALSE:
                                free(obj1);
                                fprintf(stream, "Нет?..\n");
                                break;
                        case ANS_EXIT:
                                free(obj1);
                                fprintf(stream, "Хочу играть!\n");
                                break;
                        case ANS_LONG:
                                break;
                        default:
                                assert(0 && "Invalid ans value.");
                }
        }

        bool found = false;
        int err = 0;
        if (ak_find(tree, obj1, tree->root, &def1, &found, &err) == false) {
                if (err != 0)
                        fprintf(stream, "Немного спать захотелось...\n");
                else
                        fprintf(stream, "Это... Превзошло мои ожидания.\n"
                                        "Я не знаю, что Вы имели в виду.\n");
                
                free(obj1);
                stack_dtor(&def1);
                stack_dtor(&def2);

                return AK_NORMAL;
        }

        fprintf(stream, "И с кем?\n");

        ans = 0;
        while (ans != ANS_LONG) {
                ans = ask(&obj2);
                switch(ans) {
                        case ANS_TRUE:
                                free(obj2);
                                fprintf(stream, "Да?..\n");
                                break;
                        case ANS_FALSE:
                                free(obj2);
                                fprintf(stream, "Нет?..\n");
                                break;
                        case ANS_EXIT:
                                free(obj2);
                                fprintf(stream, "Хочу играть!\n");
                                break;
                        case ANS_LONG:
                                break;
                        default:
                                assert(0 && "Invalid ans value.");
                }
        }

        found = false;
        err = 0;
        if (ak_find(tree, obj2, tree->root, &def2, &found, &err) == false) {
                free(obj1);
                free(obj2);
                stack_dtor(&def1);
                stack_dtor(&def2);

                if (err != 0) {
                        fprintf(stream, "Немного спать захотелось...\n");
                        return AK_ERROR;
                }
                else {
                        fprintf(stream, "Это... Превзошло мои ожидания.\n"
                                        "Я не знаю, что Вы имели в виду.\n");
                        return AK_NORMAL;
                }
        }

        if (strcmp(obj1, obj2) == 0) {
                fprintf(stream, "Это одно и то же!\n");
        } else {
                ak_print_comp(tree, &def1, &def2, obj1, obj2, stream);
        }

        free(obj1);
        free(obj2);

        stack_dtor(&def1);
        stack_dtor(&def2);

        return AK_NORMAL;
}

//////////////////////////////END_COMPARATOR////////////////////////////////////

