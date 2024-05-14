#include <chrono>
#include <mutex>
#include <string>
#include <thread>
//-
#include <bits/stdc++.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
//-
#include "cqueue.hpp"


#define ADDRESS "127.0.0.1"
#define PORT 514
#define MAXLINE 1024

int is_running = 1;
pid_t pid = 0;
gto::cqueue<std::string> queue;
std::mutex queue_lock;

int syslog_server_init(int port)
{
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
    {
        perror("Syslog server creation failed");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    // Filling server information
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ADDRESS, &(server_addr.sin_addr));

    if (bind(sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        close(sockfd);
        sockfd = 0;
        perror("Syslog server bind failed");
    }

    return sockfd;
}

void syslog_server_run(int sockfd)
{
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    struct pollfd pfd = {sockfd, POLLIN, {}};

    char buffer[MAXLINE];
    int n;
    socklen_t len;

    while (is_running)
    {
        if (poll(&pfd, 1, 1000) == 1)
        {
            n = recvfrom(sockfd, (char*)buffer, MAXLINE, MSG_WAITALL,
                    (struct sockaddr *)&client_addr, &len);
            buffer[n] = 0;
            queue_lock.lock();
            queue.push(buffer);
            queue_lock.unlock();
        }
    }
}

void pipe_syslog()
{
    std::string tmp;
    while (is_running)
    {
        while (!queue.empty())
        {
            queue_lock.lock();
            tmp = queue.pop();
            queue_lock.unlock();

            std::cout << tmp << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

pid_t subprocess_init(const char* argv[])
{
    pid_t pid;
    if ((pid = fork()) >= 0)
    {
        if (pid == 0)
        {
            return execvp(argv[0], (char* const*)argv);
        }
    }
    else
    {
        perror("Subprocess creation failed");
    }
    return pid;
}

void signal_handler(int s)
{
    if (pid)
    {
        kill(pid, s);
    }
}

int main(int argc, const char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Example: supervisor-echo-init program arg1 arg2" << std::endl;
    }

    int sockfd = syslog_server_init(PORT);
    if (sockfd <= 0)
    {
        return EXIT_FAILURE;
    }
    std::thread syslog_thread(syslog_server_run, sockfd);
    std::thread pipe_thread(pipe_syslog);

    struct sigaction sig_h;
    sig_h.sa_handler = signal_handler;
    sigemptyset(&sig_h.sa_mask);
    sig_h.sa_flags = 0;
    sigaction(SIGINT, &sig_h, NULL);
    sigaction(SIGTERM, &sig_h, NULL);
    sigaction(SIGHUP, &sig_h, NULL);

    int ret;
    int wstatus;

    pid = subprocess_init(++argv);
    if (pid > 0)
    {
        waitpid(pid, &wstatus, 0);
        ret = WEXITSTATUS(wstatus);
    }
    else
    {
        ret = EXIT_FAILURE;
    }

    is_running = false;
    syslog_thread.join();
    pipe_thread.join();
    close(sockfd);
    return ret;
}
