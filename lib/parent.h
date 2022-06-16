#ifndef _PARENT_H

#define _PARENT_H

char * p1_src_file = "./var/p1.out";
char * p2_src_file = "./var/p2.out";
char * p3_src_file = "./var/p3.out";

pid_t p1;   // Process 1 ID
pid_t p2;   // Process 2 ID
pid_t p3;   // Process 3 ID

int queueOpen(key_t key);    // Create the queue
int queueRemove();  // Remove the queue

int buildTheSyncStructures();   // create the synchronization structures
int removeTheSyncStructures();  // remove the synchronization structures

int callTheChildProcesses();    // create and run the child processes
void killTheChildProcesses();   // terminate the child processes


#endif