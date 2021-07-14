#include "mqtt_config.h"

static int load_config(struct uci_package **p)
{
    struct uci_context *ctx = NULL;
    ctx = uci_alloc_context();
    if (ctx == NULL) {
            //write_to_log(LOG_ERROR, "uci context memory allocation failed");
            return -2;
    }
    		
    if (UCI_OK != uci_load(ctx, CONFIG_FILE, p)) {
            //write_to_log(LOG_ERROR, "uci load failed");
            uci_free_context(ctx);
            return -1;
    }

    return 0;
}

static int set_settings(char *option_name, char *option_value, struct settings **settings)
{
    if (strcmp(option_name, "remote_address") == 0) {
            strcpy((*settings)->remote_address, option_value);
    } else if (strcmp(option_name, "remote_port") == 0) {
            strcpy((*settings)->remote_port, option_value);
    } else if (strcmp(option_name, "remote_username") == 0) {
            strcpy((*settings)->username, option_value);
    } else if (strcmp(option_name, "remote_password") == 0) {
            strcpy((*settings)->password, option_value);
    } else if (strcmp(option_name, "ssl_enabled") == 0) {
            (*settings)->ssl_enabled = atoi(option_value);
    }

    if ((*settings)->ssl_enabled == 0) 
            return 0;

    if (strcmp(option_name, "tls_type") == 0) {
            strcpy((*settings)->tls_type, option_value);
    }

    if (strcmp((*settings)->tls_type, "psk") == 0) {
            if (strcmp(option_name, "psk") == 0) {
                    strcpy((*settings)->psk, option_value);
            } else {
                    strcpy((*settings)->identity, option_value);
            }
    } else if (strcmp((*settings)->tls_type, "cert") == 0) {
            if (strcmp(option_name, "ca_file") == 0) {
                    strcpy((*settings)->ca_cert, option_value);
            } else if (strcmp(option_name, "cert_file") == 0) {
                    strcpy((*settings)->cert_file, option_value); 
            } else {
                    strcpy((*settings)->key_file, option_value);
            }
    }
}


extern int get_topics_and_settings(struct topic **topics, struct settings **settings)
{
    struct uci_package *p = NULL;
    struct uci_element *i, *j; 
    int tc = 0;
    int k = 0;

    load_config(&p);

    tc = count_topics(p);
    if (tc > 0) {
            *topics = (struct topic *) malloc (sizeof(struct topic) * tc);
    } else {
            return -1;
    }

    *settings = (struct settings *) malloc (sizeof(struct settings));

    uci_foreach_element(&p->sections, i) {
            struct uci_section *s = uci_to_section(i);

            uci_foreach_element(&s->options, j) {
                    struct uci_option *option = uci_to_option(j);
                    if (strcmp(option->e.name, "topic") == 0) {
                            strcpy((*topics)[k].name, option->v.string);
                    }  else if (strcmp(option->e.name, "qos") == 0) {
                            (*topics)[k].qos = atoi(option->v.string);
                    }
                    if (strcmp(s->type, "mqtt_sub") == 0) {
                            set_settings(option->e.name, option->v.string, settings);
                    }
            }
            if (strcmp(s->type, "mqtt_topic") == 0) {
                    k++;
            }
    }
    return k;
}

static int count_topics(struct uci_package *p)
{
    int count = 0;
    struct uci_element *i;
    uci_foreach_element(&p->sections, i) {
            count++;
    }

    return count-2; //because config always has mqtt_settings and mqtt_msg and we need only topics
}
