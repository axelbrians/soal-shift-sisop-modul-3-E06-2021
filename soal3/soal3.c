#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdbool.h>

typedef struct {
    bool isUsed;
    pthread_t thread;
} mythread_t;


char base_path[200];
char file_col[30000][500];
int counter = 2;
bool success = false;
int index_unkown = 2;

const int MODE_FILE = 1;
const int MODE_DIR = 2;
const int MODE_WD = 3;

// question function
void option_file();
void option_dir();
void option_wd();
void *move_file();


// execv function
bool is_file_exist();
bool is_file_type_unknown(void *param);
int is_directory();
void create_dir();
void list_file();
void convert_lowercase();


int main(int argc, char *argv[]) {

    // for (int i = 0; i < argc; i++) {
    //     printf("argument: %d\n", i);
    //     printf("content: %s\n\n", argv[i]);
    // }

    getcwd(base_path, sizeof(base_path));
    // printf("\nbase path\n%s\n\n", base_path);

    if (argv[1][0] == '*') {
        option_wd();
    } else if (strcmp(argv[1], "-f") == 0) {
        option_file(argc, argv, MODE_FILE);
    } else if (strcmp(argv[1], "-d") == 0) {
        option_dir(argv, MODE_DIR);
    }

}

void option_wd() {
    char *argv[] = { "dummy", "mode", base_path, NULL };
    option_dir(argv, MODE_WD);
}

void option_dir(char *argv[], int option) {
    char *param[30000];

    list_file(argv[2]);

    //printf("=== list of file ===\n");
    for (int i = 2; i < counter + 2; i++) {
        param[i] = &file_col[i];
        // printf("%s\n", param[i]);
    }

    option_file(counter, param, option);
}

void list_file(char *param_path) {
    char path[300], path_to_file[300], temp[300], swap[300];

    struct dirent *dp;
    DIR *dir = opendir(param_path);

    bool is_unknown = false;

    if (!dir) return;

    // list file recursive
    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            
            strcpy(path, param_path);
            strcat(path, "/");
            strcat(path, dp->d_name);
            // printf("%s\n", path);
            is_unknown = is_file_type_unknown(path);

            if (is_unknown) {
                strcpy(temp, file_col[index_unkown]);
                strcpy(file_col[index_unkown], path);
                strcpy(file_col[counter], temp);
                index_unkown++;
            } else {
                strcpy(file_col[counter], path);
            }
            
            counter++;

            list_file(path);
        }
    }

    closedir(dir);
}


void option_file(int argc, char *argv[], int mode) {
    mythread_t mythread;
    int err, file_stat, counter = 0;
    bool is_exist;
    pthread_t thread[1000];

    mythread.isUsed = false;

    for (int i = 2; i < argc; i++) {
        is_exist = is_file_exist(argv[i]);
        file_stat = is_directory(argv[i]);

        if (is_exist && file_stat == 0) {
            char *param = (char*) argv[i];
            err = pthread_create(
                    &thread[i - 2], 
                    NULL,
                    &move_file,
                    (void *) param);
            mythread.isUsed = true;
            if (mode == MODE_FILE) 
                printf("File %d : Berhasil Dikategorikan\n", i - 1);
            if (mode == MODE_DIR)
                success = true;
        } else {
            if (mode == MODE_FILE)
                printf("File %d : Sad, gagal :(\n", (i - 1));
        }

        counter++;

        if (err != 0)
            printf("\n can't create thread : [%s]\n", strerror(err));
        else
            printf("\n create thread success %d\n", i - 2);
    }
    
    if (mythread.isUsed){
        for (int i = 0; i < counter; i++) {
            if (pthread_join(thread[i], NULL)) {
                fprintf(stderr, "error: Cannot join thread # %d\n", i);
            }
        }
    }

    if (!success && mode == MODE_DIR) {
        printf("Yah, gagal disimpan :(\n");
        return;
    } else if (success && mode == MODE_DIR){
        printf("Direktori sukses disimpan!\n");
    }
}

void *move_file(void *param) {
    char path_to_file[300], file_name[300], file_ext[30], temp[300];
    char *buffer;

    strcpy(temp, param);
    buffer = strtok(temp, "/");

    while (buffer != NULL) {
        // printf("%s ", buffer);
        if (buffer != NULL) strcpy(file_name, buffer);
        buffer = strtok(NULL, "/");
    }
    // printf("\n");
    strcpy(temp, file_name);
    strcpy(path_to_file, param);

    buffer = strtok(temp, ".");
    buffer = strtok(NULL, "");


    
    if (file_name[0] == '.') { // case for hidden file start with .
        strcpy(file_ext, "Hidden");
    } else if (buffer != NULL) { // for normal case
        strcpy(file_ext, buffer);
    } else {
        strcpy(file_ext, "Unknown");
    }

    convert_lowercase(file_ext);

    // create new path
    strcpy(temp, base_path);
    strcat(temp, "/");
    strcat(temp, file_ext);
    strcat(temp, "/");
    strcat(temp, file_name);


    // print information about the file that will be moved
    //printf("=== file information ===\n");
    // printf("path_to_file: %s\nfile_name: %s\nfile_ext: %s\nnew path: %s\n\n", path_to_file, file_name, file_ext, temp);

    // create dir based on extension
    create_dir(file_ext);

    // move file
    rename(path_to_file, temp);
}

// check is unknown type file or not
bool is_file_type_unknown(void *param) {
    char path_to_file[300], file_name[300], file_ext[30], temp[300];
    char *buffer;

    strcpy(temp, param);
    buffer = strtok(temp, "/");

    while (buffer != NULL) {
        // printf("%s ", buffer);
        if (buffer != NULL) strcpy(file_name, buffer);
        buffer = strtok(NULL, "/");
    }
    // printf("\n");
    strcpy(temp, file_name);
    strcpy(path_to_file, param);

    buffer = strtok(temp, ".");
    buffer = strtok(NULL, "");


    
    if (file_name[0] == '.') { // case for hidden file start with .
        return false;
    } else if (buffer != NULL) { // for normal case
        return false;
    } else {
        return true;
    }
}

// convert to lowercase
void convert_lowercase(char *param) {
    int i = 0;
    for (i; param[i]; i++)
        param[i] = tolower(param[i]);
}

// check path is it file or folder
int is_directory (const char *path) {
    struct stat statbuf;

    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
}

// check file in pwd path_to_file exist or not
bool is_file_exist (char *path_to_file) {
    FILE *file;

    if (file = fopen(path_to_file, "r")) {
        fclose(file);
        return true;
    }
    return false;
}

// create dir based on param
void create_dir(void *param) {
    char *value = (char *) param;
    char path[100];

    strcpy(path, base_path);
    strcat(path, "/");
    strcat(path, value);
    // printf("=== create_dir ===\n path: %s\n\n", path);
    
    int res = mkdir(path, 0755);
    if (res == -1) {
        // fprintf(stderr, "error: Cannot create dir # %s\n", path);
    }
}