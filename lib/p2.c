#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#include "consts.h"
#include "child.h"

int main(int argc, char ** argv)
{
    build(argv);
    report_out("Ready.");
    
    return 0;
}


void build(char ** argv)
{
    ppid = getppid();
    pid = getpid();
    p_n = 2;

    signal(SIGINT,  sigHandler);
    signal(SIGUSR1, sigHandler);
    signal(SIGUSR2, sigHandler);
}


void sigHandler(int signum)
{
    char * report = (char *) malloc(64 * sizeof(char));

    if(signum == SIGINT)
    {
        sprintf(report, "Received SIGINT: [%d]", signum);
        report_err(report);
    }
    else if(signum == SIGUSR1)
    {
        sprintf(report, "Received SIGUSR1: [%d]", signum);
        report_err(report);
    }
    else if(signum == SIGUSR2)
    {
        sprintf(report, "Received SIGUSR2: [%d]", signum);
        report_err(report);
    }
    else
    {
        sprintf(report, "Received unknown signal: [%d]", signum);
        report_err(report);
    }

    free(report);
}

void report_out(char * message)
{
    fprintf(stdout, "P2[%d]: %s\n", pid, message);
}

void report_err(char * message)
{
    fprintf(stderr, "P2[%d]: %s\n", pid, message);
}