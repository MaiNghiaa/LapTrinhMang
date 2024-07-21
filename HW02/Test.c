// TCPClient.c : Defines the entry point for the console application.

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
#include <errno.h>

#define SERVER_PORT 5500
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 2048
#define MAX_CLIENT 4096

#define SUCCESS_LOGIN "10"
#define SUCCESS_POST "20"
#define SUCCESS_LOGOUT "30"

void *thread(void *param);
void *thread1(void *param);
void *thread2(void *param);

int main(int argc, char* argv[])
{
    //Step 2: Construct socket
    int client;
    client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //Step 3: Specify server address
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    //Step 4: Request to connect server
    if (connect(client, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) {
        printf("Error! Cannot connect server. %d\n", errno);
        return 0;
    }
    printf("Connected server!\n");
    int tv = 100; //Time-out interval: 100ms
    setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&tv), sizeof(int));
    //Step 5: Communicate with server
    char sBuff[BUFF_SIZE], rBuff[BUFF_SIZE];
    int ret;

    //Sequence testing
    strcpy(sBuff, "POST Hello\r\n");
    ret = send(client, sBuff, strlen(sBuff), 0);
    ret = recv(client, rBuff, BUFF_SIZE, 0);
    if (ret < 0)
        printf("Sequence test fail!\n");
    else {
        rBuff[ret] = 0;
        printf("Main: %s-->%s\n", sBuff, rBuff);
        if(strstr(rBuff, SUCCESS_POST)!=0)
            printf("Sequence test fail!\n");
    }

    //Function testing
    strcpy(sBuff, "USER ductq\r\n");
    ret = send(client, sBuff, strlen(sBuff), 0);
    ret = recv(client, rBuff, BUFF_SIZE, 0);
    rBuff[ret] = 0;
    printf("Main: %s-->%s\n\n", sBuff, rBuff);
    if(strstr(rBuff, SUCCESS_LOGIN)!=0)
        printf("Login test fail!\n");

    strcpy(sBuff, "USER admin\r\n");
    ret = send(client, sBuff, strlen(sBuff), 0);
    ret = recv(client, rBuff, BUFF_SIZE, 0);
    rBuff[ret] = 0;
    printf("Main: %s-->%s\n", sBuff, rBuff);
    if(strstr(rBuff, SUCCESS_LOGIN)==0)
        printf("Login test fail!\n");

    strcpy(sBuff, "USER tungbt\r\n");
    ret = send(client, sBuff, strlen(sBuff), 0);
    ret = recv(client, rBuff, BUFF_SIZE, 0);
    rBuff[ret] = 0;
    printf("Main: %s-->%s\n", sBuff, rBuff);
    if(strstr(rBuff, SUCCESS_LOGIN)!=0)
        printf("Login test fail!\n");

    strcpy(sBuff, "POST Hello\r\n");
    ret = send(client, sBuff, strlen(sBuff), 0);
    ret = recv(client, rBuff, BUFF_SIZE, 0);
    rBuff[ret] = 0;
    printf("Main: %s-->%s\n", sBuff, rBuff);
    if(strstr(rBuff, SUCCESS_POST)==0)
        printf("Post message test fail!\n");

    strcpy(sBuff, "BYE\r\n");
    ret = send(client, sBuff, strlen(sBuff), 0);
    ret = recv(client, rBuff, BUFF_SIZE, 0);
    rBuff[ret] = 0;
    printf("Main: %s-->%s\n", sBuff, rBuff);
    if(strstr(rBuff, SUCCESS_LOGOUT)==0)
        printf("Logout test fail!\n");

    //Syntax testing
    strcpy(sBuff, "USER \r\n");
    ret = send(client, sBuff, strlen(sBuff), 0);
    ret = recv(client, rBuff, BUFF_SIZE, 0);
    if (ret < 0)
        printf("Syntax test fail!\n");
    else {
        rBuff[ret] = 0;
        printf("Main: %s-->%s\n", sBuff, rBuff);
        if(strstr(rBuff, SUCCESS_LOGIN)!=0)
        printf("Login test fail!\n");
    }

    strcpy(sBuff, "foo\r\n");
    ret = send(client, sBuff, strlen(sBuff), 0);
    ret = recv(client, rBuff, BUFF_SIZE, 0);
    if (ret < 0)
        printf("Syntax test fail!\n");
    else {
        rBuff[ret] = 0;
        printf("Main: %s-->%s\n", sBuff, rBuff);
    }

    strcpy(sBuff, "USER tungbt\r\n");
    ret = send(client, sBuff, strlen(sBuff), 0);
    ret = recv(client, rBuff, BUFF_SIZE, 0);
    rBuff[ret] = 0;
    printf("Main: %s-->%s\n", sBuff, rBuff);

    //Stream testing
    strcpy(sBuff, "USER admin\r\nPOST Hello world\r\nPOST Test stream\r\n");
    ret = send(client, sBuff, strlen(sBuff), 0);
    ret = recv(client, rBuff, BUFF_SIZE, 0);
    if (ret < 0)
        printf("Stream test 1 fail!\n");
    else {
        rBuff[ret] = 0;
        printf("Main: %s-->%s\n", sBuff, rBuff);
    }

    ret = recv(client, rBuff, BUFF_SIZE, 0);
    if (ret < 0)
        printf("Stream test 1 fail!\n");
    else {
        rBuff[ret] = 0;
        printf("Main: %s-->%s\n", sBuff, rBuff);
    }

    ret = recv(client, rBuff, BUFF_SIZE, 0);
    if (ret < 0)
        printf("Stream test 1 fail!\n");
    else {
        rBuff[ret] = 0;
        printf("Main: %s-->%s\n", sBuff, rBuff);
    }

    strcpy(sBuff, "POST I am tungbt");
    ret = send(client, sBuff, strlen(sBuff), 0);
    ret = recv(client, rBuff, BUFF_SIZE, 0);
    if (ret > 0)
        printf("Stream test 2 fail!\n");

    strcpy(sBuff, "\r\n");
    ret = send(client, sBuff, strlen(sBuff), 0);
    ret = recv(client, rBuff, BUFF_SIZE, 0);
    if (ret < 0)
        printf("Stream test 2 fail!\n");
    else {
        rBuff[ret] = 0;
        printf("Main: %s-->%s\n", sBuff, rBuff);
    }

    // Concurency test 1
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, thread1, NULL);
    pthread_create(&tid2, NULL, thread2, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    close(client);

    return 0;
}

