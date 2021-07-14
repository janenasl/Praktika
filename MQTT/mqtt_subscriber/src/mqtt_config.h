#include <uci.h>

#define CONFIG_FILE "/etc/config/mqtt_subs"

struct topic
{
        char name[200];
        int qos;
};

struct settings 
{
        char remote_address[50];
        char remote_port[30];
        char username[50];
        char password[50];
        char psk[200];
        char identity[200];
        int ssl_enabled;
        char tls_type[10];
        char cert_file[200];
        char key_file[200];
        char ca_cert[200];
};

static int count_topics(struct uci_package *p);
extern int get_topics_and_settings(struct topic **topics, struct settings **settings);
static int set_settings(char *option_name, char *option_value, struct settings **settings);
static int load_config(struct uci_package **p);