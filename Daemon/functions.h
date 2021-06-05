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
int countFiles(char *path);
char *findOwner();
int countSymbols(char *p);
int removeChars(char *s, char c);
int logs(char *text);
int checkFiles(char *path);
int checkNewFiles(char *path, char **oldfiles, int file_count);
int checkingFileMove(char **files, struct config config, int file_count);
char *find_file_name(char *str);
char *get_filename_ext(char *filename);
int check_extensions(char *types, char *extension);
int check_if_dir_exists(char *dir);
int movingFiles(char *file_full_path, char *file_name, char *sprintf_text);
int mkdir_p(const char *path);
static void daemon_process();