void *thread1(void *param)
{
    //Step 2: Construct socket
    int client;
    client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //Step 3: Specify server address
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    //Step 4: Request to connect server
    if (connect(client, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) {
        printf("Error! Cannot connect server. %d\n", errno);
        return 0;
    }
    printf("Connected server!\n");
    int tv = 100; //Time-out interval: 100ms
    setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&tv), sizeof(int));
    //Step 5: Communicate with server
    char sBuff[BUFF_SIZE], rBuff[BUFF_SIZE];
    int ret;

    //Function testing
    strcpy(sBuff, "USER ductq\r\n");
    ret = send(client, sBuff, strlen(sBuff), 0);
    ret = recv(client, rBuff, BUFF_SIZE, 0);
    rBuff[ret] = 0;
    printf("Thread 1: %s-->%s\n\n", sBuff, rBuff);
    if(strstr(rBuff, SUCCESS_LOGIN)!=0)
        printf("Login test fail!\n");

    strcpy(sBuff, "USER admin\r\n");
    ret = send(client, sBuff, strlen(sBuff), 0);
    ret = recv(client, rBuff, BUFF_SIZE, 0);
    rBuff[ret] = 0;
    printf("Thread 1: %s-->%s\n", sBuff, rBuff);
    if(strstr(rBuff, SUCCESS_LOGIN)==0)
        printf("Login test fail!\n");

    strcpy(sBuff, "USER tungbt\r\n");
    ret = send(client, sBuff, strlen(sBuff), 0);
    ret = recv(client, rBuff, BUFF_SIZE, 0);
    rBuff[ret] = 0;
    printf("Thread 1: %s-->%s\n", sBuff, rBuff);
    if(strstr(rBuff, SUCCESS_LOGIN)!=0)
        printf("Login test fail!\n");

    close(client);
    return NULL;
}

void *thread2(void *param)
{
    //Step 2: Construct socket
    int client;
    client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //Step 3: Specify server address
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    //Step 4: Request to connect server
    if (connect(client, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) {
        printf("Error! Cannot connect server. %d\n", errno);
        return 0;
    }
    printf("Connected server!\n");
    int tv = 100; //Time-out interval: 100ms
    setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&tv), sizeof(int));
    //Step 5: Communicate with server
    char sBuff[BUFF_SIZE], rBuff[BUFF_SIZE];
    int ret;

    //Function testing
    strcpy(sBuff, "USER ductq\r\n");
    ret = send(client, sBuff, strlen(sBuff), 0);
    ret = recv(client, rBuff, BUFF_SIZE, 0);
    rBuff[ret] = 0;
    printf("Thread 2: %s-->%s\n\n", sBuff, rBuff);
    if(strstr(rBuff, SUCCESS_LOGIN)!=0)
        printf("Login test fail!\n");

    strcpy(sBuff, "USER admin\r\n");
    ret = send(client, sBuff, strlen(sBuff), 0);
    ret = recv(client, rBuff, BUFF_SIZE, 0);
    rBuff[ret] = 0;
    printf("Thread 2: %s-->%s\n", sBuff, rBuff);
    if(strstr(rBuff, SUCCESS_LOGIN)==0)
        printf("Login test fail!\n");

    strcpy(sBuff, "USER tungbt\r\n");
    ret = send(client, sBuff, strlen(sBuff), 0);
    ret = recv(client, rBuff, BUFF_SIZE, 0);
    rBuff[ret] = 0;
    printf("Thread 2: %s-->%s\n", sBuff, rBuff);
    if(strstr(rBuff, SUCCESS_LOGIN)!=0)
        printf("Login test fail!\n");

    close(client);
    return NULL;
}
