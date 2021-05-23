# Soal Shift Sisop Modul 3 2021


Kelompok E-06
- Fajar Satria (05111940000083)
- Axel Briano Suherik (05111940000137)
- Ikhlasul Amal Rivel (05111940000145)
# Soal Shift Modul 3 (Kelompok E6)

**Soal Shift Modul 3 Sistem Operasi 2021:**
* [Soal 1](https://github.com/axelbrians/soal-shift-sisop-modul-3-E06-2021#Soal-1)
* [Soal 2](https://github.com/axelbrians/soal-shift-sisop-modul-3-E06-2021#Soal-2)
* [Soal 3](https://github.com/axelbrians/soal-shift-sisop-modul-3-E06-2021#Soal-3)

# Soal 1

## Soal 1
### A.

#### Deskripsi soal
-   Server dapat menerima lebih dari 1 koneksi Client sekaligus, namun hanya 1 Client yang dapat mengakses server. Client ke-2 dan seterusnya harus menunggu sampai Client pertama log out. Server dapat menerima kiriman dari Client berupa `password` dan `password2` yang berguna untuk register atau login. Semua register yang terjadi tersimpan pada file `akun.txt` dan melakukan pengecekan pada saat Client login.
-   Client diminta  `password` dan `password2`  saat register dan login.
```
id:password
id2:password2
```
#### Solusi soal
-   Multi-connections Client-Server.
    -   Gunakan template koneksi socket Client dan Server yang ada di modul 3.
    -   Manfaatkan thread untuk dapat memungkinkan terjadinya multi-connections pada program server.
	```
	// Server
	while(new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)){
		printf("Client tersambung dengan server...\n\n");
		pthread_create(&tid, NULL, &process, &new_socket);
		if (new_socket < 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}
	}
	```
	-   Variabel `connection` berguna untuk menentukan apakah ada atau tidaknya Client yang mengakses server.
    -   Tentukan apakah Client akan di terima atau di tahan aksesnya ke fungsi program berdasarkan nilai dari variabel `connection` pada server.
	```
	// Server
    send(new_socket, "unready", 1024, 0);
    while(connection){
        valread = read(new_socket, buffer, 1024);
        send(new_socket, "unready", 1024, 0);
    }
    connection = 1;
    valread = read(new_socket, buffer, 1024);
    send(new_socket, "ready", 1024, 0);
	  ```      
    -   Client akan memeriksa pesan yang diberikan dari Server.
	```
	/// Client
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
	```
- Register
	- Server akan membuat dan atau mengedit `akun.txt` sesuai dengan inputan Client
	```
	/// Server
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
	```
	- Client mengirimkan `password` dan `password2` ke server
	```
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
	```
	
### B.

#### Deskripsi Soal

-  Server terdapat  `files.tsv`  yang menyimpan
```
    path file di server
    publisher
    tahunpublikasi
```
- Setiap ada operasi add dan delete files `files.tsv`
-  Folder  `FILES`  yang menyimpan semua file yang dikirimkan oleh Client, otomatis dibuat saat server dijalankan

#### Solusi Soal

-   `files.tsv`
    -   pada operasi penambahan file, tambahkan kode yang akan membuat file  `files.tsv`
    -   perubahan pada file tersebut akan dijelaskan lebih lanjut pada bagian soal selanjutnya
-   folder  `FILES`
    -   buat folder  `FILES`  setelah Server dijalankan

### C.

#### Deskripsi Soal
-   Server dapat mengimpan di folder  `FILES`
```
    File1.ekstensi
    File2.ekstensi
```
-   Client mengirimkan command  `add`  dan Client memasukkan detail file
```
    Publisher:
    Tahun Publikasi:
    Filepath:
```
-   `files.tsv`  berguna untuk menyimpan data file yang dikirimkan ke Server

#### Solusi Soal
- Pada server, server menerima dan menyimpan file yang diberikan client. Lalu mengapdatenya pada file `files.tsv`
```
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
```
- Pada client, client mengirimkan detail dari file lalu membuka file dan mengirimkan file ke Server.
```
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
```
#### Referensi

-   [https://stackoverflow.com/questions/40786888/send-file-from-client-to-server-using-socket-in-c-on-linux](https://stackoverflow.com/questions/40786888/send-file-from-client-to-server-using-socket-in-c-on-linux)
-   [https://stackoverflow.com/questions/3501338/c-read-file-line-by-line](https://stackoverflow.com/questions/3501338/c-read-file-line-by-line)

### D.

#### Deskripsi Soal

-   Client dapat mendownload file yang ada pada server

#### Solusi Soal
   -   Solusi dari download sangat serupa dengan solusi dari add, hanya saja peran Client-Server dibalik
-   Client mengirimkan command  `download nama_file.extension`
        
-   Server menerima command dan mencari nama file di  `files.tsv`
        
-   Server akan mengirimkan file ke Client jika ada file yang diminta

### E.

#### Deskripsi Soal
-   Client mengirimkan command  `delete nama_file.extension`
-   Jika file ada, file hanya di-rename menjadi  `old-nama_file.extension`
-   Hapus file dari list pada  `files.tsv`

#### Solusi Soal
-   Server menerima command dan mengecek apakah ada file tersebut di  `files.tsv`, jika ada file yang ingin dihapus, rename nama file, selama membaca  `files.tsv`, simpan pada baris ke-berapa terdapat detail dari file yang dihapus, buat fungsi untuk menghapus baris tertentu di  `files.tsv`.
```
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
```
-   Client mengirimkan command delete dan nama file yang ingin di-delete
```
	               else if (strcmp(command, "delete") == 0) {
                        printf("\nDelete\n");
                        send(sock, command, 1024, 0);
                        memset(buffer, 0, sizeof(buffer));

                        scanf("%s", command);
                        send(sock, command, 1024, 0);
                        valread = read(sock, buffer, 1024);

                        printf("Pesan : %s", buffer);
                    }
```

### F.

#### Deskripsi Soal

-   Client mengirimkan command  `see`
-   Format output di Client sebagai berikut:
```
    Nama:
    Publisher:
    Tahun publishing:
    Ekstensi File :
    Filepath :
```
```
    Nama:
    Publisher:
    Tahun publishing:
    Ekstensi File :
    Filepath :
```
#### Solusi Soal

-   Client mengirimkan command  `see`
    -   Client-Server kirim-terima command
    -   Server membuka  `files.tsv`
-   Output di client
    -   Server membaca  `files.tsv`  line-by-line
    -   Server memproses tiap line agar mendapatkan informasi yang diperlukan
    -   Selama masih ada baris untuk diproses, kirim informasi ke Client

### G.
#### Deskripsi Soal

-   Client menerima detail files yang mengandung string yang dikirimkan
-   Format output sama dengan 1.f

#### Solusi Soal

-   Client send command
    -   Client-Server kirim-terima
    -   Server membaca  `files.tsv`  line-by-line
    -   Manfaatkan strstr agar sesuai dengan apa yang diminta soal
	-   Format output sama dengan 1.f

### H.
#### Deskripsi Soal

-   tiap operasi add dan delete, log di  `running.log`
-   format  `running.log`
	```
	Tambah : File1.ektensi (id:pass)
	Hapus : File2.ektensi (id:pass)
	```
    
 
#### Solusi Soal
-   buat fungsi untuk memudahkan penambahan log di  `running.log`     
-   ketika Server memproses add dan delete, panggil fungsi tersebut
```
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
```

### Kendala Soal
-   Kesulitan Multi-connections Client-Server.
-   Kurang pahamnya cara pemakaian fungsi dari `string.h`

# Soal 2

# Soal 3
* Mengkategorikan file pada path tertentu berdasarkan extensionnya.
* Untuk extension file tidak case sensitive (**PNG** == **png**).
* Pengambilan extension file dimulai dari titik pertama. Misal **file.tar.gz** extensionnya adalah **tar.gz**.
* Tidak boleh menggunakan `exec() dan system()`.
* Sub-soal b dan c bersifat **rekursif**.

## Subsoal a
### Penjelasan
* Program bisa mengelompokan file dengan option `-f`.
* Format input adalah `./{nama script} -f [space separated full file path]`. Contoh `./soal3 -f /downloads/foto.jpg /downloads/installer.deb /downloads/venus.svg`.
* Mengelompokan file dalam parameter command ke folder masing - masing sesuai extensionnya.
* Lokasi folder pengelompokkan tergantung dari directory script.
* Jika berhasil untuk memindahkan file, tampilkan `File <no urut>: berhasil dikaregorikan`.
* Jika gagal, tampilkan `File <no urut>: Sad, gagal :(`.

### Penyelesaian
1. Pastikan input command valid
2. Looping membuat thread baru untuk tiap parameter path file.
3. Check pada path tersebut valid atau tidak, dalam artinya filenya ada atau di path tersebut bukan merupakan directory.
4. Untuk tiap thread bertugas memecah path file ke path yang baru, dan memindahkannya ke directory sesuai extensionnya.
5. Jika gagal, termasuk jika path invalid tampilkan `File <no urut>: Sad, gagal :(`.
6. Jika berhasil tampilkan `File <no urut>: berhasil dikaregorikan`.

## Subsoal b
### Penjelasan
* Program bisa mengelompokkan file pada suatu directory dengan option `-d`.
* Format input adalah `./{nama script} -d <path directory>`.
* Hanya bisa menerima satu path directory tidak seperti subsoal a.
* Jika berhasil, tampilkan `Direktori sukses disimpan!`.
* Jika gagal, tampilkan `Yah, gagal disimpan :(`.

### Penyelesaian
* List nama - nama file pada directory tersebut secara rekursif.
* File bertipe unknown didahulukan untuk menghindari konflik saat membuat directory.
* Jalankan fungsi pada subsoal a untuk tiap path file yang sudah di list secara rekursif.

## Subsoal c
### Penjelasan
* Program bisa mengelompokkan directory pada path script itu berada dengan `\*`.
* Script juga ikut dipindahkan.

### Penyelesaian
* Jalankan fungsi pada subsoal b. Namun path directorynya berdasarkan tempat script berada.

## Subsoal d
### Penjelasan
* File tanpa extension masuk kedalam kategori `unknown`.
* File hidden (memiliki prefix '.') dipindahkan ke `hidden`.

### Penyelesaian
* Pada fungsi `*move_file()` terdapat blok kode seperti berikut.
```
    if (file_name[0] == '.') { // case for hidden file start with .
        strcpy(file_ext, "Hidden");
    } else if (buffer != NULL) { // for normal case
        strcpy(file_ext, buffer);
    } else {
        strcpy(file_ext, "Unknown");
    }
```
file_ext merupakan variabel yang menampung tipe extension atau tujuan dari file tersebut akan dipindahkan.

## Subsoal e
### Penjelasan
* Setiap file yang akan dipindahkan harus dijalankan pada satu thread masing - masing.

### Penyelesaian
* Tiap file selalu dibuatkan thread baru untuk memindahkannya. Bisa dilihat pada fungsi `option_file()`.

### Kendala
* Masalah pada file tipe unknown yang memblok pembuatan folder baru dengan nama yang sama. Mengatasinya dengan mendahulukan pemindahan file dengan tipe unknown.
