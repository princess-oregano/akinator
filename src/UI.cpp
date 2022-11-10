#include <stdio.h>
#include <stdlib.h>
#include "akinator.h"
#include "UI.h"

void
print_hello()
{
        printf(
        "                                       ;\\                        \n"
        "                                      _' \\_                      \n"
        " Приветствую.                       ,' '  '`.                     \n"
        " Меня зовут Баст, мудрейшая        ;,)       \\                   \n"
        " из кошек. Загадай мне            /          :                    \n"
        " любой oбъект и я его отгадаю,    (_         :                    \n"
        " или, может быть, чему-то          `--.       \\                  \n"
        " научусь.                             /        `.                 \n"
        "                                     ;           `.               \n"
        "                                    /              `.             \n"
        "                                    :                 `.          \n"
        "                                    :                   \\        \n"
        "                                     \\                   \\      \n"
        "                                      ::                 :        \n"
        "                                      || |               |        \n"
        "                                      || |`._            ;        \n"
        "                                     _;; ; __`._,       (________ \n"
        "                                   ((__/(_____(______,'______(___)\n");

        printf("Возможно, мы раньше играли в такую игру.\n"
               "Как мы ее тогда назвали?\n");
}

void
open_save(tree_t *tree, game_save_t *save)
{
        char *filename = nullptr;
        size_t n = 0;
        n = getline(&filename, &n, stdin);
        filename[n - 1] = '\0';
        FILE *stream = fopen(filename, "r");
        if (stream == nullptr) {
                printf("Я не помню такой игры.\n");
                printf("Давай попробуем ее начать.\n");
                stream = fopen(filename, "w");
                ak_start(tree, stdout);
        } else {
                ak_restore(tree, filename);
                printf("Да, я вспомнила.\n");
                printf("Начнем игру.\n");
        }

        save->stream = stream;
        save->filename = filename;
}

void
close_save(tree_t *tree, game_save_t *save)
{
        free(save->filename);
        fclose(save->stream);
}
