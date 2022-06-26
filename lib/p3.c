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

int chr_count;

int main(int argc, char ** argv)
{
    build(argv);
    report_out("Ready.");

    char * buffer = (char *) malloc(BUFF_SIZE * sizeof(char));
    int ret;

    while(1)
    {
        if(running)
        {
            ret = receive(buffer);

            if(ret == 0)
            {
                chr_count += atoi(buffer);
                memset(buffer, 0, BUFF_SIZE);
                buffer = (char *) malloc(BUFF_SIZE * sizeof(char));
            }
            else if(ret == 1)
            {
                memset(buffer, 0, BUFF_SIZE);
                buffer = (char *) malloc(BUFF_SIZE * sizeof(char));

                fprintf(stderr, "%d\n", chr_count);
                chr_count = 0;
            }
        }
    }
    
    return 0;
}

int queueOpen(key_t key)
{
    int qID;
    if((qID = msgget(key, 0)) == -1) return -1;
    else return qID;
}

void build(char ** argv)
{
    ppid = getppid();
    pid = getpid();
    p_n = 3;
    running = 1;

    qID_2 = queueOpen((key_t) KEY_QUEUE_2);
    if(qID_2 == -1){
        report_err("Error opening the queue 2!");
    }

    int shmkey = ftok ("/dev/null", KEY_SHMEM);
    shmid = shmget (shmkey, sizeof (int), 0644 | IPC_CREAT);
    if (shmid < 0){
        report_err("Error getting shared memory segment!");
    }

    shm_code = (int *) shmat (shmid, NULL, 0);

    chr_count = 0;

    signal(SIGINT,  sigHandler);
    signal(SIGUSR1, sigHandler);
    signal(SIGTSTP, sigHandler);
    signal(SIGUSR2, sigHandler);
}

int receive(char * buffer)
{
    struct msgbuff * rcv;

    msgrcv(qID_2, rcv, sizeof(struct msgbuff), 0, 0);

    //char * report = (char *) malloc(BUFF_SIZE * sizeof(char));
    //sprintf(report, "Received: { %d : %s : %d }", rcv->type, rcv->data, chr_count);
    //report_err(report);
    //free(report);

    if(rcv->type == INT_TYPE){
        strcpy(buffer, rcv->data);
        return 0;
    }
    else if(rcv->type == NWLINE_TYPE){
        return 1;
    }
    else {
        return -1;
    }
}

void sigHandler(int signum)
{
    char * report = (char *) malloc(BUFF_SIZE * sizeof(char));

    if(signum == SIGUSR2)
    {    
        sprintf(report, "Received SIGUSR2: [%d]", signum);
        report_err(report);

        int sig_code = *shm_code;

        sprintf(report, "Read: [%d]", sig_code);
        report_err(report);

        if(sig_code == SIGINT)
        {
            shmdt (shm_code);

            sprintf(report, "End of work.");
            report_err(report);
            free(report);
            
            kill(pid, SIGTERM);
        }
        else if(sig_code == SIGUSR1)
        {
            running = 1;
        }
        else if(sig_code == SIGTSTP)
        {
            running = 0;
        }
    }
    else
    {
        sprintf(report, "Received signal: [%d]", signum);
        report_err(report);

        kill(ppid, signum);
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