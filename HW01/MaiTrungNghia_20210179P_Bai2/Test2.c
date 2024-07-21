#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h> // Bao gồm errno.h

#define SERVER_PORT 5500
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 2048
#define MAX_CLIENT 4096

#define SUCCESS_LOGIN "10"
#define SUCCESS_POST "20"
#define SUCCESS_LOGOUT "30"

int main()
{
    char buff[BUFF_SIZE];
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_ADDR, &(serverAddr.sin_addr.s_addr));

    int connSock;
    connSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connect(connSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) {
        printf("\nError: %d", errno);
        close(connSock);
        return 0;
    }

    char sBuff[BUFF_SIZE], rBuff[BUFF_SIZE];
    int ret;

    sBuff[0] = 0;
    sBuff[1] = 0;
    sBuff[2] = 0;
    sBuff[3] = 3;
    sBuff[4] = '1';
    sBuff[5] = '2';
    sBuff[6] = '3';

    sBuff[7] = 0;
    sBuff[8] = 0;
    sBuff[9] = 0;
    sBuff[10] = 3;
    sBuff[11] = 'a';
    sBuff[12] = '@';
    sBuff[13] = 'c';

    sBuff[14] = 0;
    sBuff[15] = 0;
    sBuff[16] = 0;
    sBuff[17] = 2;
    sBuff[18] = '5';
    sBuff[19] = '6';
    
    ret = send(connSock, sBuff, 20, 0);
    usleep(100000); // Sleep 100ms

    ret = recv(connSock, rBuff, BUFF_SIZE, 0);
    if (ret < 0) {
        printf("Stream test fail!\n");
    } else {
        rBuff[ret] = 0;
        printf("Main: %s-->%s\n", sBuff, rBuff);
    }

    ret = recv(connSock, rBuff, BUFF_SIZE, 0);
    if (ret < 0) {
        printf("Stream test fail!\n");
    } else {
        rBuff[ret] = 0;
        printf("Main: %s-->%s\n", sBuff, rBuff);
    }

    ret = recv(connSock, rBuff, BUFF_SIZE, 0);
    if (ret < 0) {
        printf("Stream test fail!\n");
    } else {
        rBuff[ret] = 0;
        printf("Main: %s-->%s\n", sBuff, rBuff);
    }

    close(connSock);

    return 0;
}
