#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>

int main(int argc, char** argv) {
    char line[100];
    int ch, i;

    while (true) {
        printf("Your Command> ");

        // Read input using getchar() (Unchanged)
        i = 0;
        while ((ch = getchar()) != '\n') {
            line[i++] = ch;
        }
        line[i] = '\0';  // Null-terminate the string

        // Exit command
        if (strcmp(line, "exit") == 0) {
            break;
        }

        // Find the pipe symbol '|'
        char *pipe_pos = strchr(line, '|');
        if (!pipe_pos) {
            printf("Invalid command format. Use: cmd1 | cmd2\n");
            continue;
        }

        // Split into two commands
        *pipe_pos = '\0';
        char *cmd1 = line;
        char *cmd2 = pipe_pos + 1;

        // Trim spaces
        while (*cmd1 == ' ') cmd1++;
        while (*cmd2 == ' ') cmd2++;

        // Tokenize command arguments
        char *args1[10], *args2[10];  // Max 10 args
        int j = 0;
        args1[j] = strtok(cmd1, " ");
        while (args1[j] != NULL) {
            args1[++j] = strtok(NULL, " ");
        }

        j = 0;
        args2[j] = strtok(cmd2, " ");
        while (args2[j] != NULL) {
            args2[++j] = strtok(NULL, " ");
        }

        // Create a pipe
        int fd[2];
        if (pipe(fd) == -1) {
            perror("pipe failed");
            exit(1);
        }

        pid_t pid1 = fork();
        if (pid1 < 0) {
            perror("fork failed");
            exit(1);
        }

        if (pid1 == 0) {
            // First child (cmd1)
            close(fd[0]);  
            dup2(fd[1], STDOUT_FILENO);  // Redirect stdout to pipe
            close(fd[1]);  

            execvp(args1[0], args1);
            perror("execvp failed");
            exit(1);
        } 

        pid_t pid2 = fork();
        if (pid2 < 0) {
            perror("fork failed");
            exit(1);
        }

        if (pid2 == 0) {
            // Second child (cmd2)
            close(fd[1]);  
            dup2(fd[0], STDIN_FILENO);  // Redirect stdin from pipe
            close(fd[0]);  

            execvp(args2[0], args2);
            perror("execvp failed");
            exit(1);
        }

        // Parent closes pipe and waits for children
        close(fd[0]);
        close(fd[1]);
        wait(NULL);
        wait(NULL);
    }

    return 0;
}