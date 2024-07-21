// TCP_Server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

int is_digit_string(const char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

int sum_of_digits(const char *str) {
    int sum = 0;
    for (int i = 0; i < strlen(str); i++) {
        sum += str[i] - '0';
    }
    return sum;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s PortNumber\n", argv[0]);
        exit(1);
    }

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char response[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(1);
    }

    // Loại bỏ setsockopt để tránh lỗi "Protocol not available"
    /*
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(1);
    }
    */

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(argv[1]));

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(1);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(1);
    }

    printf("Server is up and running, waiting for incoming connections...\n");

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            close(server_fd);
            exit(1);
        }

        int valread = read(new_socket, buffer, BUFFER_SIZE);
        buffer[valread] = '\0';

        printf("Received string: %s\n", buffer);

        if (is_digit_string(buffer)) {
            int sum = sum_of_digits(buffer);
            snprintf(response, BUFFER_SIZE, "Sum of digits: %d", sum);
        } else {
            snprintf(response, BUFFER_SIZE, "Failed: String contains non-number character.");
        }

        send(new_socket, response, strlen(response), 0);
        printf("Response sent: %s\n", response);

        close(new_socket);
    }

    close(server_fd);
    return 0;
}
