#ifndef _CONSTS_H

#define _CONSTS_H

#define BUFF_SIZE 128   // Buffer size

#define KEY_QUEUE_1 1010
#define KEY_QUEUE_2 1020

#define STRING_TYPE 1
#define INT_TYPE 2

// Message structure
struct msgbuff
{
    int type;
    char data[BUFF_SIZE];
};

int qID_1;  // queue ID: P1 > P2
int qID_2;  // queue ID: P2 > P3

void sigHandler(int signum);    // signal interpreter

void report_out(char * message);    // print the message on stdout
void report_err(char * message);    // print the message on stderr

#endif