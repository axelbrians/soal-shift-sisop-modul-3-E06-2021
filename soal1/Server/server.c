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

bool equal(char *temp, char *temp1){
    for(int i = 0; temp[i] != '\0'; i++){
        if(toupper(temp[i]) != toupper(temp1[i]))
            return false;
    }
    return true;
}

void *process(void *temp){
    Account akun;
    FILE* file;
    int new_socket = *(int *) temp;
    int valread;
    char buffer[1024] = {0};

    while(connection);
    connection = 1;

    while(connection){
        printf("Client dapat mengakses server...\n\n");
        send(new_socket, "ready", 1024, 0);
        valread = read(new_socket, buffer, 1024);

        if (equal("register", buffer)){
            if (access("akun.txt", F_OK) != 0){
                printf("akun.txt tidak ditemukan, membuat file...\n\n");
                file = fopen("akun.txt", "a+");
            }
            file = fopen("akun.txt", "a");

            valread = read(new_socket, akun.name, 1024);
            valread = read(new_socket, akun.password, 1024);

            fprintf(file, "%s:%s\n", akun.name, akun.password);
            printf("Akun %s telah dicatat pada server...\n\n", akun.name);
            fclose(file);
        }

        else if (equal("login", buffer)){
            valread = read(new_socket, akun.name, 1024);
            valread = read(new_socket, akun.password, 1024);

            if (access("akun.txt", F_OK) != 0){
                printf("akun.txt tidak ditemukan, membuat file...\n\n");
                file = fopen("akun.txt", "a+");
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

                if (equal(akun.name, t_name) && equal(akun.password, t_pass)) {
                    flag = 1;
                    char *buffer = "Login success";
                    send(new_socket, buffer, strlen(buffer), 0);
                    break;
                }
            }

            if (flag == 0) {
                char *buffer = "Login failed";
                send(new_socket, buffer, strlen(buffer), 0);
            }

            else {
                printf("Login berhasil...\n\n");

                while (true){
                    valread = read(new_socket, buffer, 1024);
                    if (equal("logout", buffer)) {
                        printf("Semoga hari anda menyenangkan...\n\n");
                        break;
                    }
                }
            }

            fclose(file);
        }

        else if (equal("quit", buffer)){
            printf("Berhasil keluar...\n\n");
            connection = 0;
            close(new_socket);
            break;
        }

        else if (equal("return", buffer)){
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
