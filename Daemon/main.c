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
char *checkFiles(char *path);
char *checkNewFile(char *path, char *oldfiles);
char *get_filename_ext(char *filename);
struct config read_config();

int main(int argc, char* argv[])
{
   // daemon_process();
    struct config config;
    FILE *fp = NULL;

    int file_count = 0;
    int new_file_count = 0;
    char log_file[14]; 
    char *first_files = malloc (sizeof(first_files) * 500);
    char *new_files = malloc (sizeof(first_files) * 500);

    strcpy(log_file, "DaemonLog.txt");
    fp = fopen(log_file, "w+");
    if (fp == NULL) {
        return 0;
    }

    config = read_config();

    file_count = countFiles(config.dir_to_watch);

    first_files = checkFiles(config.dir_to_watch);
    printf("%s \n", first_files);
    fprintf(fp, "starting: %d\n", file_count);
    while(1) {
        sleep(1);
        new_file_count = countFiles(config.dir_to_watch);
        if (new_file_count != file_count) {
            fprintf(fp, "%d\n", new_file_count);
            checkNewFile(config.dir_to_watch, first_files);




            file_count = new_file_count;
        }
        fflush(fp);
    }
    free(first_files);
    fclose(fp);

}

int countFiles(char *path) {
    DIR *dir = NULL;
    struct dirent *direntp = NULL;
    char *npath;
    int count=0;
    char file_names[1000];
    
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

char *checkFiles(char *path) {
    DIR *dir = NULL;
    struct dirent *direntp = NULL;
    char *npath;
    char *file_names = malloc (sizeof(file_names) * 500);
    
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
                strcat(file_names, direntp->d_name);
                strcat(file_names, ",");
                break;
            case DT_DIR:            
                npath=malloc(strlen(path)+strlen(direntp->d_name)+2);
                sprintf(npath,"%s/%s",path, direntp->d_name);
                strcat(file_names, checkFiles(npath));
                free(npath);
                break;
        }
    }
    closedir(dir);
    return file_names;
}

char *checkNewFile(char *path, char *oldfiles) {
    DIR *dir = NULL;
    struct dirent *direntp = NULL;
    char *npath;
    char *file_names = malloc (sizeof(file_names) * 500);
    
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
           // printf("bandau\n");
                if(strstr(oldfiles, direntp->d_name) == NULL) {
                    printf("naujas: %s\n", direntp->d_name);
                }
                break;
            case DT_DIR:            
                npath=malloc(strlen(path)+strlen(direntp->d_name)+2);
                sprintf(npath,"%s/%s",path, direntp->d_name);
                checkNewFile(npath, oldfiles);
                free(npath);
                break;
        }
    }
    closedir(dir);
    return file_names;
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

char *get_filename_ext(char *filename) {
    char *extension = strrchr(filename, '.');
    if(!extension || extension == filename) {
        return "";
    }
    return extension + 1;
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
        exit(1);
    }
	while (fgets(str, Symbols, fp) != NULL) {   
        if (strstr(str, "audio_types =") != NULL) {
            ptr = malloc (sizeof(char) * 100);
            strcpy(ptr, strchr(str, '='));
            removeChars(ptr, '\n');
            strcpy(pradinis.audio_type.types, ptr);
          
        }
        else if (strstr(str, "video_types =") != NULL) {
            ptr = malloc (sizeof(char) * 100);
            strcpy(ptr, strchr(str, '='));
            removeChars(ptr, '\n');
            strcpy(pradinis.video_type.types, ptr);
        }
        else if (strstr(str, "document_types =") != NULL) {
            ptr = malloc (sizeof(char) * 100);
            strcpy(ptr, strchr(str, '='));
            removeChars(ptr, '\n');
            strcpy(pradinis.document_type.types, ptr);
        }
        else if (strstr(str, "photo_types =") != NULL) {
            ptr = malloc (sizeof(char) * 100);
            strcpy(ptr, strchr(str, '='));
            removeChars(ptr, '\n');
            strcpy(pradinis.photo_type.types, ptr);
        }
        else if (strstr(str, "types_to_watch =") != NULL) {
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

        }
        else if (strstr(str, "dir_to_watch =") != NULL) {
            ptr = malloc (sizeof(char) * 100);
            strcpy(ptr, strchr(str, '='));
            removeChars(ptr, ' ');
            removeChars(ptr, '=');
            removeChars(ptr, '\n');
            strcpy(pradinis.dir_to_watch, ptr);
        }
        line++;
    }
        free(ptr);
	    rv = fclose( fp );
	    if( rv != 0 ) {
         exit(0);
        }
        return (pradinis);
}