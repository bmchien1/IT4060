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
    addr.sin_port = htons(8000);

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

    while (1)
    {
        printf("Waitng for new client....\n");
        int client = accept(listener, NULL, NULL);
        printf("New client accepted, client = %d\n", client);

        if (fork() == 0)
        {
            close(listener);

            char buf[BUFFER_SIZE];
            while (1)
            {
                int ret = recv(client, buf, sizeof(buf), 0);
                if (ret <= 0)
                    break;

                buf[ret] = 0;
                printf("Receive: %s", buf);
                char todo[15];
                char format[20];
                int n = sscanf(buf, "%s %s", todo, format);

                if (n != 2 || strcmp(todo, "GET_TIME") != 0 ||
                    (strcmp(format, "dd/mm/yyyy") != 0 &&
                     strcmp(format, "dd/mm/yy") != 0 &&
                     strcmp(format, "mm/dd/yyyy") != 0 &&
                     strcmp(format, "mm/dd/yy") != 0))
                {
                    send(client, "Wrong format\n", sizeof("Wrong format\n"), 0);
                    continue;
                }

                time_t t = time(NULL);
                char time_str[12];
                if (strcmp(format, "dd/mm/yyyy") == 0)
                    strftime(time_str, sizeof(time_str), "%d/%m/%Y\n", localtime(&t));
                if (strcmp(format, "dd/mm/yy") == 0)
                    strftime(time_str, sizeof(time_str), "%d/%m/%y\n", localtime(&t));
                if (strcmp(format, "mm/dd/yyyy") == 0)
                    strftime(time_str, sizeof(time_str), "%m/%d/%Y\n", localtime(&t));
                if (strcmp(format, "mm/dd/yy") == 0)
                    strftime(time_str, sizeof(time_str), "%m/%d/%y\n", localtime(&t));
                send(client, time_str, sizeof(time_str), 0);
            }
            close(client);
            exit(0);
        }

        close(client);
    }
}