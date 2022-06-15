#ifndef _CHILD_H

#define _CHILD_H

int ppid;   // parent process ID
int pid;    // process ID

int p_n;    // process number

int qID_1;  // queue ID: P1 > P2
int qID_2;  // queue ID: P2 > P3

void build(char ** argv);       // prepare the control data

int receive(char * buffer);     // receive data
int transfer(char * buffer);    // transfer data to next process

#endif