#ifndef _CONSTS_H

#define _CONSTS_H

#define BUFF_SIZE 128   // Buffer size

#define KEY_QUEUE_1 1010    // key to create queue P1 -> P2
#define KEY_QUEUE_2 1020    // key to create queue P2 -> P3

#define KEY_SHMEM 2022      // key to create shared memory segment

#define STRING_TYPE 1       // type of message with raw string
#define INT_TYPE 2          // type of message with chars count
#define NWLINE_TYPE 10      // type of control message

// Message structure
struct msgbuff
{
    int type;               // message type
    char data[BUFF_SIZE];   // message data
};

// Shared memory segment structure
struct shmseg
{
    int code;   // signal code number
};

int qID_1;  // queue ID: P1 > P2
int qID_2;  // queue ID: P2 > P3
int shmid;  // shared memory ID
int *shm_code;  // shared memory ptr

void sigHandler(int signum);    // signal interpreter

void report_out(char * message);    // print the message on stdout
void report_err(char * message);    // print the message on stderr

#endif