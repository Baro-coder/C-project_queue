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

void menu();
void manual();
void file();

int main(int argc, char ** argv)
{
    build(argv);
    report_out("Ready.");

    while(1)
    {
        menu();
    }

    return 0;
}


void build(char ** argv)
{
    ppid = getppid();
    pid = getpid();
    p_n = 1;

    qID_1 = queueOpen((key_t) KEY_QUEUE_1);

    if(qID_1 == -1){
        report_err("Error opening the queue 1!");
    }

    signal(SIGINT,  sigHandler);
    signal(SIGUSR1, sigHandler);
    signal(SIGUSR2, sigHandler);
}


void menu()
{
    char c = '\0';
    
    while(1)
    {
        usleep(500);

        printf("----------------------------\n");
        printf(" # MENU # \n");
        printf(" [1]. Manual\n");
        printf(" [2]. File\n");
        printf(" Select option: \n");
        printf(" > ");
        scanf("%c", &c);

        if(c == '1')
        {
            manual();
        }
        else if(c == '2')
        {
            file();
        }
        else
        {
            printf("\n Select the valid option!\n");
        }

        c = '\0';
    }
}

void manual()
{
    char * buffer;

    while(1)
    {
        usleep(400);

        buffer = (char *) malloc(BUFF_SIZE * sizeof(char));

        printf("\nType the message ('exit' to back to the menu):\n");
        printf(" > ");
        fgets(buffer, sizeof(buffer), stdin);

        if(strncmp(buffer, "exit", 4) == 0){
            free(buffer);
            break;
        } else {
            transfer(buffer, STRING_TYPE);
        }

        memset(buffer, 0, BUFF_SIZE);
    }
}

void file()
{
    char * filepath = (char *) malloc(BUFF_SIZE * sizeof(char));

    printf("\nType the filepath:\n");
    printf(" > ");
    scanf("%s", filepath);

    FILE * fp = fopen(filepath, "r");
    if(fp == NULL){
        printf("\n %s: Cannot open the file!\n", filepath);
    } else {
        char * buffer;

        while(1)
        {
            buffer = (char *) malloc(BUFF_SIZE * sizeof(char));

            if(fgets(buffer, sizeof(buffer), fp) == NULL) break;

            transfer(buffer, STRING_TYPE);

            memset(buffer, 0, BUFF_SIZE);
        }

        fclose(fp);
        free(filepath);
        free(buffer);
    }
}


int queueOpen(key_t key)
{
    int qID;
    if((qID = msgget(key, 0)) == -1) return -1;
    else return qID;
}

void transfer(char * buffer, int msg_type)
{
    struct msgbuff msg;

    msg.type = msg_type;

    sprintf(msg.data, "%s", buffer);

    //char * report = (char *) malloc(BUFF_SIZE * sizeof(char));
    //sprintf(report, "Send: {%d : %s}", msg.type, msg.data);
    //report_out(report);

    msgsnd(qID_1, &msg, sizeof(struct msgbuff), 0);
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