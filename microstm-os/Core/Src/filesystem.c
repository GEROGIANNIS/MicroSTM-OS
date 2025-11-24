#include "filesystem.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static Directory root_dir;
static Directory* current_dir;

// A helper function to find a directory by path
static Directory* fs_find_dir(const char* path) {
    if (path == NULL) {
        return NULL;
    }

    if (strcmp(path, ".") == 0 || strcmp(path, "") == 0) {
        return current_dir;
    }

    if (strcmp(path, "/") == 0) {
        return &root_dir;
    }

    char path_copy[MAX_PATH_SIZE];
    strncpy(path_copy, path, MAX_PATH_SIZE);
    path_copy[MAX_PATH_SIZE - 1] = '\0';

    Directory* start_dir = (path[0] == '/') ? &root_dir : current_dir;
    Directory* target_dir = start_dir;

    char* token = strtok(path_copy, "/");
    while (token != NULL) {
        if (strcmp(token, "..") == 0) {
            if (target_dir->parent != NULL) {
                target_dir = target_dir->parent;
            }
            token = strtok(NULL, "/");
            continue;
        }

        Directory* subdir = target_dir->subdirs;
        while (subdir != NULL) {
            if (strcmp(subdir->name, token) == 0) {
                target_dir = subdir;
                break;
            }
            subdir = subdir->next;
        }

        if (subdir == NULL) {
            return NULL; // Path not found
        }

        token = strtok(NULL, "/");
    }

    return target_dir;
}
// List of available commands for autocompletion
static const char* commands[] = {
    "help",
    "ls",
    "pwd",
    "cd",
    "mkdir",
    "rmdir",
    "touch",
    "cat",
    "rm",
    "BLUE",
	"RED",
	"GREEN",
	"WHITE",
	"ON",
	"OFF",
    "clear",
	"exit",
    NULL // Sentinel value to mark the end of the array
};

void filesystem_init(void) {
    // Initialize the root directory
    strncpy(root_dir.name, "/", MAX_FILENAME_SIZE);
    root_dir.parent = &root_dir; // Root's parent is itself
    root_dir.subdirs = NULL;
    root_dir.files = NULL;
    root_dir.next = NULL;

    // Set the current directory to root
    current_dir = &root_dir;
}

Directory* get_root_dir(void) {
    return &root_dir;
}

Directory* get_current_dir(void) {
    return current_dir;
}

void set_current_dir(Directory* dir) {
    current_dir = dir;
}

void fs_get_cwd_path(char* buf, int size) {
    if (current_dir == &root_dir) {
        snprintf(buf, size, "/");
        return;
    }

    char path[MAX_PATH_SIZE] = "";
    Directory* dir = current_dir;

    while (dir != &root_dir) {
        char temp[MAX_PATH_SIZE];
        snprintf(temp, MAX_PATH_SIZE, "/%s%s", dir->name, path);
        strncpy(path, temp, MAX_PATH_SIZE);
        dir = dir->parent;
    }

    snprintf(buf, size, "%s", path);
}

void fs_pwd(char* buf, int size) {
    if (current_dir == &root_dir) {
        snprintf(buf, size, "/\r\n");
        return;
    }

    char path[MAX_PATH_SIZE] = "";
    Directory* dir = current_dir;

    while (dir != &root_dir) {
        char temp[MAX_PATH_SIZE];
        snprintf(temp, MAX_PATH_SIZE, "/%s%s", dir->name, path);
        strncpy(path, temp, MAX_PATH_SIZE);
        dir = dir->parent;
    }

    snprintf(buf, size, "%s\r\n", path);
}

void fs_ls(char* buf, int size) {
    Directory* dir;
    File* file;
    int offset = 0;

    // List subdirectories
    for (dir = current_dir->subdirs; dir != NULL; dir = dir->next) {
        offset += snprintf(buf + offset, size - offset, "d %s\r\n", dir->name);
    }

    // List files
    for (file = current_dir->files; file != NULL; file = file->next) {
        offset += snprintf(buf + offset, size - offset, "f %s\r\n", file->name);
    }

    if (offset == 0) {
        snprintf(buf, size, "\r\n");
    }
}

int fs_mkdir(char* name) {
    if (name == NULL || strlen(name) == 0 || strlen(name) >= MAX_FILENAME_SIZE) {
        return -1; // Invalid name
    }

    // Check if a directory with the same name already exists
    for (Directory* dir = current_dir->subdirs; dir != NULL; dir = dir->next) {
        if (strcmp(dir->name, name) == 0) {
            return -2; // Directory already exists
        }
    }

    Directory* new_dir = (Directory*)malloc(sizeof(Directory));
    if (new_dir == NULL) {
        return -3; // Malloc failed
    }

    strncpy(new_dir->name, name, MAX_FILENAME_SIZE);
    new_dir->parent = current_dir;
    new_dir->subdirs = NULL;
    new_dir->files = NULL;

    // Add to the list of subdirectories
    new_dir->next = current_dir->subdirs;
    current_dir->subdirs = new_dir;

    return 0; // Success
}

int fs_cd(char* name) {
    if (name == NULL || strlen(name) == 0) {
        return -1; // Invalid name
    }

    Directory* new_dir = fs_find_dir(name);

    if (new_dir != NULL) {
        current_dir = new_dir;
        return 0; // Success
    }

    return -2; // Directory not found
}

