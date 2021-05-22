#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <pthread.h>
#include <stdbool.h>
#include <fcntl.h>

#define PORT 8080

pthread_t tid;
int connection = 0;

typedef struct akun{
    char name[64];
    char password[64];
} Account;

typedef struct file{
    char publisher[64];
    char year[64];
    char name[64];
    char path[64];
} File;

void read_tsv_line(File *store, char *line) {
    int i = 0;
    int j = 0;
    // read file name
    while (line[i] != '/') {
        i++;
    }
    i++;
    while (line[i] != '\t') {
        store->name[j] = line[i];
        i++;
        j++;
    }
    store->name[j] = '\0';
    i++;
    j = 0;
    strcpy(store->path, "FILES/");
    strcat(store->path, store->name);

    // read publisher
    while (line[i] != '\t') {
        store->publisher[j] = line[i];
        i++;
        j++;
    }
    store->publisher[j] = '\0';
    i++;
    j = 0;

    // read year
    while (line[i] != '\n') {
        store->year[j] = line[i];
        i++;
        j++;
    }
    store->year[j] = '\0';

    return;
}

void log_action(char *type, char *fileName, char *user, char *pass) {
    FILE *log;
    char action[16];

    if (strcmp(type, "add") == 0) {
        strcpy(action, "Tambah");
    }
    else if (strcmp(type, "delete") == 0) {
        strcpy(action, "Hapus");
    }

    log = fopen("running.log", "a");
    fprintf(log, "%s : %s (%s:%s)\n", action, fileName, user, pass);
    fclose(log);

    return;
}

void removeLine(int line) {
    int ctr = 0;
    char ch;

    char fname[] = "files.tsv";
    char temp[] = "temp.tsv";
    FILE *fp1, *fp2;

    // buat copy ke file temp
    char str[1024];

    fp1 = fopen(fname, "r");
    // can't open file
    if (!fp1) {
        printf("file cannot be opened\n");
        return;
    }

    fp2 = fopen(temp, "w");
    if (!fp2) {
        printf("unable to make temp\n");
        fclose(fp1);
        return;
    }

    while (!feof(fp1)) {
        strcpy(str, "\0");
        fgets(str, 1024, fp1);

        if (!feof(fp1)) {
            if (ctr != line) {
                fprintf(fp2, "%s", str);
            }
            else {
                int len = strlen(str);
                str[len-1] = '\0';
            }
            ctr++;
        }
    }
    fclose(fp1);
    fclose(fp2);
    remove(fname);
    rename(temp, fname);
}

void processPath(char *client, char *fileName) {
    int i = 0;
    int flag = 0;
    while (client[i] != '.') {
        if (client[i] == '/') {
            flag = 1;
        }
        i++;
    }
    if (flag) {
        while (client[i] != '/') {
            i--;
        }
        i++;
    }
    else {
        i = 0;
    }

    int j = 0;
    while (client[i] != '\0') {
        fileName[j] = client[i];
        i++;
        j++;
    }
    fileName[j] = '\0';

    return;
}

