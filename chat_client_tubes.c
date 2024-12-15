/*
 * Client Chat dalam C (Obrolan Grup Real-time)
 *
 * Fitur:
 * - Terhubung ke server chat
 * - Mengirim dan menerima pesan secara real-time
 * - Penggunaan select() untuk menangani input pengguna dan pesan server
 *
 * Catatan:
 * - Program ini berfungsi sebagai klien yang terhubung ke server chat.
 * - Input dari pengguna dikirim ke server, dan pesan yang diterima dari server ditampilkan di terminal.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>

#define PORT 5050                   // Port server
#define BUFFER_SIZE 1024            // Ukuran buffer untuk pesan
#define USERNAME_SIZE 50            // Ukuran buffer untuk username

// Fungsi untuk memvalidasi input
void sanitize_input(char *input, size_t size) {
    input[strcspn(input, "\n")] = '\0'; // Menghapus karakter newline
    if (strlen(input) >= size) {
        fprintf(stderr, "Peringatan: Input terlalu panjang, dipotong hingga batas.\n");
        input[size - 1] = '\0'; // Memastikan input sesuai ukuran buffer
    }
}

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server_addr; // Struktur untuk alamat server
    char buffer[BUFFER_SIZE];       // Buffer untuk pesan
    char username[USERNAME_SIZE];   // Buffer untuk username
    fd_set readfds;                 // Set file descriptor untuk select()

    // 1. Memvalidasi argumen program
    if (argc < 2) {
        fprintf(stderr, "Penggunaan: %s <alamat_server>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 2. Membuat socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Gagal membuat socket");
        exit(EXIT_FAILURE);
    }

    // 3. Konfigurasi alamat server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Alamat server tidak valid: %s\n", argv[1]);
        close(sock);
        exit(EXIT_FAILURE);
    }

    // 4. Menghubungkan ke server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Gagal terhubung ke server");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Terhubung ke server %s:%d\n", argv[1], PORT);

    // 5. Mengirim username
    printf("Masukkan username Anda: ");
    fgets(username, sizeof(username), stdin);
    sanitize_input(username, sizeof(username));

    if (send(sock, username, strlen(username), 0) == -1) {
        perror("Gagal mengirim username");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Selamat datang, %s! Anda dapat mulai mengobrol.\n", username);

    while (1) {
        // Membersihkan set dan menambahkan file descriptor
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds); // Tambahkan input standar
        FD_SET(sock, &readfds);        // Tambahkan socket server

        int max_sd = sock;

        // Menunggu aktivitas
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0 && errno != EINTR) {
            perror("Kesalahan pada select()");
            break;
        }

        // Memeriksa apakah ada input dari pengguna
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            fgets(buffer, sizeof(buffer), stdin);
            sanitize_input(buffer, sizeof(buffer));

            if (strlen(buffer) == 0)
                continue;

            // Mengirim pesan ke server
            if (send(sock, buffer, strlen(buffer), 0) == -1) {
                perror("Gagal mengirim pesan");
                break;
            }
        }

        // Memeriksa apakah ada pesan dari server
        if (FD_ISSET(sock, &readfds)) {
            ssize_t bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0'; // Menambahkan null-terminator
                printf("%s\n", buffer);       // Menampilkan pesan
            } else if (bytes_received == 0) {
                printf("Server terputus.\n");
                break;
            } else {
                perror("Kesalahan saat menerima pesan");
                break;
            }
        }
    }

    close(sock); // Menutup koneksi socket
    return 0;
}
