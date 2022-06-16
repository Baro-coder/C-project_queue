#ifndef _CHILD_H

#define _CHILD_H

int ppid;   // parent process ID
int pid;    // process ID

int p_n;    // process number

void build(char ** argv);       // prepare the control data

int queueOpen(key_t key);       // get access to the queue

int receive(char * buffer);    // receive data
void transfer(char * buffer, int msg_type);   // transfer data to next process

#endif