#ifndef _CONSTS_H

#define _CONSTS_H

#define BUFF_SIZE 128   // Buffer size

// Message structure
struct msgbuff
{
    int type;
    char data[BUFF_SIZE];
};

void sigHandler(int signum);    // signal interpreter

void report_out(char * message);    // print the message on stdout
void report_err(char * message);    // print the message on stderr

#endif