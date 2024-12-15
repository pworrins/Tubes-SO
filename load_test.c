#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 5050
#define MAX_CLIENTS 10
#define MESSAGE_INTERVAL 1 // Interval pengiriman pesan (dalam detik)
#define TEST_DURATION 10 // Durasi pengujian (dalam detik)

int create_client_socket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_HOST, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        return -1;
    }

    return sock;
}

int main() {
    int client_sockets[MAX_CLIENTS];
    fd_set read_fds, write_fds;
    char buffer[1024];

    // Membuat soket untuk semua klien
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = create_client_socket();
        if (client_sockets[i] < 0) {
            fprintf(stderr, "Failed to create socket for client %d\n", i + 1);
            return EXIT_FAILURE;
        }
        printf("Client %d connected to the server.\n", i + 1);
    }

    for (int t = 0; t < TEST_DURATION; t++) {
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);

        // Menambahkan soket klien ke set
        int max_fd = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            FD_SET(client_sockets[i], &read_fds);
            FD_SET(client_sockets[i], &write_fds);
            if (client_sockets[i] > max_fd) {
                max_fd = client_sockets[i];
            }
        }

        // Multiplexing dengan select()
        int activity = select(max_fd + 1, &read_fds, &write_fds, NULL, NULL);
        if (activity < 0) {
            perror("Select error");
            break;
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (FD_ISSET(client_sockets[i], &write_fds)) {
                // Mengirim pesan
                snprintf(buffer, sizeof(buffer), "Message %d from client %d", t, i + 1);
                send(client_sockets[i], buffer, strlen(buffer), 0);
                printf("Client %d sent: %s\n", i + 1, buffer);
            }

            if (FD_ISSET(client_sockets[i], &read_fds)) {
                // Menerima pesan
                int bytes_received = recv(client_sockets[i], buffer, sizeof(buffer) - 1, 0);
                if (bytes_received > 0) {
                    buffer[bytes_received] = '\0';
                    printf("Client %d received: %s\n", i + 1, buffer);
                }
            }
        }

        sleep(MESSAGE_INTERVAL);
    }

    // Menutup semua soket
    for (int i = 0; i < MAX_CLIENTS; i++) {
        close(client_sockets[i]);
        printf("Client %d disconnected.\n", i + 1);
    }

    printf("Load test completed.\n");
    return 0;
}
