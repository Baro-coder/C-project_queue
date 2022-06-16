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
#include <string.h>
#include <fcntl.h>

#include "consts.h"
#include "child.h"

int main(int argc, char ** argv)
{
    build(argv);
    report_out("Ready.");

    char * buffer = (char *) malloc(BUFF_SIZE * sizeof(char));

    while(1)
    {
        if(receive(buffer) == 0)
        {
            printf("P2: %s\n", buffer);
            memset(buffer, 0, BUFF_SIZE);
            buffer = (char *) malloc(BUFF_SIZE * sizeof(char));
        }
    }

    return 0;
}


void build(char ** argv)
{
    ppid = getppid();
    pid = getpid();
    p_n = 2;

    qID_1 = queueOpen((key_t) KEY_QUEUE_1);
    if(qID_1 == -1){
        report_err("Error opening the queue 1!");
    }

    qID_2 = queueOpen((key_t) KEY_QUEUE_2);
    if(qID_2 == -1){
        report_err("Error opening the queue 2!");
    }

    signal(SIGINT,  sigHandler);
    signal(SIGUSR1, sigHandler);
    signal(SIGUSR2, sigHandler);
}

int queueOpen(key_t key)
{
    int qID;
    if((qID = msgget(key, 0)) == -1) return -1;
    else return qID;
}

int receive(char * buffer)
{
    struct msgbuff * rcv;

    msgrcv(qID_1, rcv, sizeof(struct msgbuff), 0, 0);

    if(rcv->type == STRING_TYPE){
        strcpy(buffer, rcv->data);
        return 0;
    }
    else {
        return 1;
    }
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
    fprintf(stdout, "P%d[%d]: %s\n", p_n, pid, message);
}

void report_err(char * message)
{
    fprintf(stderr, "P%d[%d]: %s\n", p_n, pid, message);
}