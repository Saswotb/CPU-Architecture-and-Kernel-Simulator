#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MEM_SIZE 100

int memory[MEM_SIZE];

// Function declarations
void show_help();
void exec_add(char **args);
void exec_store(char **args);
void exec_load(char **args);
void exec_parallel();
void exec_mkdir(char **args);
void exec_write(char **args);
void exec_read(char **args);
void exec_list(char **args);

int main() {
    char input[256];
    printf("=== TinyOS CLI Simulator ===\n");
    printf("Type 'help' for available commands.\n");

    while (1) {
        printf("\nTinyOS> ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0; // remove newline

        char *cmd = strtok(input, " ");
        char *args[10];
        int i = 0;
        while ((args[i++] = strtok(NULL, " ")) != NULL);

        if (!cmd) continue;

        // COMMAND HANDLER
        if (strcmp(cmd, "exit") == 0) break;
        else if (strcmp(cmd, "help") == 0) show_help();
        else if (strcmp(cmd, "add") == 0) exec_add(args);
        else if (strcmp(cmd, "store") == 0) exec_store(args);
        else if (strcmp(cmd, "load") == 0) exec_load(args);
        else if (strcmp(cmd, "parallel") == 0) exec_parallel();
        else if (strcmp(cmd, "mkdir") == 0) exec_mkdir(args);
        else if (strcmp(cmd, "write") == 0) exec_write(args);
        else if (strcmp(cmd, "read") == 0) exec_read(args);
        else if (strcmp(cmd, "list") == 0) exec_list(args);
        else printf("Unknown command. Type 'help' for list.\n");
    }

    printf("TinyOS terminated.\n");
    return 0;
}

// -----------------------------------------------
// COMMAND DEFINITIONS
// -----------------------------------------------

void show_help() {
    printf("\nAvailable Commands:\n");
    printf(" add a b              → Add two numbers\n");
    printf(" store idx value      → Store value into memory\n");
    printf(" load idx             → Load value from memory\n");
    printf(" mkdir dirname        → Create a directory\n");
    printf(" write filename text  → Write text to a file\n");
    printf(" read filename        → Read contents of a file\n");
    printf(" list [path]          → List files in directory\n");
    printf(" parallel             → Run tasks in parallel (OpenMP)\n");
    printf(" help                 → Show this help message\n");
    printf(" exit                 → Exit the simulator\n");
}

// Arithmetic
void exec_add(char **args) {
    if (!args[0] || !args[1]) {
        printf("Usage: add a b\n");
        return;
    }
    int a = atoi(args[0]), b = atoi(args[1]);
    printf("Result = %d\n", a + b);
}

// Memory simulation
void exec_store(char **args) {
    if (!args[0] || !args[1]) {
        printf("Usage: store idx value\n");
        return;
    }
    int idx = atoi(args[0]), val = atoi(args[1]);
    if (idx >= 0 && idx < MEM_SIZE) {
        memory[idx] = val;
        printf("Stored %d at memory[%d]\n", val, idx);
    } else printf("Invalid memory index.\n");
}

void exec_load(char **args) {
    if (!args[0]) {
        printf("Usage: load idx\n");
        return;
    }
    int idx = atoi(args[0]);
    if (idx >= 0 && idx < MEM_SIZE)
        printf("memory[%d] = %d\n", idx, memory[idx]);
    else
        printf("Invalid memory index.\n");
}

// Parallel demonstration
void exec_parallel() {
    printf("Running 4 parallel tasks using OpenMP...\n");
    #pragma omp parallel num_threads(4)
    {
        int id = omp_get_thread_num();
        printf("Thread %d running task...\n", id);
    }
    printf("All parallel tasks finished.\n");
}

// Directory management
void exec_mkdir(char *args[]) {
    if (args[0] == NULL) {
        printf("mkdir: missing directory name\n");
        return;
    }
#ifdef _WIN32
    if (mkdir(args[0]) == 0)
#else
    if (mkdir(args[0], 0777) == 0)
#endif
        printf("Directory '%s' created successfully.\n", args[0]);
    else
        perror("mkdir");
}

// File operations
void exec_write(char **args) {
    if (!args[0] || !args[1]) {
        printf("Usage: write filename text\n");
        return;
    }
    FILE *fp = fopen(args[0], "w");
    if (!fp) {
        perror("write");
        return;
    }
    fprintf(fp, "%s\n", args[1]);
    fclose(fp);
    printf("Written to '%s'.\n", args[0]);
}

void exec_read(char **args) {
    if (!args[0]) {
        printf("Usage: read filename\n");
        return;
    }
    FILE *fp = fopen(args[0], "r");
    if (!fp) {
        perror("read");
        return;
    }
    char line[256];
    printf("Contents of %s:\n", args[0]);
    while (fgets(line, sizeof(line), fp))
        printf(" %s", line);
    fclose(fp);
}

void exec_list(char **args) {
    const char *path = args[0] ? args[0] : ".";
    DIR *d = opendir(path);
    if (!d) {
        perror("list");
        return;
    }
    struct dirent *dir;
    printf("Listing directory: %s\n", path);
    while ((dir = readdir(d)) != NULL)
        printf(" - %s\n", dir->d_name);
    closedir(d);
}
