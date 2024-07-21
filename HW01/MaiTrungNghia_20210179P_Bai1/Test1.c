#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h> // Bao gồm errno.h

#define BUFF_SIZE 1024
#define SERVER_PORT 5500
#define SERVER_ADDR "127.0.0.1"
#define ENDING_DELIMITER "\r\n"

int main()
{
    char buff[BUFF_SIZE];
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_ADDR, &(serverAddr.sin_addr.s_addr));

    int connSock;
    connSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct timeval tv;
    tv.tv_sec = 2;  // 2 seconds
    tv.tv_usec = 0; // 0 microseconds
    setsockopt(connSock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));

    if (connect(connSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("\nError: %d\n", errno);
        close(connSock);
        return 0;
    }

    char sBuff[BUFF_SIZE], rBuff[BUFF_SIZE];
    int ret;

    ret = recv(connSock, rBuff, BUFF_SIZE, 0);

    /******Test1***/
    strcpy(sBuff, "123");
    strcat(sBuff, ENDING_DELIMITER);
    strcat(sBuff, "a@b");
    strcat(sBuff, ENDING_DELIMITER);
    strcat(sBuff, "456");
    strcat(sBuff, ENDING_DELIMITER);

    ret = send(connSock, sBuff, strlen(sBuff), 0);
    ret = recv(connSock, rBuff, BUFF_SIZE, 0);
    if (ret < 0)
        printf("Stream test fail!\n");
    else {
        rBuff[ret] = 0;
        printf("Main: %s-->%s\n", sBuff, rBuff);
    }

    ret = recv(connSock, rBuff, BUFF_SIZE, 0);
    if (ret < 0)
        printf("Stream test fail!\n");
    else {
        rBuff[ret] = 0;
        printf("Main: %s-->%s\n", sBuff, rBuff);
    }

    ret = recv(connSock, rBuff, BUFF_SIZE, 0);
    if (ret < 0)
        printf("Stream test fail!\n");
    else {
        rBuff[ret] = 0;
        printf("Main: %s-->%s\n", sBuff, rBuff);
    }

    /******Test2***/
    strcpy(sBuff, "123");
    ret = send(connSock, sBuff, strlen(sBuff), 0);
    strcpy(sBuff, ENDING_DELIMITER);
    ret = send(connSock, sBuff, strlen(sBuff), 0);
    ret = recv(connSock, rBuff, BUFF_SIZE, 0);
    if (ret < 0)
        printf("Stream test 2 fail!\n");
    else {
        rBuff[ret] = 0;
        printf("Main: %s-->%s\n", sBuff, rBuff);
    }

    close(connSock);

    return 0;
}
