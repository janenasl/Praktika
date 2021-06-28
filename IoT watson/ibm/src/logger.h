#define LOG_INFO "INFO: "
#define LOG_WARN "WARNING: "
#define LOG_ERROR "ERROR: "

int init_logger();
void cleanup();
void write_to_log(char *level, char *message);
