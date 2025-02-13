/*
Name : Joseph Gargiulo
Date: 3/30/24
Pledge: I pledge my honor that I have abided by the Stevens Honor System
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <pwd.h>
#include <errno.h>
#include <dirent.h>

#define BLUE "\x1b[34;1m"
#define DEFAULT "\x1b[0m"

volatile sig_atomic_t interrupted = 0;

void sig_handler(int sig) {
    interrupted = sig;
    write(1, "\n", 1);
}

void lp_command();
void lf_command();
void echo_command(int argc, char *argv[]);

int main() {
    char input[4096];
    char *command;
    char *args[4096];
    int arg_count = 0;
    struct passwd *pw = getpwuid(getuid());

    while (1) {
        // Set up signal handler
        struct sigaction action;
        action.sa_handler = sig_handler;
        if (sigaction(SIGINT, &action, NULL) == -1) {
            fprintf(stderr, "Error: Cannot register signal handler. %s.\n", strerror(errno));
            continue;
        }

        // Get current working directory
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Print prompt
        printf("[%s%s%s]> ", BLUE, cwd, DEFAULT);
        fflush(stdout);

        // Read user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            if (interrupted == SIGINT) {
                interrupted = 0;
                continue;
            }
            fprintf(stderr, "Error: Failed to read from stdin. %s.\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Check for empty input
        if (strcmp(input, "\n") == 0) {
            continue;
        }

        // Remove trailing newline
        if (input[strlen(input) - 1] == '\n') {
            input[strlen(input) - 1] = '\0';
        }

        // Tokenize user input
        arg_count = 0;
        char *token = strtok(input, " ");
        while (token != NULL && arg_count < sizeof(args)/sizeof(args[0]) - 1) {
            args[arg_count] = token;
            token = strtok(NULL, " ");
            arg_count++;
        }
        args[arg_count] = NULL;

        command = args[0];

        // Check for exit command
        if (strcmp(command, "exit") == 0) {
            exit(EXIT_SUCCESS);
        }

        // Check for cd command
        if (strcmp(command, "cd") == 0) {
            // Change directory
            if (arg_count == 1 || (arg_count == 2 && strcmp(args[1], "~") == 0)) {
                if (pw != NULL && chdir(pw->pw_dir) == -1) {
                    fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", pw->pw_dir, strerror(errno));
                }
            } else if (arg_count == 2) {
                // Check for ~ expansion
                if (args[1][0] == '~') {
                    char expanded_path[1024];
                    snprintf(expanded_path, sizeof(expanded_path), "%s%s", pw->pw_dir, args[1] + 1);
                    if (chdir(expanded_path) == -1) {
                        fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", expanded_path, strerror(errno));
                    }
                } else {
                    if (chdir(args[1]) == -1) {
                        fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", args[1], strerror(errno));
                    }
                }
            } else {
                fprintf(stderr, "Error: Too many arguments to cd.\n");
            }
            continue;
        }

        // Check for pwd command
        if (strcmp(command, "pwd") == 0) {
            printf("%s\n", cwd);
            continue;
        }

        // Check for lf command
        if (strcmp(command, "lf") == 0) {
            lf_command();
            continue;
        }

        // Check for lp command
        if (strcmp(command, "lp") == 0) {
            lp_command();
            continue;
        }

        // Check for echo command
        if (strcmp(command, "echo") == 0) {
            echo_command(arg_count, args);
            continue;
        }

        // Fork and execute other commands
        pid_t pid = fork();
        if (pid == -1) {
            fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno));
            continue;
        }

        if (pid == 0) {
            // Child process
            if (execvp(command, args) == -1) {
                fprintf(stderr, "Error: exec() failed. No such file or directory. %s.\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        } else {
            // Parent process
            int status;
            if (waitpid(pid, &status, 0) == -1) {
                fprintf(stderr, "Error: wait() failed. %s.\n", strerror(errno));
                continue;
            }
        }

        interrupted = 0; // Reset signal flag
    }

    return 0;
}

void lp_command() {
    DIR *proc_dir;
    struct dirent *entry;
    char cmd_path[1024];
    char cmdline[1024];

    // Open the /proc directory
    if ((proc_dir = opendir("/proc")) == NULL) {
        fprintf(stderr, "Error: Cannot open /proc directory. %s.\n", strerror(errno));
        return;
    }

    // Iterate through each entry in the /proc directory
    while ((entry = readdir(proc_dir)) != NULL) {
        if (atoi(entry->d_name) != 0) {
            // This is a directory representing a process
            sprintf(cmd_path, "/proc/%s/cmdline", entry->d_name);
            FILE *cmd_file = fopen(cmd_path, "r");
            if (cmd_file == NULL) {
                fprintf(stderr, "Error: Cannot open %s. %s.\n", cmd_path, strerror(errno));
                continue;
            }
            // Read the command line of the process
            fgets(cmdline, sizeof(cmdline), cmd_file);
            fclose(cmd_file);

            // Remove trailing newline
            if (cmdline[strlen(cmdline) - 1] == '\n') {
                cmdline[strlen(cmdline) - 1] = '\0';
            }

            // Print the process information
            printf("%s %s %s\n", entry->d_name, getpwuid(getuid())->pw_name, cmdline);
        }
    }

    closedir(proc_dir);
}

void lf_command() {
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(".")) == NULL) {
        fprintf(stderr, "Error: Cannot open current directory. %s.\n", strerror(errno));
        return;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
            printf("%s\n", ent->d_name);
        }
    }

    closedir(dir);
}

void echo_command(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");
}
