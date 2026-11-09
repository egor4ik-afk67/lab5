/*
 * Лабораторна робота №5, завдання 3: SIGUSR1 і перезавантаження конфігурації.
 * Збірка: gcc -Wall -Wextra -o lab5_3 lab5_3.c
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CONFIG_FILE "config.txt"

static char logLevel[32] = "info";
static int  logInterval  = 5;

/* Прапорець, який виставляє обробник сигналу. */
static volatile sig_atomic_t reload_requested = 0;

/* Читає config.txt (формат: "<рівень> <інтервал>") і виводить нові значення. */
static void loadConfig(void)
{
    FILE *f = fopen(CONFIG_FILE, "r");
    if (f == NULL) {
        perror("Не вдалося відкрити " CONFIG_FILE);
        return;
    }

    char level[32];
    int interval;

    if (fscanf(f, "%31s %d", level, &interval) == 2 && interval > 0) {
        strcpy(logLevel, level);
        logInterval = interval;
        printf("\n[CONFIG LOADED] logLevel: \"%s\", logInterval: %d\n\n",
               logLevel, logInterval);
    } else {
        fprintf(stderr, "Некоректний формат %s, залишено попередні значення\n",
                CONFIG_FILE);
    }
    fclose(f);
    fflush(stdout);
}

/*
 * Обробник SIGUSR1. Функції fopen/fscanf/printf не є async-signal-safe,
 * тому в самому обробнику лише ставимо прапорець; loadConfig() викликається
 * одразу після цього в головному циклі (sleep перериває сам сигнал).
 */
static void handle_sigusr1(int sig)
{
    (void)sig;
    reload_requested = 1;
}

int main(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigusr1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; /* без SA_RESTART: sleep() повертається одразу після сигналу */

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    loadConfig();

    pid_t pid = getpid();
    printf("PID процесу: %d\n", (int)pid);
    printf("Команда для перезавантаження конфігурації:\n");
    printf("  kill -USR1 %d\n\n", (int)pid);
    fflush(stdout);

    for (;;) {
        if (reload_requested) {
            reload_requested = 0;
            loadConfig(); /* виклик loadConfig у відповідь на SIGUSR1 */
            continue;
        }

        printf("[%s] Повідомлення системи (інтервал %d сек)...\n",
               logLevel, logInterval);
        fflush(stdout);

        sleep((unsigned)logInterval); /* переривається сигналом */
    }

    return EXIT_SUCCESS;
}