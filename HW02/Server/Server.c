#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 5500
#define BUFFER_SIZE 2048
#define MAX_CLIENTS 4096

typedef struct {
    int socket;
    struct sockaddr_in address;
    int addr_len;
    char username[50];
    int logged_in;
} client_t;

client_t *clients[MAX_CLIENTS];
int account_count = 0;
typedef struct {
    char username[50];
    int status;
} account_t;

account_t accounts[100];

void *handle_client(void *arg);
void load_accounts();
int authenticate_user(char *username);
void send_message(client_t *client, char *message);
void broadcast_message(char *message, client_t *sender);

int main() {
    int sockfd, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    pthread_t tid;

    load_accounts();

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 10) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        addr_size = sizeof(new_addr);
        new_sock = accept(sockfd, (struct sockaddr *)&new_addr, &addr_size);
        if (new_sock < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        client_t *client = (client_t *)malloc(sizeof(client_t));
        client->socket = new_sock;
        client->address = new_addr;
        client->addr_len = addr_size;
        client->logged_in = 0;

        pthread_create(&tid, NULL, handle_client, (void *)client);
    }

    close(sockfd);
    return 0;
}

void *handle_client(void *arg) {
    client_t *client = (client_t *)arg;
    char buffer[BUFFER_SIZE];
    int len;

    while ((len = recv(client->socket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[len] = '\0';
        char *command = strtok(buffer, " ");
        if (strcmp(command, "USER") == 0) {
            char *username = strtok(NULL, " ");
            int status = authenticate_user(username);
            if (status == 10) {
                client->logged_in = 1;
                strcpy(client->username, username);
            }
            char response[3];
            sprintf(response, "%d", status);
            send_message(client, response);
        } else if (strcmp(command, "POST") == 0) {
            if (client->logged_in) {
                // Handle post article
                char *article = strtok(NULL, "\0");
                broadcast_message(article, client);
                send_message(client, "20 / đăng bài thành công");
            } else {
                send_message(client, "21 / nếu chưa đăng nhập");
            }
        } else if (strcmp(command, "BYE") == 0) {
            if (client->logged_in) {
                client->logged_in = 0;
                send_message(client, "30 / đăng xuất thành công");
            } else {
                send_message(client, "21 / nếu chưa đăng nhập");
            }
        } else {
            send_message(client, "99 / Khong xac dinh duoc yeu cau");
        }
    }

    close(client->socket);
    free(client);
    return NULL;
}

void load_accounts() {
    FILE *file = fopen("account.txt", "r");
    if (file == NULL) {
        perror("Could not open account file");
        exit(EXIT_FAILURE);
    }

    while (fscanf(file, "%s %d", accounts[account_count].username, &accounts[account_count].status) != EOF) {
        account_count++;
    }

    fclose(file);
}

int authenticate_user(char *username) {
    for (int i = 0; i < account_count; i++) {
        if (strcmp(accounts[i].username, username) == 0) {
            if (accounts[i].status == 0) {
                printf("tài khoản tồn tại và hoạt động");
                return 10; // Active account //
            } else {
                printf("tài khoản bị khóa");
                return 11; // Account locked //ếu 
            }
        }
    }
    printf("tài khoản không tồn tại");
    return 12; // Account does not exist //ếu 
}

void send_message(client_t *client, char *message) {
    send(client->socket, message, strlen(message), 0);
}

void broadcast_message(char *message, client_t *sender) {
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "%s: %s", sender->username, message);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] && clients[i]->logged_in) {
            send_message(clients[i], buffer);
        }
    }
}
