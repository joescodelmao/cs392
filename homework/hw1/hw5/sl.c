/*
Name: Joseph Gargiulo
Date: 4/2/24
I pledge my honor that I have abided by the Stevens Honor System
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]) {
    // Task 1: Validating Command-Line Arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <DIRECTORY>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Check if the first argument is a directory
    struct stat path_stat;
    if (stat(argv[1], &path_stat) == -1) {
        fprintf(stderr, "Permission denied. %s cannot be read.", argv[1]);
        return EXIT_FAILURE;
    }

    if (!S_ISDIR(path_stat.st_mode)) {
        fprintf(stderr, "The first argument has to be a directory.");
        return EXIT_FAILURE;
    }

    // Check if directory can be read
    DIR* dir = opendir(argv[1]);
    if (!dir) {
        fprintf(stderr, "Permission denied. %s cannot be read.", argv[1]);
        return EXIT_FAILURE;
    }
    closedir(dir);

    // Create pipes
    int pipe_ls_sort[2], pipe_sort_parent[2];
    if (pipe(pipe_ls_sort) == -1 || pipe(pipe_sort_parent) == -1) {
        perror("Pipe creation failed");
        return EXIT_FAILURE;
    }

    pid_t pid_ls, pid_sort;

    // Fork 1: ls -ai <DIRECTORY> child process
    pid_ls = fork();
    if (pid_ls == -1) {
        perror("Fork for ls failed");
        return EXIT_FAILURE;
    } else if (pid_ls == 0) {
        // Child 1: ls -ai
        close(pipe_ls_sort[0]); // Close unused read end of pipe
        dup2(pipe_ls_sort[1], STDOUT_FILENO); // Redirect stdout to pipe
        close(pipe_ls_sort[1]); // Close duplicated fd
        execlp("ls", "ls", "-1ai", argv[1], NULL);
        // If execlp fails
        perror("Error: ls failed");
        exit(EXIT_FAILURE);
    }

    // Fork 2: sort child process
    pid_sort = fork();
    if (pid_sort == -1) {
        perror("Fork for sort failed");
        return EXIT_FAILURE;
    } else if (pid_sort == 0) {
        // Child 2: sort
        close(pipe_ls_sort[1]); // Close write end of pipe from ls
        dup2(pipe_ls_sort[0], STDIN_FILENO); // Redirect stdin from pipe
        close(pipe_ls_sort[0]); // Close duplicated fd
        close(pipe_sort_parent[0]); // Close unused read end of second pipe
        dup2(pipe_sort_parent[1], STDOUT_FILENO); // Redirect stdout to second pipe
        close(pipe_sort_parent[1]); // Close duplicated fd
        execlp("sort", "sort", NULL);
        // If execlp fails
        perror("Error: sort failed");
        exit(EXIT_FAILURE);
    }

    // Parent process
    close(pipe_ls_sort[0]); // Close read end of pipe from ls
    close(pipe_ls_sort[1]); // Close write end of pipe from ls

    close(pipe_sort_parent[1]); // Close write end of second pipe
    dup2(pipe_sort_parent[0], STDIN_FILENO); // Redirect stdin from second pipe
    close(pipe_sort_parent[0]); // Close duplicated fd

    // Count lines to get total files
    int count = 0;
    char buffer[4096];
    int isTotalFilesLinePrinted = 0;
    ssize_t n;
    while ((n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';  // Null-terminate the buffer
        char *line = strtok(buffer, "\n");
        while (line != NULL) {
            if (strlen(line) > 0) {
                // Skip the line if it's the current directory (.)
                if (strcmp(line, ".") != 0) {
                    printf("%s\n", line); // Print each line except "Total files:" and the current directory
                    count++;
                }
            }
            line = strtok(NULL, "\n");
        }
    }

    // Print total files if the "Total files:" line was not printed already
    if (!isTotalFilesLinePrinted && count > 0) {
        printf("Total files: %d\n", count);
    }

    // Wait for child processes to finish
    wait(NULL);
    wait(NULL);

    return EXIT_SUCCESS;
}
