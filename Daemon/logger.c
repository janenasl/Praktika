#include <stdio.h>

#define LOGFILE "/var/log/DaemonLog.log"
FILE *fp = NULL;

int init_logger()
{
    fp = fopen(LOGFILE, "a");
    if (fp == NULL){
            printf("Can't open log file, program will be terminated! \n");
            return 1;
    }
    return 0;
}

void cleanup()
{
    fclose(fp);
    //Do other stuff if you need to
}

void write_to_log(char *level, char *message)
{
    fprintf(fp, "[%s %s] %s %s\n", __DATE__, __TIME__, level, message);
    fflush(fp);
}