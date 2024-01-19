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


#define SERVER_PORT 8080
#define MAX_LISTEN  128
#define SERVER_IPADDRESS "192.168.241.132"
#define BUFFER_SIZE 128

void sigHander()
{
    int ret = 0;
    /* 资源回收 */
    exit(0);
}

int main(int argc, char const *argv[])
{
    /* 信号注册 */
    signal(SIGINT, (__sighandler_t)sigHander);
    signal(SIGQUIT, (__sighandler_t)sigHander);
    signal(SIGTSTP, (__sighandler_t)sigHander);

    /* 创建socket套接字 */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("sorket error");
        exit(-1);
    }

    /* 连接 */
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    /* 地址族 */
    serverAddress.sin_family = AF_INET;
    /* 端口需要转成大端 */
    serverAddress.sin_port = htons(SERVER_PORT);

    /* 配置本机IP */
    int ret = inet_pton(AF_INET, SERVER_IPADDRESS, &serverAddress.sin_addr.s_addr);
    if (ret != 1)
    {
        perror("inet_pthon error");
        exit(-1);
    }

    int serverAddressLen = sizeof(serverAddress);
    ret = connect(sockfd, (struct sockaddr*)&serverAddress, serverAddressLen);
    if (ret == -1)
    {
        perror("connect error");
        exit(-1);
    }


    char buffer[BUFFER_SIZE];
    memset(&buffer, 0, sizeof(buffer));

    char recvBuffer[BUFFER_SIZE];
    memset(&recvBuffer, 0, sizeof(buffer));

    while (1)
    {
        strncpy(buffer, "我是客户端2", sizeof(buffer) - 1);

        write(sockfd, buffer, sizeof(buffer) - 1);

        sleep(1);

        read(sockfd, recvBuffer, sizeof(buffer));

        printf("服务器对客户端说:%s\n", recvBuffer);

    }

    close(sockfd);

}
