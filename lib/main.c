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
#include "parent.h"

int main()
{
    report_out("Building the synchronization structures...");
    if(buildTheSyncStructures() != 0){
        report_err("Error building the synchronization structures!");
        exit(EXIT_FAILURE);
    } else {
        report_out("The synchronization structures build successfully.");
    }

    report_out("Calling the child processes...");
    if(callTheChildProcesses() == 1){
        report_err("Error while child processes fork!");
    }

    report_out("Removing the synchronization structures...");
    if(removeTheSyncStructures() != 0){
        report_err("Error removing the synchronization structures!");
        exit(EXIT_FAILURE);
    } else {
        report_out("The synchronization structures removed successfully.");
    }

    report_out("Program stoped with success!");
    exit(EXIT_SUCCESS);
}

int buildTheSyncStructures()
{
    if ((qID_1 = queueOpen((key_t) KEY_QUEUE_1)) == -1)
    {
        report_err("Error opening queue 1!");
        return 1;
    }

    if ((qID_2 = queueOpen((key_t) KEY_QUEUE_2)) == -1)
    {
        report_err("Error opening queue 1!");
        return 1;
    }

    int shmkey = ftok ("/dev/null", KEY_SHMEM);
    shmid = shmget (shmkey, sizeof (int), 0644 | IPC_CREAT);
    if (shmid < 0){
        report_err("Error getting shared memory segment!");
        return 1;
    }

    shm_code = (int *) shmat (shmid, NULL, 0);
    *shm_code = 0;

    return 0;
}

int removeTheSyncStructures()
{
    if (queueRemove(qID_1) == -1)
    {
        report_err("Error removing queue 1!");
        return 1;
    }

    if (queueRemove(qID_2) == -1)
    {
        report_err("Error removing queue 2!");
        return 1;
    }

    shmdt (shm_code);
    shmctl (shmid, IPC_RMID, 0);

    return 0;
}


int queueOpen(key_t key)
{
    int qID;
    if((qID = msgget(key, IPC_CREAT | 0660)) == -1) return -1;
    else return qID;
}

int queueRemove(int qID)
{
    if(msgctl(qID, IPC_RMID, 0) == -1) return -1;
    else return qID;
}


int callTheChildProcesses()
{
    char * qID_1_str = (char *) malloc(32 * sizeof(char));
    char * qID_2_str = (char *) malloc(32 * sizeof(char));
    char * p2_str = (char *) malloc(16 * sizeof(char));
    char * p3_str = (char *) malloc(16 * sizeof(char));

    sprintf(qID_1_str, "%d", qID_1);
    sprintf(qID_2_str, "%d", qID_2);
    sprintf(p2_str, "%d", getpid() + 2);
    sprintf(p3_str, "%d", getpid() + 3);

    ((p1 = fork()) && (p2 = fork()) && (p3 = fork()));
    if(p1 < 0 || p2 < 0 || p3 < 0) {
        killTheChildProcesses();
        return 1;
    }
    else if(p1 == 0){
        char* args[] = {p1_src_file, qID_1_str, qID_2_str, p2_str, NULL};
        execvp(p1_src_file, args);
        exit(EXIT_SUCCESS);
    }
    else if(p2 == 0){
        char* args[] = {p2_src_file, qID_1_str, qID_2_str, p3_str, NULL};
        execvp(p2_src_file, args);
        exit(EXIT_SUCCESS);
    }
    else if(p3 == 0){
        char* args[] = {p3_src_file, qID_1_str, qID_2_str, NULL};
        execvp(p3_src_file, args);
        exit(EXIT_SUCCESS);
    }
    else{
        signal(SIGINT,  sigHandler);
        signal(SIGUSR1, sigHandler);
        signal(SIGUSR2, sigHandler);

        wait(NULL);
        wait(NULL);
        wait(NULL);
        report_out("The child processes are done.");
    }
}

void killTheChildProcesses()
{
    report_out("Terminating the child processes...");
    if(kill(p1, SIGTERM) == 0){
        report_out("Process 1 Terminated.");
    }
    if(kill(p2, SIGTERM) == 0){
        report_out("Process 2 Terminated.");
    }
    if(kill(p3, SIGTERM) == 0){
        report_out("Process 3 Terminated.");
    }
}


void sigHandler(int signum)
{
    char * report = (char *) malloc(64 * sizeof(char));

    *shm_code = signum;

    kill(p1, SIGUSR2);

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
    fprintf(stdout, "MAIN[%d]: %s\n", getpid(), message);
}

void report_err(char * message)
{
    fprintf(stderr, "MAIN[%d]: %s\n", getpid(), message);
}