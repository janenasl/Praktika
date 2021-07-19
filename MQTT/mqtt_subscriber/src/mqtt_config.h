#include <string.h>
#include <stdlib.h>

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

struct events
{
    char topic[200];
    char type[10];
    char opt_value[200];
    int dec_operator;
    int str_operator;
    char user_email[150];
};

static int count_sections(struct uci_package *p, int selection);
extern int get_parameters(struct topic **topics, struct settings **settings, struct events **events);
static int set_settings(char *option_name, char *option_value, struct settings **settings);
extern int set_topics(char *opt_name, char *opt_value, int k, struct topic **topics);
static int load_config(struct uci_package **p);