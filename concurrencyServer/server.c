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
#include "threadPool.h"


#define SERVER_PORT 8080
#define MAX_LISTEN  128
#define LOCAL_IPADDRESS "192.168.241.132"
#define BUFFER_SIZE 128
#define MINTHREADS 5
#define MAXTHREADS 10
#define MAXQUEUESIZE 50

void sigHander()
{
    /* 资源回收 */
    exit(0);
}

/* 线程处理函数 */
void * threadHandle(void *arg)
{
    /* 设置线程分离 */
    pthread_detach(pthread_self());
    /* 通信句柄 */
    int acceptfd = *(int *)arg;

    /* 通信 */  

    /* 接收缓冲区 */   
    char recvbuffer[BUFFER_SIZE];
    memset(&recvbuffer, 0, sizeof(recvbuffer));

    /* 发送缓冲区 */ 
    char sendBuffer[BUFFER_SIZE];
    memset(&sendBuffer, 0, sizeof(sendBuffer));

    /* 读取到的字节数 */
    int readBytes = 0;
    while (1)
    {
        readBytes = read(acceptfd, recvbuffer, sizeof(recvbuffer) - 1);
        if (readBytes <= 0)
        {
            printf("有个客户端下线了\n");
            close(acceptfd);
            break;
        }
        else
        {
            /* 读到的字符串 */
            printf("客户端对服务器说:%s\n",recvbuffer);
            if (strncmp(recvbuffer, "我是客户端1", strlen("我是客户端1")) == 0)
            {
                strncpy(sendBuffer, "你是客户端1", sizeof(sendBuffer) - 1);
                sleep(1);
                write(acceptfd, sendBuffer, sizeof(sendBuffer));
            }
            else if (strncmp(recvbuffer, "我是客户端2", strlen("我是客户端2")) == 0)
            {
                strncpy(sendBuffer, "你是客户端2", sizeof(sendBuffer) - 1);
                sleep(1);
                write(acceptfd, sendBuffer, sizeof(sendBuffer));
            }
        }
    }

    pthread_exit(NULL);
}


int main(int argc, char const *argv[])
{
    /* 初始化线程池 */
    // threadPool_t pool;
    // threadPoolInit(&pool, MINTHREADS, MAXTHREADS, MAXQUEUESIZE);


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

    /* 循环去接收客户端的请求 */
    while (1)
    {
        socklen_t clientAddressLen = 0;

        int acceptfd = accept(sockfd, (struct sockaddr *)&clientAddress, &clientAddressLen);
        if (acceptfd == -1)
        {
            perror("accept error");
            exit(-1);
        }

#if 1
        /* 这种情况每来一个客户端就开辟线程资源， */
        /* 开一个线程去服务acceptfd */
        pthread_t tid;
        ret = pthread_create(&tid, NULL, threadHandle, (void *)&acceptfd);
        if (ret != 0 )
        {
            perror("thread create error");
            exit(-1);
        }
#else
        /* 将任务添加到任务队列 */
        threadPoolAddTask(&pool, threadHandle, (void *)&acceptfd);
#endif
    }

    /* 释放线程池 */
    // threadPoolDestory(&pool);

    close(sockfd);

    return 0;
}
