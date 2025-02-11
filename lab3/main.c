#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE 100  // Max input length
#define MAX_ARGS 10   // Max number of arguments

int main(int argc, char *argv[]) {
    char line[MAX_LINE];
    char *args[MAX_ARGS];
    int background;
    int ch, i;

    while (true) {
        printf("Your command> ");
        fflush(stdout);

        // Read input using getchar()
        i = 0;
        while ((ch = getchar()) != '\n') {
            line[i++] = ch;
        }
        line[i] = '\0';  // Null-terminate the string

        // Exit command
        if (strcmp(line, "exit") == 0) {
            break;
        }

        // Tokenize input and store in args[]
        int arg_count = 0;
        char *token = strtok(line, " ");
        while (token != NULL && arg_count < MAX_ARGS - 1) {
            args[arg_count++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_count] = NULL; // Null-terminate args list

        // Handle background execution (& detection)
        background = 0;
        // Check if the last argument has '&' and ensure there's no space before it
        if (arg_count > 0 && strlen(args[arg_count - 1]) > 1 && args[arg_count - 1][strlen(args[arg_count - 1]) - 1] == '&' && args[arg_count - 1][strlen(args[arg_count - 1]) - 2] != ' ') {
            background = 1;
            args[arg_count - 1][strlen(args[arg_count - 1]) - 1] = '\0'; // Remove '&'

            // If last arg becomes empty, remove it from args[]
            if (strlen(args[arg_count - 1]) == 0) {
                args[arg_count - 1] = NULL;
                arg_count--;
            }
        }

        if (arg_count == 0) {
            continue; // Ignore empty input
        }

        // Fork and execute the command
        pid_t pid = fork(); // Creating a child process
        if (pid < 0) {
            perror("Fork failed");
            exit(1);
        } else if (pid == 0) { // Child process
            execvp(args[0], args);
            perror("Exec failed"); // Print error if exec fails
            exit(1);
        } else { // Parent process
            if (!background) {
                waitpid(pid, NULL, 0); // Wait for child unless it's a background process
            }
        }
    }

    return 0;
}