void *process(void *temp){
    Account akun;
    FILE* file;
    int new_socket = *(int *) temp;
    int valread;
    char buffer[1024] = {0};

    // Client handler, baik single-connect maupun multiconnect
    send(new_socket, "unready", 1024, 0);
    while(connection){
        valread = read(new_socket, buffer, 1024);
        send(new_socket, "unready", 1024, 0);
    }
    connection = 1;
    valread = read(new_socket, buffer, 1024);
    send(new_socket, "ready", 1024, 0);

    while(true){
        printf("Client dapat mengakses server...\n\n");
        valread = read(new_socket, buffer, 1024);

        if (strcmp("login", buffer) == 0){
            valread = read(new_socket, akun.name, 1024);
            valread = read(new_socket, akun.password, 1024);

            if (access("akun.txt", F_OK) != 0){
                printf("akun.txt tidak ditemukan, membuat file...\n\n");
                file = fopen("akun.txt", "a+");
                fclose(file);
            }

            file = fopen("akun.txt", "r");

            int flag = 0;
            char *line = NULL;
            ssize_t len = 0;
            ssize_t file_read;

            while ((file_read = getline(&line, &len, file) != -1)) {
                char t_name[1024];
                char t_pass[1024];

                int i = 0;
                while (line[i] != ':') {
                    t_name[i] = line[i];
                    i++;
                }
                t_name[i] = '\0';
                i++;
                int j = 0;
                while (line[i] != '\n') {
                    t_pass[j] = line[i];
                    j++;
                    i++;
                }
                t_pass[j] = '\0';

                if (strcmp(akun.name, t_name) == 0 && strcmp(akun.password, t_pass) == 0) {
                    flag = 1;
                    char *buffer = "Login success";
                    send(new_socket, buffer, strlen(buffer), 0);
                    break;
                }
            }

            fclose(file);
            if (flag == 0) {
                printf("Login gagal...\n\n");
                char *buffer = "Login failed";
                send(new_socket, buffer, strlen(buffer), 0);
            }

            else {
                printf("Login berhasil...\n\n");

                while (true){
                    valread = read(new_socket, buffer, 1024);
                    if (strcmp("logout", buffer) == 0) {
                        break;
                    }

                    else if (strcmp("add", buffer) == 0) {
                        File request;

                        char clientPath[1024];

                        valread = read(new_socket, request.publisher, 1024);
                        valread = read(new_socket, request.year, 1024);
                        valread = read(new_socket, clientPath, 1024);

                        processPath(clientPath, request.name);

                        strcpy(request.path, "FILES/");
                        strcat(request.path, request.name);

                        // start adding
                        int des_fd = open(request.path, O_WRONLY | O_CREAT | O_EXCL, 0700);
                        if (!des_fd) {
                            perror("can't open file");
                            exit(EXIT_FAILURE);
                        }

                        int file_read_len;
                        char buff[1024];

                        while (true) {
                            memset(buff, 0x00, 1024);
                            file_read_len = read(new_socket, buff, 1024);
                            write(des_fd, buff, file_read_len);
                            break;
                        }
                        // done adding

                        file = fopen("files.tsv", "a");
                        fprintf(file, "%s\t%s\t%s\n", request.path, request.publisher, request.year);
                        fclose(file);

                        log_action("add", request.name, akun.name, akun.password);
                        continue;
                    }

                    else if (strcmp("download", buffer) == 0) {
                        valread = read(new_socket, buffer, 1024);

                        file = fopen("files.tsv", "r");

                        char *line = NULL;
                        ssize_t len = 0;
                        ssize_t file_read;

                        bool found = false;
                        char error_message[] = "No such file found.\n";
                        char good_message[] = "File ready to download.\n";
                        char file_loc[1024];

                        while ((file_read = getline(&line, &len, file) != -1)) {
                            File temp_entry;
                            read_tsv_line(&temp_entry, line);

                            if (strcmp(buffer, temp_entry.name) == 0) {
                                found = true;
                                strcpy(file_loc, temp_entry.path);
                                break;
                            }
                        }
                        if (!found) {
                            send(new_socket, error_message, 1024, 0);
                        }
                        else {
                            send(new_socket, good_message, 1024, 0);
                            
                            printf("Sedang mengirim file : %s\n", file_loc);
                            int fd = open(file_loc, O_RDONLY);
                            if (!fd) {
                                perror("can't open");
                                exit(EXIT_FAILURE);
                            }

                            int read_len;
                            while (true) {
                                memset(file_loc, 0x00, 1024);
                                read_len = read(fd, file_loc, 1024);

                                if (read_len == 0) {
                                    break;
                                }
                                else {
                                    send(new_socket, file_loc, read_len, 0);                               
                                }
                            }
                        }
                        fclose(file);
                    }

                    else if (strcmp("delete", buffer) == 0) {
                        valread = read(new_socket, buffer, 1024);

                        file = fopen("files.tsv", "r");

                        char *line = NULL;
                        ssize_t len = 0;
                        ssize_t file_read;

                        bool found = false;
                        char error_message[] = "No such file found.\n";
                        char good_message[] = "deleted.\n";

                        int index = 0;

                        while ((file_read = getline(&line, &len, file) != -1)) {
                            File temp_entry;
                            read_tsv_line(&temp_entry, line);

                            if (strcmp(buffer, temp_entry.name) == 0) {
                                found = true;
                                char old[] = "FILES/old-";
                                strcat(old, temp_entry.name);
                                rename(temp_entry.path, old);
                                log_action("delete", temp_entry.name, akun.name, akun.password);
                                break;
                            }
                            index++;
                        }
                        if (!found) {
                            send(new_socket, error_message, 1024, 0);
                        }
                        else {
                            removeLine(index);
                            send(new_socket, good_message, 1024, 0);
                        }
                        fclose(file);
                    }

                    else if (strcmp("see", buffer) == 0) {
                        file = fopen("files.tsv", "r");
                        if (!file) {
                            send(new_socket, "e", sizeof("e"), 0);
                            memset(buffer, 0, sizeof(buffer));
                            continue;
                        }

                        char *line = NULL;
                        ssize_t len = 0;
                        ssize_t file_read;
                        while ((file_read = getline(&line, &len, file) != -1)) {
                            File temp_entry;
                            read_tsv_line(&temp_entry, line);

                            // read extension
                            char ext[64];
                            int i = 0;
                            while (temp_entry.path[i] != '.') {
                                i++;
                            }
                            int j = 0;
                            while (temp_entry.path[i] != '\0') {
                                ext[j] = temp_entry.path[i];
                                i++;
                                j++;
                            }
                            ext[j] = '\0';

                            char message[1024];
                            sprintf(message, "Nama : %s\nPublisher : %s\nTahun Publishing : %s\nEkstensi File : %s\nFilepath : %s\n\n", 
                                    temp_entry.name, temp_entry.publisher, temp_entry.year, ext, temp_entry.path);
                            
                            send(new_socket, message, 1024, 0);
                        }
                        send(new_socket, "e", sizeof("e"), 0);
                        fclose(file);
                    }

                    else if (strcmp("find", buffer) == 0) {
                        valread = read(new_socket, buffer, 1024);

                        file = fopen("files.tsv", "r");

                        char *line = NULL;
                        ssize_t len = 0;
                        ssize_t file_read;

                        bool found = false;
                        char error_message[] = "No such file found.\n";

                        while ((file_read = getline(&line, &len, file) != -1)) {
                            File temp_entry;
                            read_tsv_line(&temp_entry, line);

                            char *h;
                            if ((h = strstr(temp_entry.name, buffer)) != NULL) {
                                found = true;

                                // read extension
                                char ext[64];
                                int i = 0;
                                while (temp_entry.path[i] != '.') {
                                    i++;
                                }
                                int j = 0;
                                while (temp_entry.path[i] != '\0') {
                                    ext[j] = temp_entry.path[i];
                                    i++;
                                    j++;
                                }
                                ext[j] = '\0';

                                char message[1024];
                                sprintf(message, "Nama : %s\nPublisher : %s\nTahun Publishing : %s\nEkstensi File : %s\nFilepath : %s\n\n", 
                                        temp_entry.name, temp_entry.publisher, temp_entry.year, ext, temp_entry.path);
                                
                                send(new_socket, message, 1024, 0);
                            }
                        }
                        if (!found) {
                            send(new_socket, error_message, 1024, 0);
                        }
                        send(new_socket, "e", sizeof("e"), 0);
                        fclose(file);
                    }
                }
            }
        }

        if (strcmp("register", buffer) == 0){
            if (access("akun.txt", F_OK) != 0){
                printf("akun.txt tidak ditemukan, membuat file...\n\n");
                file = fopen("akun.txt", "a+");
                fclose(file);
            }
            file = fopen("akun.txt", "a");

            valread = read(new_socket, akun.name, 1024);
            valread = read(new_socket, akun.password, 1024);

            fprintf(file, "%s:%s\n", akun.name, akun.password);
            printf("Akun %s telah dicatat pada server...\n\n", akun.name);
            fclose(file);
        }

        else if (strcmp("quit", buffer) == 0){
            printf("Berhasil keluar...\n\n");
            connection = 0;
            close(new_socket);
            break;
        }

        else if (strcmp("return", buffer) == 0){
            printf("Mengambungkan kembali...\n\n");
            continue;
        }
    }
}

int main(int argc, char const *argv[]) {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
      
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    mkdir("FILES", 0777);

    while(new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)){
        printf("Client tersambung dengan server...\n\n");
        
        pthread_create(&tid, NULL, &process, &new_socket);

        if (new_socket < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
    }
    

    return 0;
}
