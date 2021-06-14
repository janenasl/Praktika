#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <errno.h>

#include "LinkedList.h"
#include "logger.h"
#include "mainHeader.h"
#include "fileMover.h"

#define CONFIGFILE "/etc/DaemonConfig.conf"

int main(int argc, char* argv[])
{
    struct config *config = NULL;

    int rc = 0;
    rc = init_logger();
    if (rc) {
        goto cleanup_1;
    }

    write_to_log(LOG_INFO, "----- program started ------");
    config = read_config();
    if (config == NULL) {
            goto cleanup_2;
    }

    //daemon_process();
    main_process(config);

    cleanup_2:
        cleanup();
        free(config);
    cleanup_1:
        return 0;
}

int main_process(struct config *config)
{
    char **not_movable; 
    char **new_files;
    int file_count = 0;
    int temp_file_count = 0;
    int new_files_count = 0;
    int not_movable_count = 0;

    file_count = count_files(config->dir_to_watch);
    if(file_count == -1) {
            goto clean_1;
    }

    if(save_not_movable_files(config->dir_to_watch) == 1) {
            goto clean_1;
    }
    not_movable = take_list(length());
    not_movable_count = length();
    delete_list();

    while(1) {
            sleep(1);
            temp_file_count = count_files(config->dir_to_watch);
            if(temp_file_count == -1) {
                    goto clean_1;
            }

            if (temp_file_count != file_count) {
                    if (check_new_files(config->dir_to_watch, not_movable, not_movable_count) == 1) {
                            goto clean_1;
                    }

                    new_files = take_list(length());
                    new_files_count = length();
                    clean_arrays(not_movable_count, not_movable);

                    if (check_if_file_move(new_files, *config, length()) == 1) {
                            goto clean_2;
                    }
                    clean_arrays(new_files_count, new_files);
                    delete_list();

                    if(save_not_movable_files(config->dir_to_watch) == 1) {
                            goto clean_1;
                    }

                    not_movable = take_list(length());
                    not_movable_count = length();
                    delete_list();

                    file_count = temp_file_count;
                    break;
            }
    }
    clean_2:
            if (length() > 0) 
            clean_arrays(new_files_count, new_files);
    clean_1:
            clean_arrays(not_movable_count, not_movable);
            if (length() > 0) {
                    delete_list();
            }
        return 1;
}

int clean_arrays(int count, char **array) 
{
    for (int i = 0; i < count; i++) {
            free(array[i]);
    }
    free(array);
    return 0;
}

int check_if_file_move(char **files, struct config config, int file_count)
{
    char extension[10];
    char file_name[200];
    char *owner;
    char temp[150];

    owner = find_owner();
    if (owner == NULL) {
            write_to_log(LOG_ERROR, "couldn't find program owner. Program will be terminated.");
            return 1;
    }
    for(int i = 0; i < file_count; i++){
            sprintf(temp, "New file found: %s", files[i]);
            write_to_log(LOG_INFO, temp);
            strcpy(extension, get_filename_ext(files[i]));
            strcpy(file_name, get_file_name(files[i]));
            if ((check_extensions(config.photo_type.types, extension)) == 0 && config.photo_type.monitor == 1){
                    move_files(files[i], file_name, "Pictures", owner);
            } else if (check_extensions(config.document_type.types, extension) == 0 && config.document_type.monitor == 1) {
                    move_files(files[i], file_name, "Documents", owner);
            } else if (check_extensions(config.video_type.types, extension) == 0 && config.video_type.monitor == 1) {
                    move_files(files[i], file_name, "Videos", owner);
            } else if (check_extensions(config.audio_type.types, extension) == 0 && config.audio_type.monitor == 1) {
                    move_files(files[i], file_name, "Music", owner);
            } else {
                sprintf(temp, "file %s was not moved, because this type of file (.%s) is not monitored", files[i], extension);
                write_to_log(LOG_ERROR, temp);
            }
    }
    free(owner);
    return 0;
}



