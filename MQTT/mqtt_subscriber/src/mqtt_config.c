#include "mqtt_config.h"

struct uci_package *p = NULL;
/**
* load config
*/
extern int iniciate_config_read(struct topic **topics, struct settings **settings)
{
    int tc = 0;

    load_config(&p, CONFIG_FILE);

    tc = read_config(topics, settings);
    if (tc < 1) {
            fprintf(stderr, "no topics to subscribe\n");
            return -1;
    } else if (tc == -3 || tc == -2) {
            fprintf(stderr, "settings or topics struct malloc failed\n");
            return -2;
    }

    if (set_events(tc, topics) != 0) {
            fprintf(stderr,"Failed to set events\n");
            return tc;
    }

    return tc;
}
/**
* load config from file
*/
static int load_config(struct uci_package **p, char *config_name)
{
    struct uci_context *ctx = NULL;
    ctx = uci_alloc_context();
    if (ctx == NULL) {
            fprintf(stderr, "uci context memory allocation failed");
            return -2;
    }
    		
    if (UCI_OK != uci_load(ctx, config_name, p)) {
            fprintf(stderr, "uci load failed");
            uci_free_context(ctx);
            return -1;
    }

    return 0;
}

/**
* set user defined settings in web to struct settings
*/
static void set_settings(char *option_name, char *option_value, struct settings **settings)
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
            return ;

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
/**
* set user defined topics in web to struct topics
*/
static void set_topics(char *opt_name, char *opt_value, int k, struct topic **topics)
{
    if (strcmp(opt_name, "topic") == 0) {
        strcpy((*topics)[k].name, opt_value);
    }  else if (strcmp(opt_name, "qos") == 0) {
        (*topics)[k].qos = atoi(opt_value);
    }
    (*topics)[k].ec = 0;
}

static void set_sender_settings(struct events *temp_event)
{
    struct uci_package *pck = NULL;
    struct uci_element *i, *j;

    load_config(&pck, "user_groups");

    uci_foreach_element(&pck->sections, i) {
            struct uci_section *s = uci_to_section(i);

            uci_foreach_element(&s->options, j) {
                    struct uci_option *option = uci_to_option(j);
                    if (strcmp(option->e.name, "smtp_ip") == 0) {
                            strcpy(temp_event->smtp_ip, option->v.string);
                    } else if (strcmp(option->e.name, "smtp_port") == 0) {
                            strcpy(temp_event->smtp_port, option->v.string);
                    } else if (strcmp(option->e.name, "credentials") == 0) {
                            temp_event->credentials = atoi(option->v.string);
                    } else if (strcmp(option->e.name, "secure") == 0) {
                            temp_event->secure = atoi(option->v.string);
                    } else if (strcmp(option->e.name, "username") == 0) {
                            strcpy(temp_event->username, option->v.string);
                    } else if (strcmp(option->e.name, "password") == 0) {
                            strcpy(temp_event->password, option->v.string);
                    } else if (strcmp(option->e.name, "senderemail") == 0) {
                            strcpy(temp_event->sender_email, option->v.string);
                    }
            }
    }
}

/**
* set user defined events in web to struct event
*/
static int set_events(int tc, struct topic **topics)
{
    struct uci_element *i, *j;
    struct events temp_event;
    int ec = 0;

    uci_foreach_element(&p->sections, i) {
            struct uci_section *s = uci_to_section(i);

            uci_foreach_element(&s->options, j) {
                    struct uci_option *option = uci_to_option(j);

                    if (strcmp(s->type, "mqtt_event") != 0)
                            break;

                    if (strcmp(option->e.name, "topic") == 0) {
                            strcpy(temp_event.topic, option->v.string);
                    } else if (strcmp(option->e.name, "type") == 0) {
                            strcpy(temp_event.type, option->v.string);
                    } else if (strcmp(option->e.name, "value") == 0) {
                            strcpy(temp_event.opt_value, option->v.string);
                    } else if (strcmp(option->e.name, "dec_operator") == 0) {
                            temp_event.dec_operator = atoi(option->v.string);
                    } else if (strcmp(option->e.name, "str_operator") == 0) {
                            temp_event.str_operator = atoi(option->v.string);
                    } else if (strcmp(option->e.name, "user_email") == 0) {
                            strcpy(temp_event.user_email, option->v.string);
                    } else if (strcmp(option->e.name, "sender") == 0) {
                            set_sender_settings(&temp_event);
                    }
            }
            for (int i = 0; i<tc; i++) {
                    ec = (*topics)[i].ec;
                    if(strcmp(temp_event.topic, (*topics)[i].name) == 0) {
                            if (ec == 0) {
                                    (*topics)[i].event = (struct events *) malloc(sizeof(struct events));
                                    if ((*topics)[i].event == NULL)
                                            return -1;
                            } else {
                                    (*topics)[i].event = (struct events *) realloc((*topics)[i].event, sizeof(struct events) * (ec+1));
                                    if ((*topics)[i].event == NULL)
                                            return -1;
                            }
                            (*topics)[i].event[ec] = temp_event;
                            (*topics)[i].ec += 1;
                    }
            }
            strcpy(temp_event.topic, "");
    }

    // testavimui:
    // printf("%s\n", (*topics)[0].event[0].user_email);
    // printf("%s\n", (*topics)[1].event[0].user_email);
    // printf("%s\n", (*topics)[0].event[1].user_email);
    // printf("%s\n", (*topics)[1].event[1].user_email);
    return 0;
}

/**
* loop through config sections and set topics and settings
*/
static int read_config(struct topic **topics, struct settings **settings)
{
    struct uci_element *i, *j; 
    int topic_count_overall = 0;
    int topic_count = 0;

    topic_count_overall = count_topics(p);
    if (topic_count_overall < 1) 
            return 0;

    *topics = (struct topic *) malloc(sizeof(struct topic) * topic_count_overall);
    if (*topics == NULL)
            return -2;

    *settings = (struct settings *) malloc(sizeof(struct settings));
    if (*settings == NULL)
            return -3;

    uci_foreach_element(&p->sections, i) {
            struct uci_section *s = uci_to_section(i);

            uci_foreach_element(&s->options, j) {
                    struct uci_option *option = uci_to_option(j);

                    if (strcmp(s->type, "mqtt_topic") == 0)
                            set_topics(option->e.name, option->v.string, topic_count, topics);

                    if (strcmp(s->type, "mqtt_sub") == 0)
                            set_settings(option->e.name, option->v.string, settings);
            }
            if (strcmp(s->type, "mqtt_topic") == 0)
                    topic_count++;
    }

    return topic_count_overall;
}

/**
* count user defined topics
*/
static int count_topics()
{
    int count = 0;
    struct uci_element *i;

    uci_foreach_element(&p->sections, i) {
            struct uci_section *s = uci_to_section(i);

            if(strcmp(s->type, "mqtt_topic") == 0)
                    count++;
    }

    return count;
}
