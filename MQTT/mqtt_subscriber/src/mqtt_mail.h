#ifndef MQTT_MAIL_H
#define MQTT_MAIL_H

#include <curl/curl.h>
#include "mqtt_config.h"
#include <string.h>

struct upload_status {
    const char *readptr;
    size_t sizeleft;
};

extern int send_mail(char *value, struct topic topics);

#endif