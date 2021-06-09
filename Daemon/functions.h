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
char *get_file_name(char *str);
char *get_filename_ext(char *filename);
char *find_owner();
int count_files(char *path);
int count_symbols(char *p);
int check_not_movable_files(char *path);
int check_new_files(char *path, char **oldfiles, int file_count);
int check_extensions(char *types, char *extension);
int check_if_dir_exists(char *dir);
int mkdir_p(const char *path);
int daemon_process();
void check_if_file_move(char **files, struct config config, int file_count);
void move_files(char *file_full_path, char *file_name, char *sprintf_text);
void remove_chars(char *s, char c);
void main_process(struct config config);
void open_file();