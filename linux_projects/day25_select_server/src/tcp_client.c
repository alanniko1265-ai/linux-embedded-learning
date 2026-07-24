#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9000
#define BUFFER_SIZE 128

int main(void)
{
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(client_fd);
        return 1;
    }

    if (connect(client_fd, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect");
        close(client_fd);
        return 1;
    }

    printf("connected to %s:%d\n", SERVER_IP, SERVER_PORT);
    printf("enter a message, or enter quit to exit\n");

    char send_buffer[BUFFER_SIZE];
    char recv_buffer[BUFFER_SIZE];

    while (fgets(send_buffer, sizeof(send_buffer), stdin) != NULL) {
        size_t message_len = strlen(send_buffer);

        ssize_t sent = send(client_fd, send_buffer, message_len, 0);
        if (sent < 0) {
            perror("send");
            break;
        }

        ssize_t n = recv(client_fd, recv_buffer,
                         sizeof(recv_buffer) - 1, 0);
        if (n < 0) {
            perror("recv");
            break;
        }

        if (n == 0) {
            printf("server disconnected\n");
            break;
        }

        recv_buffer[n] = '\0';
        printf("[client] echo: %s", recv_buffer);

        if (strcmp(send_buffer, "quit\n") == 0) {
            break;
        }
    }

    close(client_fd);
    return 0;
}
