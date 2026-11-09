/*
 * Лабораторна робота №5, завдання 1: аналог утиліти pwd.
 * Збірка: gcc -Wall -Wextra -o pwd_79_17 pwd_79_17.c
 */
#define _GNU_SOURCE
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define PROG_NAME "pwd_79_17"
#define VERSION   "1.0.0"
#define AUTHOR    "Єгор Сємєйко" 

static void print_help(void)
{
    printf("Використання: %s [-L | -P] [ОПЦІЇ]\n", PROG_NAME);
    printf("Виводить ім'я поточної робочої директорії.\n\n");
    printf("  -L         Вивести логічний шлях (з $PWD, враховуючи символічні лінки)\n");
    printf("  -P         Вивести фізичний шлях (розкриваючи всі символічні лінки)\n");
    printf("  --help     Показати цю допомогу та завершити роботу\n");
    printf("  --version  Показати версію утиліти\n");
    printf("  --author   Показати ім'я та прізвище розробника\n");
}

/* Перевіряє, що $PWD дійсно вказує на поточну директорію ("."). */
static int pwd_is_valid(const char *pwd)
{
    struct stat a, b;

    if (pwd == NULL || pwd[0] != '/')
        return 0;
    if (stat(pwd, &a) != 0 || stat(".", &b) != 0)
        return 0;
    return a.st_dev == b.st_dev && a.st_ino == b.st_ino;
}

/* Логічний шлях: змінна оточення PWD (із запасним варіантом getcwd). */
static int print_logical(void)
{
    const char *pwd = getenv("PWD");

    if (pwd_is_valid(pwd)) {
        puts(pwd);
        return 0;
    }

    char buf[PATH_MAX];
    if (getcwd(buf, sizeof(buf)) == NULL) {
        perror(PROG_NAME ": getcwd");
        return 1;
    }
    puts(buf);
    return 0;
}

/* Фізичний шлях: realpath(".", NULL) без символічних лінків. */
static int print_physical(void)
{
    char *path = realpath(".", NULL);

    if (path == NULL) {
        perror(PROG_NAME ": realpath");
        return 1;
    }
    puts(path);
    free(path);
    return 0;
}

int main(int argc, char *argv[])
{
    static const struct option long_opts[] = {
        {"help",    no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'v'},
        {"author",  no_argument, NULL, 'a'},
        {NULL, 0, NULL, 0}
    };

    int physical = 0; /* за замовчуванням -L */
    int opt;

    while ((opt = getopt_long(argc, argv, "LP", long_opts, NULL)) != -1) {
        switch (opt) {
        case 'L':
            physical = 0;
            break;
        case 'P':
            physical = 1;
            break;
        case 'h':
            print_help();
            return 0;
        case 'v':
            printf("pwd_c версія %s\n", VERSION);
            return 0;
        case 'a':
            printf("Розробник: %s\n", AUTHOR);
            return 0;
        default:
            fprintf(stderr, "Спробуйте '%s --help' для додаткової інформації.\n",
                    PROG_NAME);
            return 1;
        }
    }

    if (optind < argc) {
        fprintf(stderr, "%s: забагато аргументів\n", PROG_NAME);
        return 1;
    }

    return physical ? print_physical() : print_logical();
}