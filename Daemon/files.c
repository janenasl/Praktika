#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <errno.h>

FILE *fp;

#include "LinkedList.h"

int save_not_movable_files(char *path)
{
    DIR *dir = NULL;
    struct dirent *direntp = NULL;

    char *npath;
    char *temp_path;

    if (!path) {
            fprintf(fp, "[%s %s] given path is null", __DATE__, __TIME__);
            exit(1);
    }

    dir = opendir(path);

    if (dir == NULL ) { 
            fprintf(fp, "[%s %s] directory %s does not exist", __DATE__, __TIME__, path);
            fflush(fp);
            exit(1);
    }

    while( (direntp = readdir(dir))) {
            if (strcmp(direntp->d_name,".") == 0 || strcmp(direntp->d_name,"..") == 0) {
                    continue;
            }

            switch (direntp->d_type) {
                    case DT_REG:
                            temp_path = malloc(strlen(path)+strlen(direntp->d_name)+2);
                            sprintf(temp_path, "%s/%s", path, direntp->d_name);
                            push(temp_path);
                            break;
                    case DT_DIR:            
                            npath=(char *) malloc(sizeof(char) * (strlen(path)+strlen(direntp->d_name)+2));
                            sprintf(npath,"%s/%s",path, direntp->d_name);
                            save_not_movable_files(npath);
                            free(npath);
                            break;
            }
    }
    closedir(dir);
    return 0;
}

int check_new_files(char *path, char **oldfiles, int file_count)
{
    DIR *dir = NULL;
    struct dirent *direntp = NULL;

    char *npath;
    char *temp_path;
    int checking = 0;

    if (!path) {
            fprintf(fp, "[%s %s] given path is null", __DATE__, __TIME__);
            exit(1);
    }

    dir = opendir(path);

    if (dir == NULL ) { 
            fprintf(fp, "[%s %s] directory %s does not exist", __DATE__, __TIME__, path);
            fflush(fp);
            exit(1);
    }

    while( (direntp = readdir(dir))) {
        if (strcmp(direntp->d_name,".") == 0 || strcmp(direntp->d_name,"..") == 0) {
                continue;
        }

        switch (direntp->d_type) {
                case DT_REG:
                        checking = 0;
                        temp_path = malloc(strlen(path)+strlen(direntp->d_name)+2);
                        sprintf(temp_path, "%s/%s", path, direntp->d_name);
                        for(int i = 0; i<file_count; i++) {
                                if (strcmp(oldfiles[i],temp_path) == 0) {
                                        checking = 1;
                                }
                        }
                        if (checking == 0) {
                                push(temp_path);
                        }
                    break;
                case DT_DIR:            
                        npath=malloc(strlen(path)+strlen(direntp->d_name)+2);
                        sprintf(npath,"%s/%s",path, direntp->d_name);
                        check_new_files(npath,    oldfiles, file_count);
                        free(npath);
                        break;
        }
    }
    closedir(dir);
    return 0;
}

int count_files(char *path)
{
    DIR *dir = NULL;
    struct dirent *direntp = NULL;

    char *npath;
    int count=0;

    if (!path) {
            fprintf(fp, "[%s %s] given path is null", __DATE__, __TIME__);
            exit(1);
    }

    dir = opendir(path);

    if (dir == NULL ) { 
            fprintf(fp, "[%s %s] directory %s does not exist", __DATE__, __TIME__, path);
            fflush(fp);
            exit(1);
    }

    while( (direntp = readdir(dir))) {
            if (strcmp(direntp->d_name,".") == 0 || strcmp(direntp->d_name,"..") == 0) {
                    continue;
            }

            switch (direntp->d_type) {
                case DT_REG:
                        count++;
                        break;
                case DT_DIR:            
                        npath=malloc(strlen(path)+strlen(direntp->d_name)+2);
                        sprintf(npath,"%s/%s",path, direntp->d_name);
                        count += count_files(npath);
                        free(npath);
                        break;
            }
    }
    closedir(dir);
    return count;
}