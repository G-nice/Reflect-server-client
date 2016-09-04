#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define BUFFSIZE 8192
typedef void (*sighandler_t) (int);

int main(int argc, char* argv[])
{
    int socket_fd, rec_len = 0;
    char buff[BUFFSIZE];

    struct sockaddr_in servaddr;
    sighandler_t sh;

    // 错误参数  使用方法提示
    if (argc != 3)
    {
        fprintf(stderr, "Usage: reflect_client <IP> <PORT>\n");
        exit(1);
    }

    // 初始化socket_fd
    if ( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    {
        fprintf(stderr, "create socket error: %s(errno: %d)\n",strerror(errno), errno);
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    // servaddr.sin_addr.s_addr = htonl(inet_addr(argv[1]));  //wrong
    if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) == -1)    //设置服务器IP地址
    {
        printf("inet_pton error for %s\n",argv[1]);
        exit(0);
    }
    servaddr.sin_port = htons((uint32_t)atol(argv[2]));   //设置端口

    if ( connect(socket_fd, (struct sockaddr*) &servaddr, sizeof(servaddr)) == -1)
    {
        perror("connect to server error");
        close(socket_fd);
        exit(1);
    }

    while(fgets(buff, BUFFSIZE, stdin))
    {
        if (strcmp("//exit\n", buff) == 0)
        {
            close(socket_fd);
            exit(0);
        }
        sh = signal(SIGPIPE, SIG_IGN);    //忽略SIGPIPE
        if (send(socket_fd, buff, strlen(buff)+1, 0) == -1 )
        {
            perror("send message error");
            signal(SIGPIPE, sh);  //恢复SIGPIPE
            exit(1);
        }
        signal(SIGPIPE, sh);  //恢复SIGPIPE
        memset(buff, 0, BUFFSIZE);
        if ((rec_len = recv(socket_fd, buff, BUFFSIZE, 0)) == -1)
        {
            perror("recv message error");
            exit(1);
        }
        buff[rec_len] = '\0';    //多余
        printf("receive:\t%s\n", buff);
    }

    close(socket_fd);
    exit(0);
}
