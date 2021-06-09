#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <errno.h>

#include "functions.h"
#include "LinkedList.h"
#include "files.h"

#define CONFIGFILE "/etc/DaemonConfig.conf"
#define LOGFILE "/var/log/DaemonLog.txt"

int main(int argc, char* argv[])
{
    struct config config;
    open_file();

    fprintf(fp, "[%s %s] --------- PROGRAM STARTED ----------\n", __DATE__, __TIME__ );
    fflush(fp);

    config = read_config();
    daemon_process();
    main_process(config);
}

void main_process(struct config config)
{
    char **not_movable; 
    char **new_files;
    int file_count = 0;
    int new_file_count = 0;
    int not_movable_count = 0;

    open_file();

    file_count = count_files(config.dir_to_watch);
    save_not_movable_files(config.dir_to_watch);
    not_movable = take_list(length());
    not_movable_count = length();
    delete_list();

    while(1) {
            sleep(1);
            new_file_count = count_files(config.dir_to_watch);
            if (new_file_count != file_count) {
                    check_new_files(config.dir_to_watch, not_movable, not_movable_count);
                    new_files = take_list(length());
                    check_if_file_move(new_files, config, length());
                    delete_list();
                    save_not_movable_files(config.dir_to_watch);
                    not_movable = take_list(length());
                    not_movable_count = length();
                    delete_list();

                    file_count = new_file_count;
            }
    }

    delete_list();

    if (fclose(fp) != 0) {
            fprintf(fp, "[%s %s] closing file failed!\n", __DATE__, __TIME__);
    }

}

void open_file() 
{
    fp = fopen(LOGFILE, "a");
    if (fp == NULL) {
            printf("Can't open log file, program will be terminated! \n");
            exit(1);
    }
}

void check_if_file_move(char **files, struct config config, int file_count)
{
    char extension[100];
    char file_name[100];

    for(int i = 0; i < file_count; i++){
            fprintf(fp, "[%s %s] New file found: %s\n", __DATE__, __TIME__, files[i]);
            strcpy(extension, get_filename_ext(files[i]));
            strcpy(file_name, get_file_name(files[i]));
            if (check_extensions(config.photo_type.types, extension) == 0 && config.photo_type.monitor == 1){
                    move_files(files[i], file_name, "Pictures");
            } else if (check_extensions(config.document_type.types, extension) == 0 && config.document_type.monitor == 1) {
                    move_files(files[i], file_name, "Documents");
            } else if (check_extensions(config.video_type.types, extension) == 0 && config.video_type.monitor == 1) {
                    move_files(files[i], file_name, "Videos");
            } else if (check_extensions(config.audio_type.types, extension) == 0 && config.audio_type.monitor == 1) {
                    move_files(files[i], file_name, "Music");
            } else {
                  fprintf(fp, "[%s %s] file %s was not moved, because this type of file (.%s) is not monitored\n", __DATE__, __TIME__, files[i], extension);
            }
    }
    fflush(fp);
}



void move_files(char *file_full_path, char *file_name, char *sprintf_text)
{
    char directory[200];
    sprintf(directory, "/home/%s/%s/", find_owner(), sprintf_text);
    check_if_dir_exists(directory);
    fprintf(fp, "[%s %s] trying to move to: %s\n", __DATE__, __TIME__, directory);
    sprintf(directory, "/home/%s/%s/%s", find_owner(), sprintf_text, file_name);
    if (rename(file_full_path, directory) != 0) {
        fprintf(fp, "[%s %s] file %s was not moved (bad path)\n", __DATE__, __TIME__, file_full_path);
    } else {
        fprintf(fp, "[%s %s] file moved to: %s\n", __DATE__, __TIME__, directory);
    }
}

char *get_file_name(char *str)
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


char *find_owner()
{
    struct passwd* pw;
    char *owner = NULL;

    if((pw = getpwuid(getuid())) == NULL) {
            fprintf( fp, "[%s %s] getpwuid: no password entry\n", __DATE__, __TIME__);
            exit(1);
    }

    owner = (char *) malloc(sizeof(char) * strlen(pw->pw_name)+1);

    if (owner == NULL) {
        return NULL;
    }

    strcpy(owner, pw->pw_name);
    return owner;
}

// int check_not_movable_files(char *path)
// {
//     DIR *dir = NULL;
//     struct dirent *direntp = NULL;

//     char *npath;
//     char *temp_path;

//     if (!path) {
//             fprintf(fp, "[%s %s] given path is null", __DATE__, __TIME__);
//             exit(1);
//     }

//     dir = opendir(path);

//     if (dir == NULL ) { 
//             fprintf(fp, "[%s %s] directory %s does not exist", __DATE__, __TIME__, path);
//             fflush(fp);
//             exit(1);
//     }

//     while( (direntp = readdir(dir))) {
//             if (strcmp(direntp->d_name,".") == 0 || strcmp(direntp->d_name,"..") == 0) {
//                     continue;
//             }

//             switch (direntp->d_type) {
//                     case DT_REG:
//                             temp_path = malloc(strlen(path)+strlen(direntp->d_name)+2);
//                             sprintf(temp_path, "%s/%s", path, direntp->d_name);
//                             push(temp_path);
//                             break;
//                     case DT_DIR:            
//                             npath=(char *) malloc(sizeof(char) * (strlen(path)+strlen(direntp->d_name)+2));
//                             sprintf(npath,"%s/%s",path, direntp->d_name);
//                             check_not_movable_files(npath);
//                             free(npath);
//                             break;
//             }
//     }
//     closedir(dir);
//     return 0;
// }

