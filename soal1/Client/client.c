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

bool equal(char *temp, char *temp1){
    for(int i = 0; temp[i] != '\0'; i++){
        if(toupper(temp[i]) != toupper(temp1[i]))
            return false;
    }
    return true;
}

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
    while(true){
        printf("Menghubungkan...\n\n");
        valread = read(sock, buffer, 1024);
        while(strcmp("ready", buffer) != 0){
            memset(buffer, 0, 1024);
            valread = read(sock, buffer, 1024);
        }

        memset(buffer, 0, 1024);

        printf("Terhubung...\n\n");
        printf("Menu :\n1. Register\n2. Login\n3. Quit\n\n");
        printf("Masukkan : ");  scanf("%s", command);

        if (equal(command, "register")) {
            send(sock, "register", 1024, 0);
            printf("Register\n\n");
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

        else if (equal(command, "login")) {
            send(sock, "login", 1024, 0);
            
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

            if (equal(buffer, "Login success")) {
                while (true) {
                    printf("1. Logout\n2. Add\n3. Download [File]\n4. Delete [File]\n5. See\n6. Find [File]\n");
                    scanf("%s", command);

                    if (equal(command, "logout")) {
                        send(sock, command, 1024, 0);
                        break;
                    }
                    //-------------------------------------------------------------------------------------
                    else {
                        printf("command not recognized\n");
                        continue;
                    }
                }
                memset(buffer, 0, sizeof(buffer));
            }

            else if (equal(buffer, "Login failed")){
                printf("Login tidak cocok dengan akun pada server...\n\n");
            }

            memset(buffer, 0, sizeof(buffer));
        }
        
        else if (equal(command, "quit")) {
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