int fs_rmdir(char* name) {
    if (name == NULL || strlen(name) == 0) {
        return -1; // Invalid name
    }

    Directory* dir_to_remove = NULL;
    Directory* prev_dir = NULL;

    // Find the directory to remove
    for (Directory* dir = current_dir->subdirs; dir != NULL; prev_dir = dir, dir = dir->next) {
        if (strcmp(dir->name, name) == 0) {
            dir_to_remove = dir;
            break;
        }
    }

    if (dir_to_remove == NULL) {
        return -2; // Directory not found
    }

    if (dir_to_remove->subdirs != NULL || dir_to_remove->files != NULL) {
        return -3; // Directory not empty
    }

    // Remove the directory from the list
    if (prev_dir == NULL) {
        current_dir->subdirs = dir_to_remove->next;
    } else {
        prev_dir->next = dir_to_remove->next;
    }

    free(dir_to_remove);

    return 0; // Success
}

int fs_touch(char* name) {
    if (name == NULL || strlen(name) == 0 || strlen(name) >= MAX_FILENAME_SIZE) {
        return -1; // Invalid name
    }

    // Check if a file with the same name already exists
    for (File* file = current_dir->files; file != NULL; file = file->next) {
        if (strcmp(file->name, name) == 0) {
            return -2; // File already exists
        }
    }

    File* new_file = (File*)malloc(sizeof(File));
    if (new_file == NULL) {
        return -3; // Malloc failed
    }

    strncpy(new_file->name, name, MAX_FILENAME_SIZE);
    new_file->data = NULL;
    new_file->size = 0;

    // Add to the list of files
    new_file->next = current_dir->files;
    current_dir->files = new_file;

    return 0; // Success
}

int fs_cat(char* name) {
    if (name == NULL || strlen(name) == 0) {
        return -1; // Invalid name
    }

    for (File* file = current_dir->files; file != NULL; file = file->next) {
        if (strcmp(file->name, name) == 0) {
            if (file->size == 0) {
                // For now, we just print a message for empty files
                // Later, we would print file->data
            }
            return 0; // Success
        }
    }

    return -2; // File not found
}

int fs_rm(char* name) {
    if (name == NULL || strlen(name) == 0) {
        return -1; // Invalid name
    }

    File* file_to_remove = NULL;
    File* prev_file = NULL;

    // Find the file to remove
    for (File* file = current_dir->files; file != NULL; prev_file = file, file = file->next) {
        if (strcmp(file->name, name) == 0) {
            file_to_remove = file;
            break;
        }
    }

    if (file_to_remove == NULL) {
        return -2; // File not found
    }

    // Remove the file from the list
    if (prev_file == NULL) {
        current_dir->files = file_to_remove->next;
    } else {
        prev_file->next = file_to_remove->next;
    }

    if (file_to_remove->data != NULL) {
        free(file_to_remove->data);
    }
    free(file_to_remove);

    return 0; // Success
}

void parse_command(char* buffer, char** command, char** args) {
    *command = strtok(buffer, " \r\n");
    *args = strtok(NULL, "\r\n");
}

int fs_autocomplete(char* buffer, int len, char* out_buffer) {
    if (len == 0) return 0;

    char partial_name[MAX_FILENAME_SIZE];
    strncpy(partial_name, buffer, len);
    partial_name[len] = 0;

    char* best_match = NULL;
    int match_count = 0;

    // Command completion (only for the first word)
    if (buffer == inBuffer) {
        for (int i = 0; commands[i] != NULL; i++) {
            if (strncmp(commands[i], partial_name, len) == 0) {
                if (best_match == NULL) best_match = (char*)commands[i];
                match_count++;
            }
        }
        if (match_count == 1) {
            strncpy(out_buffer, best_match + len, MAX_FILENAME_SIZE - 1);
            out_buffer[MAX_FILENAME_SIZE - 1] = '\0';
            return strlen(out_buffer);
        }
    }

    // Path completion
    match_count = 0;
    best_match = NULL;

    char path_buf[MAX_PATH_SIZE];
    strncpy(path_buf, partial_name, MAX_PATH_SIZE);
    path_buf[MAX_PATH_SIZE - 1] = '\0';

    char* last_slash = strrchr(path_buf, '/');
    Directory* search_dir;
    char* partial_item;

    if (last_slash != NULL) {
        partial_item = last_slash + 1;
        if (last_slash == path_buf) {
            search_dir = &root_dir;
        } else {
            *last_slash = '\0';
            search_dir = fs_find_dir(path_buf);
        }
    } else {
        search_dir = current_dir;
        partial_item = path_buf;
    }

    if (search_dir == NULL) return 0;

    int partial_item_len = strlen(partial_item);

    // Search subdirectories
    for (Directory* dir = search_dir->subdirs; dir != NULL; dir = dir->next) {
        if (strncmp(dir->name, partial_item, partial_item_len) == 0) {
            if (best_match == NULL) best_match = dir->name;
            match_count++;
        }
    }

    // Search files
    for (File* file = search_dir->files; file != NULL; file = file->next) {
        if (strncmp(file->name, partial_item, partial_item_len) == 0) {
            if (best_match == NULL) best_match = file->name;
            match_count++;
        }
    }

    if (match_count == 1) {
        strncpy(out_buffer, best_match + partial_item_len, MAX_FILENAME_SIZE - 1);
        out_buffer[MAX_FILENAME_SIZE - 1] = '\0';
        return strlen(out_buffer);
    }

    return 0;
}

