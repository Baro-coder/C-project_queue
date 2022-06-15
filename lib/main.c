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
    return 0;
}

int removeTheSyncStructures()
{
    return 0;
}


int callTheChildProcesses()
{
    ((p1 = fork()) && (p2 = fork()) && (p3 = fork()));
    if(p1 < 0 || p2 < 0 || p3 < 0) {
        killTheChildProcesses();
        return 1;
    }
    else if(p1 == 0){
        char* args[] = {p1_src_file, NULL};
        execvp(p1_src_file, args);
        exit(EXIT_SUCCESS);
    }
    else if(p2 == 0){
        char* args[] = {p2_src_file, NULL};
        execvp(p2_src_file, args);
        exit(EXIT_SUCCESS);
    }
    else if(p3 == 0){
        char* args[] = {p3_src_file, NULL};
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