int move_files(char *file_full_path, char *file_name, char *directory_type, char *owner)
{
    char directory[strlen(owner)+strlen(directory_type)+10];
    char temp[150];

    sprintf(directory, "/home/%s/%s/", owner, directory_type);

    if (check_if_dir_exists(directory) != 0) {
            return 1;
    }
    sprintf(temp, "trying to move to: %s", directory);
    write_to_log(LOG_INFO, temp);
    sprintf(directory, "/home/%s/%s/%s", owner, directory_type, file_name);
    
    if (rename(file_full_path, directory) != 0) {
           sprintf(temp, "file %s was not moved", file_full_path);
            write_to_log(LOG_ERROR, "file was not moved");
    } else {
            sprintf(temp, "file was moved to: %s", directory);
            write_to_log(LOG_INFO, temp);
    }
    return 0;
}

struct config *read_config()
{
    int symbols = 0;
    char line[200];

    FILE *fp_config = NULL;
    struct config *config = malloc(sizeof(struct config)); 

    if (config == NULL) {
            return NULL;
    }

    config->audio_type.monitor = 0;
    config->video_type.monitor = 0;
    config->document_type.monitor = 0;
    config->photo_type.monitor = 0;

    if ((fp_config = fopen(CONFIGFILE, "r")) == NULL) {
            write_to_log(LOG_ERROR, "Can't find config file!");
            return NULL;
    }

    symbols = count_symbols(CONFIGFILE);

    if (symbols == -1) {
            return NULL;
    }
    
	while (fgets(line, symbols, fp_config) != NULL) { 
            if (strlen(line) > 5) {
                    remove_chars(line, '\n');
                    if (set_configurations(line, config) != 0) {
                            return NULL;
                    }
            }
    }

    fclose(fp_config);

    return config;
}

int set_configurations(char *line, struct config *config)
{
    char *ptr;
    ptr = (char *) malloc(sizeof(char) * strlen(line)+1);

    if (ptr == NULL) {
            write_to_log(LOG_ERROR, "problem occured with malloc");
            return 1;
    }

    strcpy(ptr, strrchr(line, ' ')+1);
    
    if (strstr(line, "audio_types =") != NULL) {
        strcpy(config->audio_type.types, ptr);
    } else if (strstr(line, "video_types =") != NULL) {
        strcpy(config->video_type.types, ptr);
    } else if (strstr(line, "document_types =") != NULL) {
            strcpy(config->document_type.types, ptr);
    } else if (strstr(line, "photo_types =") != NULL) {
        strcpy(config->photo_type.types, ptr);
    } else if (strstr(line, "types_to_watch =") != NULL) {
            strcpy(config->types_to_watch, ptr);
            if ((strstr(ptr, "audio") != NULL )) {
                    config->audio_type.monitor = 1;
            }
            if ((strstr(ptr, "video") != NULL )) {
                    config->video_type.monitor = 1;
            }
            if ((strstr(ptr, "document") != NULL )) {
                    config->document_type.monitor = 1;
            }
            if ((strstr(ptr, "photo") != NULL )) {
                    config->photo_type.monitor = 1;
            }
    } else if (strstr(line, "dir_to_watch =") != NULL) {
                strcpy(config->dir_to_watch, ptr);
    }

    free(ptr);

    return 0;
}

int daemon_process()
{
    pid_t process_id = 0;
    pid_t sid = 0;

    process_id = fork();

    if (process_id < 0) {
            goto cleanup;
    }
    if (process_id > 0){
            goto cleanup;
    }

    sid = setsid();

    if (sid < 0){
            goto cleanup;
    }
    umask(0);
    chdir("/");

    close(STDERR_FILENO);
    close(STDOUT_FILENO);
    close(STDIN_FILENO);

    return 0;
    cleanup:
        cleanup();
        exit(0);
}