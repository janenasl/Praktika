#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include "dynamiclibrary.h"

#define LOGVALUE 14

struct audio {
    char types[100];
    int monitor;
};
struct photo {
    char types[100];
    int monitor;
};
struct document{
    char types[100];
    int monitor;
};
struct video {
    char types[100];
    int monitor;
};

struct config {
    struct audio audio_type;
    struct photo photo_type;
    struct document document_type;
    struct video video_type; 
    char dir_to_watch[100];
    char types_to_watch[100];
};

struct config read_config();
int countFiles(char *path);
int countSymbols(char *p);
void removeChars(char *s, char c);
int logs(char *text);


int main(int argc, char* argv[])
{
    struct config config;
    int file_count = 0;

    config = read_config();

    file_count = countFiles(config.dir_to_watch);

    printf("file: %d\n", file_count);

    char **str = takeList(length());
    printf("gaunu: %s\n", str[12]);
    
    //printList();
    //daemon_process();
}

int countFiles(char *path)
{
    DIR *dir = NULL;
    struct dirent *direntp = NULL;
    char *npath;
    int count=0;

    char *temp_path;
    if (!path) { 
        return 0;
    }

    dir = opendir(path);

    if (dir == NULL ) { 
        return 0;
    }

    while( (direntp = readdir(dir))) {
        if (strcmp(direntp->d_name,".")==0 || strcmp(direntp->d_name,"..")==0) {
            continue;
        }

        switch (direntp->d_type) {
            case DT_REG:
                temp_path = malloc(sizeof(char) * 200);
                sprintf(temp_path, "%s/%s", path, direntp->d_name);
                push(temp_path);
                free(temp_path);
                count++;
                break;
            case DT_DIR:            
                npath=malloc(strlen(path)+strlen(direntp->d_name)+2);
                sprintf(npath,"%s/%s",path, direntp->d_name);
                count += countFiles(npath);
                free(npath);
                break;
        }
    }
    closedir(dir);
    return count;
}

int logs(char *text) 
{
    FILE *fp = NULL;

    fp = fopen("DaemonLog.txt", "a");
    if (fp == NULL) {
            fprintf(fp, "[%s %s] Can't open DaemonLog.txt\n", __DATE__, __TIME__);
            return 1;
    }

    fprintf(fp, "[%s %s] %s\n", __DATE__, __TIME__, text);

    fflush(fp);
    return 0;
}

void removeChars(char *s, char c)
{
    int writer = 0, reader = 0;

    while (s[reader])
    {
        if (s[reader]!=c) 
        {   
            s[writer++] = s[reader];
        }

        reader++;       
    }

    s[writer]=0;
}

int countSymbols(char *p)
{
	FILE *fp;
	int count = 0;
	char c;
	
	if ((fp = fopen(p, "r")) == NULL) {
        logs("Can't open file when counting symbols!\n");
    }
	for (c = getc(fp); c != EOF; c = getc(fp)) {
        count = count + 1; 
	}
	return count;
}

struct config configuration(char *str, struct config config, char *type) 
{
    char *ptr;
    ptr = malloc (sizeof(char) * 100);
    strcpy(ptr, strchr(str, '='));
    removeChars(ptr, ' ');
    removeChars(ptr, '=');
    removeChars(ptr, '\n');
    if (strcmp(type, "audio") == 0){
            strcpy(config.audio_type.types, ptr);
    } else if (strcmp(type, "video") == 0) {
            strcpy(config.video_type.types, ptr);
    } else if (strcmp(type, "document") == 0) {
            strcpy(config.document_type.types, ptr);
    } else if (strcmp(type, "photo") == 0) {
            strcpy(config.photo_type.types, ptr);
    }
    return (config);
}

struct config read_config()
{
    struct config pradinis; 
    pradinis.audio_type.monitor = 0;
    pradinis.video_type.monitor = 0;
    pradinis.document_type.monitor = 0;
    pradinis.photo_type.monitor = 0;
	int Symbols = 0;
	FILE *fp;
	int rv;
    char *ptr;
	Symbols = countSymbols("config.cfg");
	char str[Symbols];
    if ((fp = fopen("config.cfg", "r")) == NULL) {
        logs("Can't find config file!");
        exit(1);
    }
	while (fgets(str, Symbols, fp) != NULL) {   
            if (strstr(str, "audio_types =") != NULL) {
                    pradinis = configuration(str, pradinis, "audio");
            } else if (strstr(str, "video_types =") != NULL) {
                    pradinis = configuration(str, pradinis, "video");
            } else if (strstr(str, "document_types =") != NULL) {
                    pradinis = configuration(str, pradinis, "document");
            } else if (strstr(str, "photo_types =") != NULL) {
                    pradinis = configuration(str, pradinis, "photo");
            } else if (strstr(str, "types_to_watch =") != NULL) {
                    strcpy(ptr, strchr(str, '='));
                    strcpy(pradinis.types_to_watch, ptr);
                    if ((strstr(ptr, "audio") != NULL )) {
                            pradinis.audio_type.monitor = 1;
                    }
                    if ((strstr(ptr, "video") != NULL )) {
                            pradinis.video_type.monitor = 1;
                    }
                    if ((strstr(ptr, "document") != NULL )) {
                            pradinis.document_type.monitor = 1;
                    }
                    if ((strstr(ptr, "photo") != NULL )) {
                            pradinis.photo_type.monitor = 1;
                    }

            } else if (strstr(str, "dir_to_watch =") != NULL) {
                        ptr = malloc (sizeof(char) * 100);
                        strcpy(ptr, strchr(str, '='));
                        removeChars(ptr, ' ');
                        removeChars(ptr, '=');
                        removeChars(ptr, '\n');
                        strcpy(pradinis.dir_to_watch, ptr);
            }
    }

    free(ptr);
    rv = fclose(fp);
    if( rv != 0 ) {
        logs("Closing config file failed!");
        exit(1);
    }
    return (pradinis);
}