// int check_new_files(char *path, char **oldfiles, int file_count)
// {
//     DIR *dir = NULL;
//     struct dirent *direntp = NULL;

//     char *npath;
//     char *temp_path;
//     int checking = 0;

//     if (!path) {
//             fprintf(fp, "[%s %s] given path is null", __DATE__, __TIME__);
//             exit(1);
//     }

//     dir = opendir(path);

//     if (dir == NULL ) { 
//             fprintf(fp, "[%s %s] directory %s does not exist", __DATE__, __TIME__, path);
//             fflush(fp);
//             exit(1);
//     }

//     while( (direntp = readdir(dir))) {
//         if (strcmp(direntp->d_name,".") == 0 || strcmp(direntp->d_name,"..") == 0) {
//                 continue;
//         }

//         switch (direntp->d_type) {
//                 case DT_REG:
//                         checking = 0;
//                         temp_path = malloc(strlen(path)+strlen(direntp->d_name)+2);
//                         sprintf(temp_path, "%s/%s", path, direntp->d_name);
//                         for(int i = 0; i<file_count; i++) {
//                                 if (strcmp(oldfiles[i],temp_path) == 0) {
//                                         checking = 1;
//                                 }
//                         }
//                         if (checking == 0) {
//                                 push(temp_path);
//                         }
//                     break;
//                 case DT_DIR:            
//                         npath=malloc(strlen(path)+strlen(direntp->d_name)+2);
//                         sprintf(npath,"%s/%s",path, direntp->d_name);
//                         check_new_files(npath,    oldfiles, file_count);
//                         free(npath);
//                         break;
//         }
//     }
//     closedir(dir);
//     return 0;
// }

// int count_files(char *path)
// {
//     DIR *dir = NULL;
//     struct dirent *direntp = NULL;

//     char *npath;
//     int count=0;

//     if (!path) {
//             fprintf(fp, "[%s %s] given path is null", __DATE__, __TIME__);
//             exit(1);
//     }

//     dir = opendir(path);

//     if (dir == NULL ) { 
//             fprintf(fp, "[%s %s] directory %s does not exist", __DATE__, __TIME__, path);
//             fflush(fp);
//             exit(1);
//     }

//     while( (direntp = readdir(dir))) {
//             if (strcmp(direntp->d_name,".") == 0 || strcmp(direntp->d_name,"..") == 0) {
//                     continue;
//             }

//             switch (direntp->d_type) {
//                 case DT_REG:
//                         count++;
//                         break;
//                 case DT_DIR:            
//                         npath=malloc(strlen(path)+strlen(direntp->d_name)+2);
//                         sprintf(npath,"%s/%s",path, direntp->d_name);
//                         count += count_files(npath);
//                         free(npath);
//                         break;
//             }
//     }
//     closedir(dir);
//     return count;
// }

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

void remove_chars(char *s, char c)
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

int count_symbols(char *path)
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

struct config set_configuration(char *str, struct config config, char *type) 
{
    char ptr[100];
    strcpy(ptr, strchr(str, '='));
    remove_chars(ptr, ' ');
    remove_chars(ptr, '=');
    remove_chars(ptr, '\n');

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
    char *ptr = NULL;
    char line[symbols];

    if ((fp_config = fopen(CONFIGFILE, "r")) == NULL) {
            fprintf(fp, "[%s %s] Can't find config file!", __DATE__, __TIME__);
            exit(1);
    }

    symbols = count_symbols(CONFIGFILE);

	while (fgets(line, symbols, fp_config) != NULL) {   
            if (strstr(line, "audio_types =") != NULL) {
                    config = set_configuration(line, config, "audio");
            } else if (strstr(line, "video_types =") != NULL) {
                    config = set_configuration(line, config, "video");
            } else if (strstr(line, "document_types =") != NULL) {
                    config = set_configuration(line, config, "document");
            } else if (strstr(line, "photo_types =") != NULL) {
                    config = set_configuration(line, config, "photo");
            } else if (strstr(line, "types_to_watch =") != NULL) {
                    //neišskiriama atmintis ptr
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
                        remove_chars(ptr, ' ');
                        remove_chars(ptr, '=');
                        remove_chars(ptr, '\n');
                        strcpy(config.dir_to_watch, ptr);
            }
    }
    if (ptr != NULL) {
                free(ptr);
    }

    if(fclose(fp_config) != 0) {
        fprintf(fp, "[%s %s] closing config file failed!\n", __DATE__, __TIME__);
    }
    return config;
}

int daemon_process()
{
    pid_t process_id = 0;
    pid_t sid = 0;

    process_id = fork();

    if (process_id < 0) {
            goto clear_files;
    }
    if (process_id > 0){
            goto clear_files;
    }

    sid = setsid();

    if (sid < 0){
            goto clear_files;
    }
    umask(0);
    chdir("/");

    close(STDERR_FILENO);
    close(STDOUT_FILENO);
    close(STDIN_FILENO);

    return 0;

    clear_files:
        fclose(fp);
        exit(0);
}