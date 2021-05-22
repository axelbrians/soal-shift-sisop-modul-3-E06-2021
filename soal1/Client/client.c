#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <ctype.h>
#include <termios.h>
#include <pthread.h>
#include <stdbool.h>
#include <fcntl.h>

#define PORT 8080

int main(int argc, char const *argv[]) {
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    char command[1024], name[1024], password[1024], temp;
    printf("Menghubungkan...\n\n");

    valread = read(sock, buffer, 1024);
    while(strcmp("ready", buffer) != 0){
        printf("Input sesuatu untuk memuat ulang...");
        scanf("%s", command);
        send(sock, command, 1024, 0);
        valread = read(sock, buffer, 1024);
    }
    memset(buffer, 0, 1024);
    printf("Terhubung...\n\n");

    while(true){
        printf("Menu :\n1. Register\n2. Login\n3. Quit\n\n");
        printf("Masukkan : ");  scanf("%s", command);

        if (strcmp(command, "login") == 0) {
            send(sock, "login", 1024, 0);
            
            printf("\nLogin\n");
            printf("Username: ");
            scanf("%c", &temp);
            scanf("%[^\n]", name);
            send(sock, name, 1024, 0);

            printf("Password: ");
            scanf("%c", &temp);
            scanf("%[^\n]", password);
            send(sock, password, 1024, 0);

            memset(buffer, 0, sizeof(buffer));
            valread = read(sock, buffer, 1024);
            printf("%s\n", buffer);

            if (strcmp(buffer, "Login success") == 0) {
                while (true) {
                    printf("1. Logout\n2. Add\n3. Download [File]\n4. Delete [File]\n5. See\n6. Find [File]\n\n");
                    printf("Masukkan : ");  scanf("%s", command);

                    if (strcmp(command, "logout") == 0) {
                        printf("Berhasil logout...\n\n");
                        send(sock, command, 1024, 0);
                        break;
                    }
                    
                    else if (strcmp(command, "add") == 0) {
                        send(sock, command, 1024, 0);
                        char data[1024];
                        printf("\nAdd\n");

                        printf("Publisher: ");
                        scanf("%c", &temp);
                        scanf("%[^\n]", data);
                        send(sock, data, 1024, 0);

                        printf("Tahun Publikasi: ");
                        scanf("%c", &temp);
                        scanf("%[^\n]", data);
                        send(sock, data, 1024, 0);

                        printf("Filepath: ");
                        scanf("%c", &temp);
                        scanf("%[^\n]", data);
                        send(sock, data, 1024, 0);

                        int file = open(data, O_RDONLY);
                        if (!file) {
                            perror("can't open");
                            exit(EXIT_FAILURE);
                        }

                        int read_len;
                        while (true) {
                            memset(data, 0x00, 1024);
                            read_len = read(file, data, 1024);

                            if (read_len == 0) {
                                break;
                            }
                            else {
                                send(sock, data, read_len, 0);                               
                            }
                        }
                        close(file);
                        printf("Add sukses...\n\n");
                        continue;
                    }

                    else if (strcmp(command, "download") == 0) {
                        send(sock, command, 1024, 0);
                        printf("\nDownload\n");
                        memset(buffer, 0, sizeof(buffer));

                        scanf("%s", command);
                        send(sock, command, 1024, 0);
                        valread = read(sock, buffer, 1024);

                        if (strcmp(buffer, "File ready to download.\n") == 0) {
                            printf("Sedang download : %s\n", command);
                            int des_file = open(command, O_WRONLY | O_CREAT | O_EXCL, 0700);
                            if (!des_file) {
                                perror("can't open file");
                                exit(EXIT_FAILURE);
                            }

                            int file_read_len;
                            char buff[1024];

                            while (true) {
                                memset(buff, 0x00, 1024);
                                file_read_len = read(sock, buff, 1024);
                                write(des_file, buff, file_read_len);
                                break;
                            }
                        }

                        printf("Pesan : %s", buffer);
                        continue;
                    }

                    else if (strcmp(command, "delete") == 0) {
                        printf("\nDelete\n");
                        send(sock, command, 1024, 0);
                        memset(buffer, 0, sizeof(buffer));

                        scanf("%s", command);
                        send(sock, command, 1024, 0);
                        valread = read(sock, buffer, 1024);

                        printf("Pesan : %s", buffer);
                    }

                    else if (strcmp(command, "see") == 0) {
                        printf("\nSee\n");
                        send(sock, command, 1024, 0);
                        memset(buffer, 0, sizeof(buffer));

                        while (valread = read(sock, buffer, 1024)) {
                            if (strcmp(buffer, "e") == 0) {
                                break;
                            }
                            printf("%s", buffer);
                        }

                        continue; 
                    }

                    else if (strcmp(command, "find") == 0) {
                        printf("\nFind\n");
                        send(sock, command, 1024, 0);
                        memset(buffer, 0, sizeof(buffer));

                        scanf("%s", command);
                        send(sock, command, 1024, 0);

                        while (valread = read(sock, buffer, 1024)) {
                            if (strcmp(buffer, "e") == 0) {
                                break;
                            }
                            printf("%s", buffer);
                        }
                        continue;
                    }

                    else {
                        printf("Masukkan tidak dikenali\n");
                        continue;
                    }
                }
                memset(buffer, 0, sizeof(buffer));
            }

            else if (strcmp(buffer, "Login failed") == 0){
                printf("Login tidak cocok dengan akun pada server...\n\n");
            }

            memset(buffer, 0, sizeof(buffer));
        }

        else if (strcmp(command, "register") == 0) {
            send(sock, "register", 1024, 0);

            printf("\nRegister\n");
            printf("Username: ");
            scanf("%c", &temp);
            scanf("%[^\n]", name);
            send(sock, name, 1024, 0);

            printf("Password: ");
            scanf("%c", &temp);
            scanf("%[^\n]", password);
            send(sock, password, 1024, 0);

            printf("Register berhasil\n\n");
        }
        
        else if (strcmp(command, "quit") == 0) {
            send(sock, "quit", 1024, 0);
            printf("closing\n");
            return 0;
        }

        else {
            printf("Tidak dapat mengenali input...\n\n");
        }

        send(sock, "return", 1024, 0);
    }



    return 0;
}
