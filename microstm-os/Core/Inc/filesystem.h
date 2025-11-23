#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_FILENAME_SIZE 16
#define MAX_PATH_SIZE 256

// Forward declaration
struct Directory;

typedef struct File {
    char name[MAX_FILENAME_SIZE];
    uint8_t* data;
    uint32_t size;
    struct File* next;
} File;

typedef struct Directory {
    char name[MAX_FILENAME_SIZE];
    struct Directory* parent;
    struct Directory* subdirs; // Linked list of subdirectories
    struct File* files;       // Linked list of files
    struct Directory* next;
} Directory;

void filesystem_init(void);
Directory* get_root_dir(void);
Directory* get_current_dir(void);
void set_current_dir(Directory* dir);

extern char inBuffer[2048];

void fs_get_cwd_path(char* buf, int size);
void fs_pwd(char* buf, int size);
void fs_ls(char* buf, int size);
int fs_mkdir(char* args);
int fs_cd(char* args);
int fs_rmdir(char* args);
int fs_touch(char* args);
int fs_cat(char* args);
int fs_rm(char* args);

void parse_command(char* buffer, char** command, char** args);
int fs_autocomplete(char* buffer, int len, char* out_buffer);

#endif // FILESYSTEM_H
