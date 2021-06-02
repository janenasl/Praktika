#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>

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
int movingFiles(char *files, struct config config, char *owner);
char *checkFiles(char *path);
char *checkNewFiles(char *path, char *oldfiles);
char *get_filename_ext(char *filename);
int check_extensions(char *types, char *extension);
char *find_file_name(char *str);
int check_if_dir_exists(char *dir);
struct config read_config();

int main(int argc, char* argv[])
{
   // daemon_process();
    struct config config;
    struct passwd* pw;
    FILE *fp = NULL;

    int file_count = 0;
    int new_file_count = 0;
    char log_file[14]; 
    char *first_files = malloc (sizeof(first_files) * 1000);
    char *new_files = malloc (sizeof(first_files) * 1000);
    char owner[20]; 

    config = read_config();

    if( ( pw = getpwuid( getuid() ) ) == NULL ) {
       fprintf( stderr,
          "getpwuid: no password entry\n" );
       return( EXIT_FAILURE );
    }
    strcpy(owner, pw->pw_name);


    strcpy(log_file, "DaemonLog.txt");
    fp = fopen(log_file, "w+");
    if (fp == NULL) {
        return 0;
    }

    file_count = countFiles(config.dir_to_watch);

    strcpy(first_files,checkFiles(config.dir_to_watch));

    fprintf(fp, "starting: %d\n", file_count);

    while(1) {
        sleep(1);
        new_file_count = countFiles(config.dir_to_watch);
        if (new_file_count != file_count) {
            fprintf(fp, "%d\n", new_file_count);
            strcpy(new_files,checkNewFiles(config.dir_to_watch, first_files));
            movingFiles(new_files, config, owner);
            file_count = new_file_count;
        }
        fflush(fp);
    }
    free(new_files);
    free(first_files);
    fclose(fp);

}

int movingFiles(char *files, struct config config, char *owner) {
    const char delimiter[2] = ",";
    char *token;
    char *extension;
    char *temp_token = malloc(sizeof(temp_token) * 200);
    char directory[500];

    char *file_name = malloc(sizeof(file_name) * 500);

    token = strtok(files, delimiter);
   
    while( token != NULL ) {
        printf("tokenas: %s \n", token);
        extension = get_filename_ext(token);
        strcpy(file_name, find_file_name(token));
        if (check_extensions(config.photo_type.types, extension) == 0 ){
            printf("FAILAS KELIAMAS\n");
            sprintf(directory, "/home/%s/Pictures/", owner);
            check_if_dir_exists("/home/ragaaaas");
            sprintf(directory, "/home/%s/Pictures/%s", owner, file_name);
            if (rename(token, directory) != 0) {
                printf("nepavyko :( \n");
            }
        }
        else if (check_extensions(config.video_type.types, extension) == 0){
            printf("FAILAS KELIAMAS\n");
            sprintf(directory, "/home/%s/Videos/%s", owner, file_name);
            if (rename(token, directory) != 0) {
                printf("nepavyko :( \n");
            }
        }
        else if (check_extensions(config.document_type.types, extension) == 0){
            printf("FAILAS KELIAMAS\n");
            sprintf(directory, "/home/%s/Documents/%s", owner, file_name);
            printf("dokumentas: %s\n", directory);
            printf("tokenas: %s\n", token);
            if (rename(token, directory) != 0) {
                perror("Error");
                printf("nepavyko :( \n");
            }
        }
        else if (check_extensions(config.audio_type.types, extension)==0){
            printf("FAILAS KELIAMAS\n");
            sprintf(directory, "/home/%s/Music/%s", owner, file_name);
            if (rename(token, directory) != 0) {
                printf("nepavyko :( \n");
            }
        }
        token = strtok(NULL, delimiter);
   }
}

int check_if_dir_exists(char *dir) {

    FILE *fptr = fopen(dir, "r");

    // If file does not exists 
    if (fptr == NULL){
        mkdir(dir, 0777);
        printf("CREATED\n");
        return 0;
    }
    
    return 0;
}

char *find_file_name(char *str) {
        char *file_name = strrchr(str, '/');
    if(!file_name || file_name == str) {
        return "";
    }
    printf("failo vardas: %s\n", file_name);
    return file_name + 1;
}

int check_extensions(char *types, char *extension) {
    char *token;
    token = strtok(types, ",");
   
    while( token != NULL ) {
        if (strcmp(token, extension) == 0){
            return 0;
        }
        
      token = strtok(NULL, ",");
   }
    return 1;
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
    char *file_names = malloc (sizeof(file_names) * 1000);
    strcpy(file_names, "");
    

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
                strcat(file_names, path);
                strcat(file_names, "/");
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

char *checkNewFiles(char *path, char *oldfiles) {
    DIR *dir = NULL;
    struct dirent *direntp = NULL;
    char *npath;
    char *temp_file_names;
    char *file_names;
    file_names = malloc (sizeof(file_names) * 1000);
    strcpy(file_names, "");

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
                temp_file_names = malloc(sizeof(temp_file_names)*500);
                strcpy(temp_file_names, "");
                strcat(temp_file_names, path);
                strcat(temp_file_names, "/");
                strcat(temp_file_names, direntp->d_name);
                if(strstr(oldfiles, temp_file_names) == NULL) {
                    strcat(file_names, ",");
                    strcat(file_names, path);
                    strcat(file_names, "/");
                    strcat(file_names, direntp->d_name);
                }
                free(temp_file_names);
                break;
            case DT_DIR:            
                npath=malloc(strlen(path)+strlen(direntp->d_name)+2);
                sprintf(npath,"%s/%s",path, direntp->d_name);
                strcat(file_names, checkNewFiles(npath, oldfiles));
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
            removeChars(ptr, ' ');
            removeChars(ptr, '=');
            removeChars(ptr, '\n');
            strcpy(pradinis.audio_type.types, ptr);
          
        }
        else if (strstr(str, "video_types =") != NULL) {
            ptr = malloc (sizeof(char) * 100);
            strcpy(ptr, strchr(str, '='));
            removeChars(ptr, ' ');
            removeChars(ptr, '=');
            removeChars(ptr, '\n');
            strcpy(pradinis.video_type.types, ptr);
        }
        else if (strstr(str, "document_types =") != NULL) {
            ptr = malloc (sizeof(char) * 100);
            strcpy(ptr, strchr(str, '='));
            removeChars(ptr, ' ');
            removeChars(ptr, '=');
            removeChars(ptr, '\n');
            strcpy(pradinis.document_type.types, ptr);
        }
        else if (strstr(str, "photo_types =") != NULL) {
            ptr = malloc (sizeof(char) * 100);
            strcpy(ptr, strchr(str, '='));
            removeChars(ptr, ' ');
            removeChars(ptr, '=');
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