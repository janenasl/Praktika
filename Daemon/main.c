#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

static void daemon_process();

int main(int argc, char* argv[])
{
    daemon_process();
    FILE *fp = NULL;
    fp = fopen("Log.txt", "w+");
    if (fp == NULL) {
        printf("Could not open file");
        return 0;
    }

    while(1){
        sleep(1);
        fprintf(fp, "veikiu\n");
        fflush(fp);
    }
    fclose(fp);

}

static void daemon_process()
{
    pid_t process_id = 0;
    pid_t sid = 0;
    int x = 0;

    process_id = fork();

    if (process_id < 0) {
        printf("fork failed\n");
        exit(1);
    }
    if (process_id > 0){
        printf("process_id of child process %d \n", process_id);
        exit(0);
    }

    sid = setsid();

    if (sid < 0){
        printf("sid failed");
        exit(1);
    }

    umask(0);
    chdir("/");
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--) {
        close (x);
    }

}