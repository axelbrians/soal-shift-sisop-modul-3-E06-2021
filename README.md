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