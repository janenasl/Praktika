#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

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

static void daemon_process();
void removeChars(char *s, char c);
int countFiles(char *path);
int countSymbols(char *p);
struct config read_config();

int main(int argc, char* argv[])
{
    //daemon_process();
    struct config config;
    FILE *fp = NULL;

    const char *str;
    int file_count = 0;
    int new_file_count = 0;
    char log_file[14]; 

    strcpy(log_file, "DaemonLog.txt");
    fp = fopen(log_file, "w+");
    if (fp == NULL) {
        return 0;
    }

    config = read_config();

    file_count = countFiles(config.dir_to_watch);

    printf("%d \n", file_count);
    fprintf(fp, "starting: %d\n", file_count);

    while(1) {
        sleep(1);
        new_file_count = countFiles(config.dir_to_watch);
        if (new_file_count != file_count) {
            fprintf(fp, "%d\n", new_file_count);







            file_count = new_file_count;
        }
        fflush(fp);
    }
    fclose(fp);

}

int countFiles(char *path) {
    DIR *dir = NULL;
    struct dirent *direntp = NULL;
    char *npath;
    int count=0;
    
    if (!path) { 
        return 0;
    }

    dir = opendir(path);

    if(dir == NULL ) { 
        return 0;
    }

    while( (direntp = readdir(dir))) {
        if (strcmp(direntp->d_name,".")==0 || strcmp(direntp->d_name,"..")==0) {
            continue;
        }

        switch (direntp->d_type) {
            case DT_REG:
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

int countSymbols(char *p)
{
	FILE *fp;
	int count = 0;
	char c;
	
	if ((fp = fopen(p, "r")) == NULL) {
        perror(p);
    }
	for (c = getc(fp); c != EOF; c = getc(fp)) {
        count = count + 1; 
	}
	return count;
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
    chdir("/var/log");

    for (x = sysconf (_SC_OPEN_MAX); x>=0; x--){
        close(x);
    }
    // close(STDERR_FILENO);
    // close(STDOUT_FILENO);
    // close(STDERR_FILENO);
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
	int line = 1;
    int i = 0;
	int rv;
    char *ptr;
	Symbols = countSymbols("config.cfg");
	char str[Symbols];
    if ((fp = fopen("config.cfg", "r")) == NULL) {
        perror("config.cfg");
    }
	while (fgets(str, Symbols, fp) != NULL) {   
        if (strstr(str, "audio_types =") != NULL) {
            ptr = malloc (sizeof(char) * 100);
            ptr = strchr(str, '=');
            strtok(ptr, "\n");
            strcpy(pradinis.audio_type.types, ptr);
            printf("tipai: %s\n", pradinis.audio_type.types);
        }
        if (strstr(str, "video_types =") != NULL) {
            ptr = malloc (sizeof(char) * 100);
            ptr = strchr(str, '=');
            strtok(ptr, "\n");
            strcpy(pradinis.video_type.types, ptr);
            printf("tipai: %s\n", pradinis.video_type.types);
        }
        if (strstr(str, "document_types =") != NULL) {
            ptr = malloc (sizeof(char) * 100);
            ptr = strchr(str, '=');
            strtok(ptr, "\n");
            strcpy(pradinis.document_type.types, ptr);
            printf("tipai: %s\n", pradinis.document_type.types);
        }
        if (strstr(str, "photo_types =") != NULL) {
            ptr = malloc (sizeof(char) * 100);
            ptr = strchr(str, '=');
            strtok(ptr, "\n");
            strcpy(pradinis.photo_type.types, ptr);
            printf("tipai: %s\n", pradinis.photo_type.types);
        }
        if (strstr(str, "types_to_watch =") != NULL) {
           ptr = strchr(str, '=');
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

        }
        if (strstr(str, "dir_to_watch =") != NULL) {
            ptr = malloc (sizeof(char) * 100);
            ptr = strchr(str, '=');
            strtok(ptr, "\n");
            removeChars(ptr, ' ');
            removeChars(ptr, '=');
            strcpy(pradinis.dir_to_watch, ptr);
        }
        line++;
    }
	    rv = fclose( fp );
	    if( rv != 0 )
	    perror ( "fclose() failed" );
        printf("dir:%s\n", pradinis.dir_to_watch);
        printf("types:%s\n", pradinis.types_to_watch); 
        return (pradinis);
}