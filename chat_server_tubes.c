/*
 * Server Chat dalam C (Obrolan Grup Real-time dengan Logging)
 *
 * Fitur:
 * - Penanganan multi-klien menggunakan fork()
 * - Layanan echo pesan secara real-time
 * - Autentikasi pengguna dengan username
 * - Tanda waktu pada setiap pesan
 * - Notifikasi untuk koneksi dan pemutusan koneksi klien
 * - Logging obrolan ke file
 *
 * Catatan:
 * - Program ini membuat server yang mendengarkan koneksi klien.
 * - Setiap klien yang terhubung dijalankan dalam proses anak menggunakan fork().
 * - Pesan dicetak di terminal server dan dicatat ke dalam file log.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define PORT 5050                   // Nomor port untuk server
#define BUFFER_SIZE 1024            // Ukuran buffer untuk pesan masuk
#define MAX_CLIENTS 10              // Jumlah maksimum klien yang dapat ditangani server
#define USERNAME_SIZE 50            // Ukuran maksimum username klien
#define FORMATTED_BUFFER_SIZE 2048  // Ukuran buffer untuk pesan terformat (termasuk timestamp, username, dan pesan)
#define LOG_FILE "chat_log.txt"     // Nama file log untuk menyimpan pesan obrolan

// Deklarasi fungsi
void handle_client(int client_socket);      // Fungsi untuk menangani komunikasi dengan klien
char *get_current_timestamp();              // Fungsi untuk mendapatkan timestamp saat ini
void log_message(const char *message);      // Fungsi untuk mencatat pesan ke file

/**
 * Fungsi utama: Memulai server, mendengarkan koneksi klien, dan membuat proses anak untuk setiap klien.
 */
int main() {
    int server_socket, new_socket;              // Deskriptor socket server dan klien
    struct sockaddr_in server_addr, client_addr; // Struktur untuk alamat server dan klien
    socklen_t client_addr_len = sizeof(client_addr); // Ukuran struktur alamat klien

    // 1. Membuat socket server
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Gagal membuat socket");
        exit(EXIT_FAILURE);
    }

    // 2. Konfigurasi struktur alamat server
    server_addr.sin_family = AF_INET;            // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;    // Menerima koneksi dari alamat IP mana pun
    server_addr.sin_port = htons(PORT);          // Konversi nomor port ke format network byte order

    // 3. Mengikat socket ke IP dan port yang ditentukan
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Gagal melakukan bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // 4. Mendengarkan koneksi masuk
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Gagal mendengarkan koneksi");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server berjalan pada port %d...\n", PORT);

    // 5. Loop utama untuk menerima koneksi klien baru
    while (1) {
        // Menerima koneksi klien baru
        new_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (new_socket == -1) {
            perror("Gagal menerima koneksi");
            continue; // Lanjutkan menerima koneksi lain
        }

        // Membuat proses baru untuk menangani klien
        if (fork() == 0) {
            // Proses anak: menangani klien
            close(server_socket); // Menutup socket listening di proses anak
            handle_client(new_socket);
            close(new_socket);
            exit(0); // Keluar dari proses anak setelah menangani klien
        }

        // Proses induk: menutup socket klien dan melanjutkan menerima koneksi
        close(new_socket);
    }

    close(server_socket); // Menutup socket server saat selesai (tidak tercapai dalam loop ini)
    return 0;
}

/**
 * Fungsi untuk menangani komunikasi dengan klien yang terhubung.
 *
 * @param client_socket Deskriptor socket untuk klien yang terhubung.
 */
void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];            // Buffer untuk menyimpan pesan masuk
    char username[USERNAME_SIZE];        // Buffer untuk menyimpan username klien

    // 1. Menerima username dari klien
    if (recv(client_socket, username, sizeof(username) - 1, 0) <= 0) {
        perror("Gagal menerima username");
        return;
    }
    username[USERNAME_SIZE - 1] = '\0'; // Pastikan string null-terminated
    username[strcspn(username, "\n")] = '\0'; // Menghapus karakter newline jika ada

    // 2. Memberitahukan koneksi klien baru
    char connect_message[FORMATTED_BUFFER_SIZE];
    snprintf(connect_message, sizeof(connect_message), "[%s] %s telah terhubung.\n", get_current_timestamp(), username);
    printf("%s", connect_message);  // Menampilkan pesan ke terminal server
    log_message(connect_message);   // Mencatat pesan ke file log

    // 3. Loop komunikasi untuk menerima pesan dari klien
    while (1) {
        // Menerima pesan dari klien
        ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0'; // Null-terminate pesan

            // Format pesan dengan timestamp dan username
            char formatted_message[FORMATTED_BUFFER_SIZE];
            snprintf(formatted_message, sizeof(formatted_message), "[%s] %s: %s\n",
                     get_current_timestamp(), username, buffer);

            // Mengirim kembali pesan ke klien (echo)
            send(client_socket, formatted_message, strlen(formatted_message), 0);

            // Menampilkan dan mencatat pesan
            printf("%s", formatted_message);  // Menampilkan pesan di terminal server
            log_message(formatted_message);  // Mencatat pesan ke file
        } else {
            // Penanganan pemutusan koneksi klien
            if (bytes_read == 0) {
                snprintf(connect_message, sizeof(connect_message), "[%s] %s telah terputus.\n", get_current_timestamp(), username);
                printf("%s", connect_message); // Memberitahukan pemutusan koneksi di terminal server
                log_message(connect_message);  // Mencatat pemutusan koneksi
            } else {
                perror("Gagal menerima pesan");
            }
            break; // Keluar dari loop komunikasi
        }
    }
}

/**
 * Fungsi untuk mendapatkan timestamp saat ini dalam format "YYYY-MM-DD HH:MM:SS".
 *
 * @return String yang berisi timestamp saat ini.
 */
char *get_current_timestamp() {
    static char timestamp[20]; // Buffer statis untuk menyimpan timestamp
    time_t now = time(NULL);   // Mendapatkan waktu saat ini
    struct tm *t = localtime(&now); // Mengonversi ke struktur waktu lokal
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t); // Format waktu
    return timestamp; // Mengembalikan timestamp yang sudah diformat
}

/**
 * Fungsi untuk mencatat pesan ke file.
 *
 * @param message Pesan yang akan dicatat.
 */
void log_message(const char *message) {
    FILE *log_file = fopen(LOG_FILE, "a"); // Membuka file log dalam mode append
    if (log_file == NULL) {
        perror("Gagal membuka file log");
        return;
    }
    fprintf(log_file, "%s", message); // Menulis pesan ke file log
    fclose(log_file); // Menutup file log
}
