#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

#include <string.h>
#include <stdlib.h>
#include <uci.h>

#define CONFIG_FILE "/etc/config/mqtt_subs"

struct topic
{
        char name[200];
        int qos;
        int ec; // event count
        struct events *event;
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
        char smtp_ip[30];
        char smtp_port[10];
        int credentials;
        int secure;
        char username[70];
        char password[70];
        char sender_email[200];
};


extern int iniciate_config_read(struct topic **topics, struct settings **settings);
static int load_config(struct uci_package **p, char *config_name);
static void set_settings(char *option_name, char *option_value, struct settings **settings);
static void set_topics(char *opt_name, char *opt_value, int k, struct topic **topics);
static void set_sender_settings(struct events *temp_event);
static int set_events(int tc, struct topic **topics);
static int read_config(struct topic **topics, struct settings **settings);
static int count_topics();

#endif