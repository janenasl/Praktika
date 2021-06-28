#include <stdio.h>

#define config_file "/tmp/log/IBM.log"


FILE *fp = NULL;

int init_logger()
{
    fp = fopen(config_file, "a");
    if (fp == NULL)
        return 1;
    return 0;
}

void cleanup()
{
    fclose(fp);
    //Do other stuff if you need to
}

void write_to_log(char *level, char *message)
{
    fprintf(fp, "%s: %s\n", level, message);
    fflush(fp);
}
