/*
 * Лабораторна робота №5, завдання 2: обробка SIGQUIT і дамп стеку.
 * Збірка: gcc -Wall -Wextra -g -rdynamic -o lab5_2 lab5_2.c
 * (-rdynamic потрібен, щоб backtrace_symbols_fd показував імена функцій)
 */
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_FRAMES 64

/* Обробник SIGQUIT: виводить стек викликів. */
void handle_sigquit(int sig)
{
    void *frames[MAX_FRAMES];
    const char header[] = "\n--- Stack Trace (SIGQUIT) ---\n";
    const char footer[] = "--- End of Stack Trace ---\n\n";

    (void)sig;
    write(STDOUT_FILENO, header, sizeof(header) - 1);

    int n = backtrace(frames, MAX_FRAMES);
    backtrace_symbols_fd(frames, n, STDOUT_FILENO);

    write(STDOUT_FILENO, footer, sizeof(footer) - 1);
}

int main(void)
{
    pid_t pid = getpid();

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigquit;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGQUIT, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    printf("Програму запущено. PID процесу: %d\n", (int)pid);
    printf("Команда для надсилання сигналу з іншого терміналу:\n");
    printf("  kill -3 %d\n\n", (int)pid);
    printf("Очікування сигналу... (Натисніть Ctrl+\\ у цьому терміналі для перевірки)\n");
    printf("Для завершення програми натисніть Ctrl+C.\n");
    fflush(stdout);

    for (;;)
        sleep(1);

    return EXIT_SUCCESS;
}