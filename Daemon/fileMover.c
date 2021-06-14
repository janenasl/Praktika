#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <errno.h>

#include "LinkedList.h"
#include "logger.h"
#include "fileMover.h"

/* push files to LinkedList */
int save_not_movable_files(char *path)
{
    DIR *dir = NULL;
    struct dirent *direntp = NULL;

    char *npath = NULL;
    char *temp_path = NULL;
    int rc = 1;

    if (!path) {
            write_to_log(LOG_ERROR, "given path is null!\n");
            return 1;
    }

    dir = opendir(path);

    if (dir == NULL ) { 
            write_to_log(LOG_ERROR, "directory to watch does not exist\n");
            return 1;
    }

    while( (direntp = readdir(dir))) {
            if (strcmp(direntp->d_name,".") == 0 || strcmp(direntp->d_name,"..") == 0) {
                    continue;
            }

            switch (direntp->d_type) {
                    case DT_REG:
                            temp_path = malloc(strlen(path)+strlen(direntp->d_name)+2);
                            if (temp_path == NULL) {
                                    write_to_log(LOG_ERROR, "occured problem with malloc\n");
                                    goto clean;
                            }
                             sprintf(temp_path, "%s/%s", path, direntp->d_name);
                             push(temp_path);
                             free(temp_path);
                            break;
                    case DT_DIR:            
                            npath=(char *) malloc(sizeof(char) * (strlen(path)+strlen(direntp->d_name)+2));
                            if (npath == NULL) {
                                    write_to_log(LOG_ERROR, "occured problem with malloc\n");
                                    goto clean;
                            }
                            sprintf(npath,"%s/%s",path, direntp->d_name);
                            save_not_movable_files(npath);
                            free(npath);
                            break;
            }
    }
    rc = 0;

    clean: 
        closedir(dir);
        return rc;
}
/* check if files exist previously and if not push them to LinkedList */
int check_new_files(char *path, char **oldfiles, int file_count)
{
    DIR *dir = NULL;
    struct dirent *direntp = NULL;

    char *npath;
    char *temp_path;
    int checking = 0;
    int rc = 1;

    if (!path) {
            write_to_log(LOG_ERROR, "given path is null!\n");
            return 1;
    }

    dir = opendir(path);

    if (dir == NULL ) { 
            write_to_log(LOG_ERROR, "directory to watch does not exist\n");
            return 1;
    }

    while( (direntp = readdir(dir))) {
        if (strcmp(direntp->d_name,".") == 0 || strcmp(direntp->d_name,"..") == 0) {
                continue;
        }

        switch (direntp->d_type) {
                case DT_REG:
                        checking = 0;
                        temp_path = malloc(strlen(path)+strlen(direntp->d_name)+2);
                        if (temp_path == NULL) {
                                write_to_log(LOG_ERROR, "occured problem with malloc\n");
                                goto clean;
                        }
                        sprintf(temp_path, "%s/%s", path, direntp->d_name);
                        for(int i = 0; i<file_count; i++) {
                                if (strcmp(oldfiles[i],temp_path) == 0) {
                                        checking = 1;
                                }
                        }
                        if (checking == 0) {
                                push(temp_path);
                        }
                        free(temp_path);
                    break;
                case DT_DIR:            
                        npath=malloc(strlen(path)+strlen(direntp->d_name)+2);
                        if (npath == NULL) {
                                write_to_log(LOG_ERROR, "occured problem with malloc\n");
                                goto clean;
                        }
                        sprintf(npath,"%s/%s",path, direntp->d_name);
                        check_new_files(npath,    oldfiles, file_count);
                        free(npath);
                        break;
        }
    }
    rc = 0;
    clean:
        closedir(dir);
        return rc;
}
/* recursively count how many files there are in given directory*/
int count_files(char *path)
{
    DIR *dir = NULL;
    struct dirent *direntp = NULL;

    char *npath;
    int count=0;
    int rc;

    if (!path) {
            //fprintf(fp, "[%s %s] given path is null", __DATE__, __TIME__);
            write_to_log(LOG_ERROR, "given path is null!\n");
            return -1;
    }
    dir = opendir(path);

    if (dir == NULL ) { 
            write_to_log(LOG_ERROR, "directory to watch does not exist\n");
            return -1;
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
                        if (npath == NULL) {
                                write_to_log(LOG_ERROR, "occured problem with malloc\n");
                                goto clean;
                        }
                        sprintf(npath,"%s/%s",path, direntp->d_name);
                        count += count_files(npath);
                        free(npath);
                        break;
            }
    }
    rc = count;

    clean:
        closedir(dir);
        //cleanup();
        return rc;
}
/*get file name from full path*/
char *get_file_name(char *str)
{
    char *file_name = strrchr(str, '/');
    if(!file_name || file_name == str) {
            return "";
    }
    return file_name + 1;
}
/*extract file extension from file name*/
char *get_filename_ext(char *filename) 
{
    char *extension = strrchr(filename, '.');
    if(!extension || extension == filename) {
        return "";
    }
    return extension + 1;
}
/*check if given file extension matches given extension types from config file*/
int check_extensions(char *types, char *extension)
{
    char *token;
    char temp_types[strlen(types)+1];
    
    strcpy(temp_types, types);
    token = strtok(temp_types, ",");
    while (token != NULL) {
        if (strcmp(token, extension) == 0) {
            return 0;
        }
        token = strtok(NULL, ",");
    }
    return 1;
}
/*checking if directory exist, if not we create that directory*/
int check_if_dir_exists(char *dir) 
{
    FILE *fptr = fopen(dir, "r");
    if (fptr == NULL){
            if (mkdir_p(dir) == 0) {
                   write_to_log(LOG_WARN, "directory to move file was not found, therefore created");
                    return 0;
            } 
            write_to_log(LOG_ERROR, "directory to move file was not found and creating it failed.");
            return 1;
    }
    fclose(fptr);
    return 0;
}

/* find program owner */
char *find_owner()
{
    struct passwd* pw;
    char *owner = NULL;

    pw = getpwuid(getuid());

    if(pw == NULL) {
            write_to_log(LOG_ERROR, "getpwuid: no password entry");
            return NULL;
    }

    owner = (char *) malloc(sizeof(char) * strlen(pw->pw_name)+1);

    if (owner == NULL) {
        return NULL;
    }

    strcpy(owner, pw->pw_name);
    return owner;
}
/* return symbol count of a text file */
int count_symbols(char *path)
{
    FILE *fp_counter;
	int count = 0;
	char c;
	
	if ((fp_counter = fopen(path, "r")) == NULL) {
            write_to_log(LOG_ERROR, "can't open file or file do not exist when try to count symbols!");
            return -1;
    }
	for (c = getc(fp_counter); c != EOF; c = getc(fp_counter)) {
            count += 1;
	}

    fclose(fp_counter);
	return count;
}
/* recursively create directory */
int mkdir_p(const char *path)
{
    const size_t len = strlen(path);
    char _path[PATH_MAX];
    char *p; 

    errno = 0;


    if (len > sizeof(_path)-1) {
        errno = ENAMETOOLONG;
        return -1; 
    }
    strcpy(_path, path);

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

/* remove char c from given string s*/
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