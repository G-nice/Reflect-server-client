#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>


#define BUFFSIZE 8192  //4096

typedef void (*sighandler_t) (int);

// void str_echo(int socket_fd);

// 子进程终止信号处理  防止僵死进程
void sig_chld(int signo)
{
    pid_t pid;
    int status;
    // char buff[32];

    while((pid = waitpid(-1, &status, WNOHANG)) > 0);
    // {

    //     write(STDOUT_FILENO, buff, strlen(buff)+1);
    // }
    return;
}

int main(int argc, char* argv[])
{

    int socket_fd, connect_fd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);
    char buff[BUFFSIZE];  //s输出缓冲区
    char client_addr[32];
    int n = 0;  //计数
    // uint32_t servip;  //参数的IP转化为二进制
    time_t ticks;
    pid_t clild_pid = 0;
    struct sigaction act;
    sighandler_t sh;

// 错误参数  使用方法提示
    if (argc != 3)
    {
        fprintf(stderr, "Usage: reflect_server <IP> <PORT>\n");
        exit(1);
    }

    // 初始化socket_fd
    if ( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    {
        fprintf(stderr, "create socket error: %s(errno: %d)\n",strerror(errno), errno);
        exit(1);
    }

    // 初始化
/*    if(inet_pton(AF_INET, argv[1], &servip) == 0)  //将输入的IP转化为二进制地址格式
    {
        fprintf(stderr, "server IP invalid: %s(errno: %d)\n",strerror(errno), errno);
        close(socket_fd);
        exit(1);
    }

// 或
*/

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    //servaddr.sin_addr.s_addr = htonl(servip);    //设置服务器IP地址
    if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr.s_addr) == -1)
    {
        printf("inet_pton error for %s\n",argv[1]);
        exit(0);
    }
    // servaddr.sin_addr.s_addr = htonl(inet_addr(argv[1]));    //  wrong  wrong
    servaddr.sin_port = htons((uint32_t)atol(argv[2]));   //设置端口

    // 设置套接字  能够重用blind中的地址
    int reuse = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)))
    {
        fprintf(stderr, "socket option set error: %s(errno: %d)\n",strerror(errno), errno);
        close(socket_fd);
        exit(1);
    }


    // 将运行参数提供的地址绑定到创建的套接字上
    if( bind(socket_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        fprintf(stderr, "socket bind address error: %s(errno: %d)\n",strerror(errno), errno);
        close(socket_fd);
        exit(1);
    }

    // server status
    printf("start server on %s : port %d \n", inet_ntop(AF_INET, (void*) &servaddr.sin_addr, buff, INET_ADDRSTRLEN), ntohs(servaddr.sin_port) );

    // 监听客户端连接
    if ( listen(socket_fd, 128) == -1)  //最大入队未完成连接数
    {
        fprintf(stderr, "listen error: %s(errno: %d)\n",strerror(errno), errno);
        close(socket_fd);
        exit(1);
    }

    // 进行子进程结束信号处理
    act.sa_handler = sig_chld;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_flags |= SA_RESTART;
    if (sigaction(SIGCHLD, &act, NULL) == -1)
        perror("set signal handler");

    // 等待客户连接
    printf("start wait accept\n");

    while(1)  // true
    {
        // 接受客户端连接
        //printf("new accept\n");
        if((connect_fd = accept(socket_fd, (struct sockaddr *) &cliaddr, &len) ) == -1)
        {
            if (errno == EINTR)    // 被信号处理中断    重新继续
                continue;
            else
            {
                fprintf(stderr, "server accept error: %s(errno: %d)\n",strerror(errno), errno);
                continue;
            }
        }
        ticks = time(NULL);  //接收到连接的时间

        // 输出连接到服务器的时间以及客户端的IP地址以及端口号
        inet_ntop(AF_INET, (void*) &cliaddr.sin_addr, buff, INET_ADDRSTRLEN);
        printf("at %.24s\t build connection from %s, port %d \n", ctime(&ticks), buff, ntohs(cliaddr.sin_port) );
        // snprintf(client_addr, sizeof(client_addr), "%.24s\r\n", ctime(&ticks));
        snprintf(client_addr, sizeof(client_addr), "%s : %d", buff, ntohs(cliaddr.sin_port));
        // printf("%s\n", client_addr);

        if ( (clild_pid =  fork()) == -1)
        {
            fprintf(stderr, "fork error: %s(errno: %d)\n",strerror(errno), errno);
            close(connect_fd);
            exit(1);
        }
        else if (clild_pid == 0)  //clild poccess
        {
            close(socket_fd);
            while(1)  // true
            {
                if ((n = recv(connect_fd, buff, BUFFSIZE, 0)) == -1)    //-1时候出错  0时对方结束
                {
                    fprintf(stderr, "recv massage error: %s(errno: %d)\n",strerror(errno), errno);
                     close(connect_fd);
                     exit(1);
                }
                else if (n == 0)
                {
                    ticks = time(NULL);
                    inet_ntop(AF_INET, (void*) &cliaddr.sin_addr, buff, INET_ADDRSTRLEN);
                    printf("at %.24s\t exit connection with %s, port %d \n", ctime(&ticks), buff, ntohs(cliaddr.sin_port) );
                    close(connect_fd);
                    break;
                }
                // printf("recv: %d\n", n);

                printf("%s send %s", client_addr, buff);

                sh = signal(SIGPIPE, SIG_IGN);    //忽略SIGPIPE
                if (send(connect_fd, buff, n, 0) == -1 )
                {
                    fprintf(stderr, "send error: %s(errno: %d)\n",strerror(errno), errno);
                    close(connect_fd);
                    signal(SIGPIPE, sh);  //恢复SIGPIPE
                    exit(1);
                }
                signal(SIGPIPE, sh);  //恢复SIGPIPE
                // test
                // printf("%s", buff);
                // for(n--;n>=0;n--){printf("..%d-", buff[n]);}
                // fflush(stdout);
                // printf("\n%d\n", strlen(buff));
            }
            exit(0);

        }
        else  //parent poccess
        {
            close(connect_fd);
        }

    }

    exit(0);
}
