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
    char * output = (char *) malloc(BUFF_SIZE * sizeof(char));

    while(1)
    {
        if(running)
        {
            if(receive(buffer) == 0)
            {
                sprintf(output, "%d", strlen(buffer));

                transfer(output, INT_TYPE);

                if(strstr(buffer, "\n")){
                    transfer("0", NWLINE_TYPE);
                }

                memset(buffer, 0, BUFF_SIZE);
                buffer = (char *) malloc(BUFF_SIZE * sizeof(char));
                memset(output, 0, BUFF_SIZE);
                output = (char *) malloc(BUFF_SIZE * sizeof(char));
            }
        }
    }

    return 0;
}

void build(char ** argv)
{
    ppid = getppid();
    pid = getpid();
    p_n = 2;
    p_next_id = atoi(argv[3]);
    running = 1;

    qID_1 = queueOpen((key_t) KEY_QUEUE_1);
    if(qID_1 == -1){
        report_err("Error opening the queue 1!");
    }

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

    signal(SIGINT,  SIG_IGN);
    signal(SIGUSR1, SIG_IGN);
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

void transfer(char * output, int msg_type)
{
    struct msgbuff msg;

    msg.type = msg_type;
    strcpy(msg.data, output);

    //char * report = (char *) malloc(BUFF_SIZE * sizeof(char));
    //sprintf(report, "Send: {%d : %s}", msg.type, msg.data);
    //report_out(report);

    msgsnd(qID_2, &msg, sizeof(struct msgbuff), 0);
}

void sigHandler(int signum)
{
    if(signum == SIGUSR2)
    {
        char * report = (char *) malloc(BUFF_SIZE * sizeof(char));
        sprintf(report, "Received SIGUSR2: [%d]", signum);
        report_err(report);

        int sig_code = *shm_code;

        sprintf(report, "Read: [%d]", sig_code);
        report_err(report);

        kill(p_next_id, SIGUSR2);

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
            if(running == 0) running = 1;
            else running = 0;
        }

        free(report);
    }
}

void report_out(char * message)
{
    fprintf(stdout, "P%d[%d]: %s\n", p_n, pid, message);
}

void report_err(char * message)
{
    fprintf(stderr, "P%d[%d]: %s\n", p_n, pid, message);
}