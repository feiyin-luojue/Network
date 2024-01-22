/* 服务器 */
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

char* chang_a_to_A(char *ch)    //将小写字母转换为大写字母
{
    char *p_ch = ch;
    int idx = 0;
    while (p_ch[idx] != '\0')
    {   
        if (p_ch[idx] >= 'A' && p_ch[idx] <= 'Z')
        {
            idx++;
        }
        else
        {
            p_ch[idx] = (p_ch[idx] - ('a' - 'A'));
            idx++;
        }
    }
    return ch;
}

int main(int argc, char const *argv[])
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1)
    {
        perror("sorket error");
        exit(-1);
    }

    /* 设置端口复用 */
    int enableOpt = 1;
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&enableOpt, sizeof(enableOpt));
    if (ret == -1)
    {
        perror("setsockopt error");
        exit(-1);
    }

    struct sockaddr_in localAddress;
    memset(&localAddress, 0, sizeof(localAddress));

    localAddress.sin_family = AF_INET;
    localAddress.sin_port = htons(SERVER_PORT);
    localAddress.sin_addr.s_addr = inet_addr(LOCAL_IPADDRESS);

    socklen_t localAddressLen = sizeof(localAddress);
    ret = bind(sockfd, (struct sockaddr*)&localAddress, localAddressLen);
    if (ret == -1)
    {
        perror("bind error");
        exit(-1);
    }

    char recvbuffer[BUFFER_SIZE];
    char sendBuffer[BUFFER_SIZE];
    memset(recvbuffer, 0, sizeof(recvbuffer));
    memset(sendBuffer, 0, sizeof(sendBuffer));

    recvfrom(sockfd, recvbuffer, sizeof(recvbuffer), 0, (struct sockaddr*)&localAddress, &localAddressLen);
    printf("服务器接收：%s\n", recvbuffer);

    strcpy(sendBuffer, chang_a_to_A(recvbuffer));

    printf("服务器发送：%s\n", sendBuffer);
    sendto(sockfd, sendBuffer, sizeof(sendBuffer), 0, (struct sockaddr*)&localAddress, localAddressLen);
    
    sleep(5);


    close(sockfd);
    return 0;
}
