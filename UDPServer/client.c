/* 客户端 */
#include <stdio.h>
#include <error.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

#define SERVER_PORT 8083
#define LOCAL_IPADDRESS "192.168.241.132"

#define BUFFER_SIZE 24

int main(int argc, char const *argv[])
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1)
    {
        perror("sorket error");
        exit(-1);
    }

    /* 连接 */
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    serverAddress.sin_addr.s_addr = inet_addr(LOCAL_IPADDRESS);

    socklen_t serverAddressLen = sizeof(serverAddress);

    char sendBuffer[BUFFER_SIZE] = "aBcDe";
    char acceptBuffer[BUFFER_SIZE];
    memset(acceptBuffer, 0, sizeof(acceptBuffer));


    printf("客户端发送：%s\n", sendBuffer);
    sendto(sockfd, sendBuffer, sizeof(sendBuffer), 0, (struct sockaddr*)&serverAddress, serverAddressLen);
    sleep(3);

    recvfrom(sockfd, acceptBuffer, sizeof(acceptBuffer), 0, (struct sockaddr*)&serverAddress, &serverAddressLen);
    printf("客户端接收：%s\n", acceptBuffer);
    sleep(1);

    


    return 0;
}
