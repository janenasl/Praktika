#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <limits.h>
#include <errno.h>

#include "lib/dynamiclibrary.h"
#include "functions.h"

FILE *fp;

int main(int argc, char* argv[])
{
    struct config config;
    char **old_files; 
    char **new_files;
    int file_count = 0;
    int new_file_count = 0;
    int old_files_count = 0;

    fp = fopen("/var/log/DaemonLog.txt", "a");
    if (fp == NULL) {
            printf("Can't open log file, program will be terminated. Try with again with root privilegies! \n");
            exit(1);
    }

    fprintf(fp, "[%s %s] -------------------\n" ,__DATE__, __TIME__ );
    fprintf(fp, "[%s %s] program owner: %s\n", __DATE__, __TIME__, findOwner());
    fflush(fp);

    config = read_config();
    daemon_process();

    file_count = countFiles(config.dir_to_watch);
    checkFiles(config.dir_to_watch);
    old_files = takeList(length());
    old_files_count = length();
    deleteList();

    while(1) {
            sleep(1);
            new_file_count = countFiles(config.dir_to_watch);
            if (new_file_count != file_count) {
                    checkNewFiles(config.dir_to_watch, old_files, old_files_count);
                    new_files = takeList(length());
                    checkingFileMove(new_files, config, length());
                    deleteList();
                    checkFiles(config.dir_to_watch);
                    old_files = takeList(length());
                    old_files_count = length();
                    deleteList();
                    file_count = new_file_count;
        }
    }

    deleteList();

    if (fclose(fp) != 0) {
            fprintf(fp, "[%s %s] closing file failed!\n", __DATE__, __TIME__);
    }
}

int checkingFileMove(char **files, struct config config, int file_count)
{
    char extension[100];
    char file_name[100];

    fp = fopen("/var/log/DaemonLog.txt", "a");
    if (fp == NULL) {
            printf("Can't open log file, program will be terminated! \n");
            exit(1);
    }

    for(int i = 0; i < file_count; i++){
            fprintf(fp, "[%s %s] New file found: %s\n", __DATE__, __TIME__, files[i]);
            strcpy(extension, get_filename_ext(files[i]));
            strcpy(file_name, find_file_name(files[i]));
            if (check_extensions(config.photo_type.types, extension) == 0 && config.photo_type.monitor == 1){
                    movingFiles(files[i], file_name, "Pictures");
            } else if (check_extensions(config.document_type.types, extension) == 0 && config.document_type.monitor == 1) {
                    movingFiles(files[i], file_name, "Documents");
            } else if (check_extensions(config.video_type.types, extension) == 0 && config.video_type.monitor == 1) {
                    movingFiles(files[i], file_name, "Videos");
            } else if (check_extensions(config.audio_type.types, extension) == 0 && config.audio_type.monitor == 1) {
                    movingFiles(files[i], file_name, "Music");
            } else {
                  fprintf(fp, "[%s %s] file %s was not moved, because this type of file (.%s) is not monitored\n", __DATE__, __TIME__, files[i], extension);
            }
    }
    fflush(fp);
    return 0;
}



int movingFiles(char *file_full_path, char *file_name, char *sprintf_text)
{
    char directory[200];
    sprintf(directory, "/home/%s/%s/", findOwner(), sprintf_text);
    check_if_dir_exists(directory);
    fprintf(fp, "[%s %s] trying to move to: %s\n", __DATE__, __TIME__, directory);
    sprintf(directory, "/home/%s/%s/%s", findOwner(), sprintf_text, file_name);
    if (rename(file_full_path, directory) != 0) {
        fprintf(fp, "[%s %s] file %s was not moved (bad path)\n", __DATE__, __TIME__, file_full_path);
    } else {
        fprintf(fp, "[%s %s] file moved to: %s\n", __DATE__, __TIME__, directory);
    }
    return 0;
}

char *find_file_name(char *str)
{
    char *file_name = strrchr(str, '/');
    if(!file_name || file_name == str) {
            return "";
    }
    return file_name + 1;
}

char *get_filename_ext(char *filename) 
{
    char *extension = strrchr(filename, '.');
    if(!extension || extension == filename) {
        return "";
    }
    return extension + 1;
}

int check_extensions(char *types, char *extension) 
{
    char *token;
    while ((token = strtok_r(types, ",", &types))) {
        if (strcmp(token, extension) == 0) {
            return 0;
        }
    }
    return 1;
}

int check_if_dir_exists(char *dir) 
{
    FILE *fptr = fopen(dir, "r");
    if (fptr == NULL){
            if (mkdir_p(dir) == 0) {
                    fprintf(fp, "[%s %s] directory to move file was not found, therefore created\n", __DATE__, __TIME__);
                    return 0;
            }       
            fprintf(fp, "[%s %s] directory to move file was not found and creating it failed\n", __DATE__, __TIME__);
            exit(1);
    }
    return 0;
}


