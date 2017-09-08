/*************************************************************************
	> File Name: testZombie.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 17:34:49 2017
 ************************************************************************/

#include<iostream>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstring>

using namespace std;

sig_atomic_t child_exit_status;

void clean_up_child_process(int signal_num)
{
    int status;
    wait(&status);

    child_exit_status = status;
}

int main()
{
    struct sigaction sigchild_action;
    memset(&sigchild_action, 0, sizeof(sigchild_action));
    sigchild_action.sa_handler = &clean_up_child_process;
    sigaction(SIGCHLD, &sigchild_action, nullptr);

    pid_t pid = fork();

    if (pid > 0)
    {
        cout << "in parent process, sleep for one miniute..." << endl;
        sleep(60);
        cout << "after sleeping, and exit" << endl;
    }
    else if (pid == 0)
    {
        cout << "in child process, and exit" << endl;
        exit(0);
    }
    else
    {
        cout << "fork failed" << endl;
    }

    return 0;
}
