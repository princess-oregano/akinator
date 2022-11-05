#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include "tree_dump.h"
#include "log.h"

const int FILENAME_SIZE = 100;

// Stream of dump output.
FILE *DMP_STREAM = nullptr;
char PNG_FILENAME[FILENAME_SIZE] = {};

// Opens a graphviz file to write dump to.
static void
open_graph_dump()
{
        DIR* dir = opendir("dmp");
        if (dir) {
                closedir(dir);
        } else if (ENOENT == errno) {
                mkdir("dmp", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        } else {
                fprintf(stderr, "Couldn't make a directory for dumps.\n");
        }

        char filename[] = "dmp/DUMP_XXXXXX";

        DMP_STREAM = fdopen(mkstemp(filename), "w");
        if (strlen(filename) >= FILENAME_SIZE) {
                fprintf(stderr, "Filename is too long.\n");
                return;
        }

        memcpy(PNG_FILENAME, filename, strlen(filename) + 1);
}

// Calls system function, but is used like printf-like functions.
static void
system_wformat(const char *format, ...)
{
        va_list arglist;

        va_start(arglist, format);

        char *cmd = (char *) calloc(255, sizeof(char));
        if (cmd == nullptr) {
                fprintf(stderr, "Couldn't allocate memory for system cmd.\n");
                va_end(arglist);
                return;
        }

        vsprintf(cmd, format, arglist);
        system(cmd);

        va_end(arglist);

        free(cmd);
}

// Genetares .png image from given dot code.
static void
generate_graph()
{
        char *new_filename = (char *) calloc (FILENAME_SIZE, sizeof(char));
        if (new_filename == nullptr) {
                fprintf(stderr, "Couldn't allocate memory.\n");
                return;
        }

        new_filename = strcat(new_filename, PNG_FILENAME);
        new_filename = strcat(new_filename, ".png");

        fprintf(stderr, "%s -> %s\n", PNG_FILENAME, new_filename);

        system_wformat("%s %s %s %s", "dot -Tpng", PNG_FILENAME, ">", new_filename);

        include_graph(new_filename);

        free(new_filename);
}

static void
node_graph_dump(tree_t *tree, int curr, int prev)
{
        static int node_count = 1;

        fprintf(DMP_STREAM,
        "node%d [label = \"%d\", shape = rect]\n",
        node_count, tree->nodes[curr].data);

        fprintf(DMP_STREAM,
        "node%d -> node%d [arrowhead = none]\n", prev, node_count++);

        prev = node_count - 1;

        if (tree->nodes[curr].left != -1)
                node_graph_dump(tree, tree->nodes[curr].left, prev);

        if (tree->nodes[curr].right != -1)
                node_graph_dump(tree, tree->nodes[curr].right, prev);
}

void
tree_graph_dump(tree_t *tree)
{
        open_graph_dump();

        fprintf(DMP_STREAM,
        "digraph G {\n"
        "ranksep = 1.5\n"
        "graph [dpi = 100]\n"
        "splines = ortho\n");

        fprintf(DMP_STREAM,
        "node0[label = \"root\", shape = rect]\n");

        node_graph_dump(tree, tree->root, 0);

        fprintf(DMP_STREAM, "}\n");

        fclose(DMP_STREAM);

        generate_graph();
}

