#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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

int
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
                if (ak_start(tree, stdout) == AK_EXIT) {
                        save->stream = stream;
                        save->filename = filename;

                        return AK_EXIT;
                }
        } else {
                ak_restore(tree, filename);
                printf("Да, я вспомнила.\n");
                printf("Начнем игру.\n");
        }

        printf("Я мудрая кошка, поэтому занимаюсь только умными делами.\n"
               "Я могу угадать что-то, описать или сравнить.\n");

        save->stream = stream;
        save->filename = filename;

        return AK_NORMAL;
}

void
close_save(tree_t *tree, game_save_t *save)
{
        assert(tree);
        assert(save);

        free(save->filename);
        fclose(save->stream);
}

int
get_choice()
{
        fprintf(stdout, "Что мне сделать?\n");
        char *answer = nullptr;
        int ans = 0;
        while (ans != ANS_EXIT) {
                ans = ask(&answer);
                switch(ans) {
                        case ANS_TRUE:
                                fprintf(stdout, "Да?..\n");
                                free(answer);
                                break;
                        case ANS_FALSE:
                                fprintf(stdout, "Нет?..\n");
                                free(answer);
                                break;
                        case ANS_EXIT:
                                fprintf(stdout, "Мне и самой не очень-то "
                                                "хотелось играть...\n");
                                free(answer);
                                break;
                        case ANS_LONG:
                                if (strcmp(answer, GUESS) == 0) {
                                        free(answer);
                                        return MODE_GUESS; 
                                } else if (strcmp(answer, DEFINE) == 0) {
                                        free(answer);
                                        return MODE_DEFINE;
                                } else if (strcmp(answer, COMPARE) == 0) {
                                        free(answer);
                                        return MODE_COMPARE;
                                } else {
                                        free(answer);
                                        fprintf(stdout, "Не поняла.\n");
                                }
                }
        }

        return MODE_EXIT;
}

int
ask(char **buf)
{
        assert(buf);

        char *data = nullptr;
        size_t buf_size = 0;

        buf_size = getline(&data, &buf_size, stdin); // " угадать"
        data[buf_size - 1] = '\0';

        *buf = data;

        if (strcmp(*buf, AGREE) == 0) 
                return ANS_TRUE;
        else if (strcmp(*buf, DISAGREE) == 0)
                return ANS_FALSE;
        else if (strcmp(*buf, EXIT) == 0)
                return ANS_EXIT;
        else 
                return ANS_LONG;
}

