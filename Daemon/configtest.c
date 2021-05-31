#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <libconfig.h>

struct config {
    struct audio *audio_type;
    struct photo *photo_type;
    struct document *document_type;
    struct video *video_type; 
    const char *dir_to_watch;
    char *types_to_watch;
};

struct audio {
    const char *types[10];
    int monitor;
};
struct photo {
    const char *types[10];
    int monitor;
};
struct document{
    const char *types[10];
    int monitor;
};
struct video {
    const char *types[10];
    int monitor;
};

int main(int argc, char* argv[])
{
    config_t cfg, *cf;
    config_setting_t *setting, *types, *types_selected;
    FILE *fp;

    struct config config;

    const char *strs[10];
    char typeselected[40];
    int types_count = 0;
    int types_selected_count = 0;
    const char *str;
    const char *dir_str;
    int file_count = 0;
    char output_file[14]; 
    int new_file_count = 0;

    strcpy(output_file, "DaemonLog.txt");

    fp = fopen(output_file, "w+");
    if (fp == NULL) {
        return 0;
    }

    cf = &cfg;

    config_init(cf);

    // skaitome config failą
    if(!config_read_file(cf, "config.cfg")) {
        fprintf(fp, "%s:%d - %s\n", config_error_file(cf),
        config_error_line(cf), config_error_text(cf));
        config_destroy(cf);
        return(EXIT_FAILURE);
    }

    // gauname direction to watch reikšmę
    if(config_lookup_string(&cfg, "dir_to_watch", &dir_str))
    printf("Direction to watch: %s\n\n", dir_str);

        if(config_lookup_string(&cfg, "audio", &dir_str))
    printf("Direction to watch: %s\n\n", dir_str);

    // gauname types to wach reikšmę
    types = config_lookup(cf, "types_to_watch");
    // kiekį elementų, kadangi list'as
    types_count = config_setting_length(types);
    // kiekvieno tipo išvedame 
    for(int j = 0; j < types_count; j++) {

        strcpy(typeselected, config_setting_get_string_elem(types, j));
        printf("%s \n", typeselected);

        types_selected = config_lookup(cf, typeselected);

        types_selected_count = config_setting_length(types_selected);
        printf("tipu kiekis: %d\n", types_selected_count);
        
        for (int i = 0; i<types_selected_count; i++) {
            if (strcmp(config_setting_get_string_elem(types, j), "audio") == 0) {
                             printf("tuu\t#%d. %s\n", i + 1,
             config_setting_get_string_elem(types_selected, i));
                config.audio_type->types[i] = config_setting_get_string_elem(types_selected, i);

            } else if (strcmp(config_setting_get_string_elem(types, j), "photo") == 0)  {
                config.photo_type->types[i] = config_setting_get_string_elem(types_selected, i);
                
            } else if (strcmp(config_setting_get_string_elem(types, j), "document") == 0)  {
                 printf("tuu\t#%d. %s\n", i + 1,
             config_setting_get_string_elem(types_selected, i));
                config.document_type->types[i] = config_setting_get_string_elem(types_selected, i);
                
            } else if (strcmp(config_setting_get_string_elem(types, j), "video") == 0)  {  
                config.video_type->types[i] = config_setting_get_string_elem(types_selected, i);
            }
        }
    }

    printf("%s\n", config.document_type->types[2]);

    fclose(fp);
}