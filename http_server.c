#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define MAX_FDS 2048

void signalHanlder(int signo)
{
    pid_t pid = wait(NULL);
}

int main()
{
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    // Declare server address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8080);

    // Bind socket to address structure
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind() failed");
        return 1;
    }

    // Change socket state to listening
    if (listen(listener, 5))
    {
        perror("listen() failed");
        return 1;
    }

    signal(SIGCHLD, signalHanlder);
    char buf[BUFFER_SIZE];
    while (1)
    {
        printf("Waitng for new client....\n");
        int client = accept(listener, NULL, NULL);
        if (client == -1)
            continue;
        printf("New client accepted, client = %d\n", client);
        if (fork() == 0)
        {
            close(listener);
            int ret = recv(client, buf, sizeof(buf), 0);
            if (ret <= 0)
            {
                close(client);
                continue;
            }

            if (ret < sizeof(buf))
                buf[ret] = 0;
            printf("%s\n", buf);
            close(client);
            exit(0);
        }
        else
        {
            char msg[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Xin chao cac ban!</h1></body></html>\n";
            send(client, msg, strlen(msg), 0);
        }
        close(client);
    }
}
