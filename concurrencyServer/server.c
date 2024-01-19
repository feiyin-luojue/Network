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
#define LOCAL_IPADDRESS "192.168.241.132"
#define BUFFER_SIZE 128

void sigHander()
{
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

    /* 设置端口复用 */
    int enableOpt = 1;
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&enableOpt, sizeof(enableOpt));
    if (ret == -1)
    {
        perror("setsockopt error");
        exit(-1);
    }

    /* 绑定 */
#if 0
    struct sockaddr localAddress;
#else
    struct sockaddr_in localAddress;
#endif
    memset(&localAddress, 0, sizeof(localAddress));

    /* 地址族 */
    localAddress.sin_family = AF_INET;
    /* 端口需要转成大端 */
    localAddress.sin_port = htons(SERVER_PORT);
    /* ip地址需要转成大端 */
    /* Address to accept any incoming messages */
    /* INADDR_ANY = 0x00000000 */
#if 1
    localAddress.sin_addr.s_addr = htonl(INADDR_ANY);
#else
    /* 配置本机IP */
    inet_pton(AF_INET, LOCAL_IPADDRESS, &localAddress.sin_addr.s_addr);
#endif
    int localAddressLen = sizeof(localAddress);
    ret = bind(sockfd, (struct sockaddr*)&localAddress, localAddressLen);
    if (ret == -1)
    {
        perror("bind error");
        exit(-1);
    }

    /* 监听 */
    ret = listen(sockfd, MAX_LISTEN);
    if (ret == -1)
    {
        perror("listen error");
        exit(-1);
    }

    /* 客户的信息 */
    struct sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(clientAddress));

    socklen_t clientAddressLen = 0;

    int acceptfd = accept(sockfd, (struct sockaddr *)&clientAddress, &clientAddressLen);
    if (acceptfd == -1)
    {
        perror("accept error");
        exit(-1);
    }

    char buffer[BUFFER_SIZE];
    memset(&buffer, 0, sizeof(buffer));

    char replayBuffer[BUFFER_SIZE];
    memset(&replayBuffer, 0, sizeof(replayBuffer));

    int readBytes = 0;
    while (1)
    {
        readBytes = read(acceptfd, buffer, sizeof(buffer) - 1);
        if (readBytes <= 0)
        {
            perror("read error");
            printf("读了个寂寞，直接G\n");
            close(acceptfd);
            break;
        }
        else if (readBytes == 0)
        {
            /* 资源问题 */
            printf("小黑子下线了，我直接溜溜\n");
            break;
        }
        else
        {
            /* 读到的字符串 */
            printf("客户端对服务器说:%s\n",buffer);
            sleep(3);
            
            strncpy(replayBuffer, "？？？小黑子？", sizeof(buffer) - 1);

            write(acceptfd, replayBuffer, sizeof(buffer) - 1);
        }
    }

    close(acceptfd);
    close(sockfd);

    return 0;
}
