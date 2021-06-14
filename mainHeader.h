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

struct config *read_config();
int main_process(struct config *config);
int set_configurations(char *line, struct config *config);
int check_if_file_move(char **files, struct config config, int file_count);
int move_files(char *file_full_path, char *file_name, char *directory_type, char *owner);
int daemon_process();
int clean_arrays(int count, char **array);
void remove_chars(char *s, char c);