char *findOwner()
{
    struct passwd* pw;
    char owner[50];
    char *s_owner = owner;

    if( ( pw = getpwuid( getuid() ) ) == NULL ) {
       fprintf( fp,
          "getpwuid: no password entry\n" );
          exit(1);
    }
    strcpy(s_owner, pw->pw_name);
    return s_owner;
}

int checkFiles(char *path)
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
                            npath=malloc(strlen(path)+strlen(direntp->d_name)+2);
                            sprintf(npath,"%s/%s",path, direntp->d_name);
                            checkFiles(npath);
                            free(npath);
                            break;
            }
    }
    closedir(dir);
    return 0;
}

int checkNewFiles(char *path, char **oldfiles, int file_count)
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
                        checkNewFiles(npath,    oldfiles, file_count);
                        free(npath);
                        break;
        }
    }
    closedir(dir);
    return 0;
}

int countFiles(char *path)
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
                        count += countFiles(npath);
                        free(npath);
                        break;
            }
    }
    closedir(dir);
    return count;
}

int mkdir_p(const char *path)
{
    const size_t len = strlen(path);
    char _path[PATH_MAX];
    char *p; 

    errno = 0;

    /* Copy string so its mutable */
    if (len > sizeof(_path)-1) {
        errno = ENAMETOOLONG;
        return -1; 
    }   
    strcpy(_path, path);

    /* Iterate the string */
    for (p = _path + 1; *p; p++) {
        if (*p == '/') {

            *p = '\0';

            if (mkdir(_path, S_IRWXU) != 0) {
                if (errno != EEXIST)
                    return -1; 
            }

            *p = '/';
        }
    }   

    if (mkdir(_path, S_IRWXU) != 0) {
        if (errno != EEXIST)
            return -1; 
    }   

    return 0;
}

int removeChars(char *s, char c)
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

    return 0;
}

int countSymbols(char *path)
{
    FILE *fp_counter;
	int count = 0;
	char c;
	
	if ((fp_counter = fopen(path, "r")) == NULL) {
            fprintf(fp, "Can't open file when counting symbols!\n");
    }
	for (c = getc(fp_counter); c != EOF; c = getc(fp_counter)) {
            count = count + 1;
	}
	return count;
}

struct config configuration(char *str, struct config config, char *type) 
{
    char ptr[100];
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
    return config;
}

struct config read_config()
{
    FILE *fp_config;
    struct config config; 
    config.audio_type.monitor = 0;
    config.video_type.monitor = 0;
    config.document_type.monitor = 0;
    config.photo_type.monitor = 0;

	int symbols = 0;
    char *ptr;
    char line[symbols];
    char log_file[11];

    strcpy(log_file, "config.cfg");

    if ((fp_config = fopen(log_file, "r")) == NULL) {
            fprintf(fp, "[%s %s] Can't find config file!", __DATE__, __TIME__);
            exit(1);
    }

    symbols = countSymbols(log_file);

	while (fgets(line, symbols, fp_config) != NULL) {   
            if (strstr(line, "audio_types =") != NULL) {
                    config = configuration(line, config, "audio");
            } else if (strstr(line, "video_types =") != NULL) {
                    config = configuration(line, config, "video");
            } else if (strstr(line, "document_types =") != NULL) {
                    config = configuration(line, config, "document");
            } else if (strstr(line, "photo_types =") != NULL) {
                    config = configuration(line, config, "photo");
            } else if (strstr(line, "types_to_watch =") != NULL) {
                    strcpy(ptr, strchr(line, '='));
                    strcpy(config.types_to_watch, ptr);
                    if ((strstr(ptr, "audio") != NULL )) {
                            config.audio_type.monitor = 1;
                    }
                    if ((strstr(ptr, "video") != NULL )) {
                            config.video_type.monitor = 1;
                    }
                    if ((strstr(ptr, "document") != NULL )) {
                            config.document_type.monitor = 1;
                    }
                    if ((strstr(ptr, "photo") != NULL )) {
                            config.photo_type.monitor = 1;
                    }
            } else if (strstr(line, "dir_to_watch =") != NULL) {
                        ptr = malloc (strlen(line)+2);
                        strcpy(ptr, strchr(line, '='));
                        removeChars(ptr, ' ');
                        removeChars(ptr, '=');
                        removeChars(ptr, '\n');
                        strcpy(config.dir_to_watch, ptr);
            }
    }

    free(ptr);

    if(fclose(fp_config) != 0) {
        fprintf(fp, "[%s %s] closing config file failed!\n", __DATE__, __TIME__);
    }
    return config;
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

    for (x = sysconf (_SC_OPEN_MAX); x>=0; x--){
            close(x);
    }
    // close(STDERR_FILENO);
    // close(STDOUT_FILENO);
    // close(STDERR_FILENO);
}