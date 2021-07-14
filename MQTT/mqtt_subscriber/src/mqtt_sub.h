#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <mosquitto.h> 
#include "mqtt_config.h"


extern int connect_to_broker(struct mosquitto **mosq, struct settings **settings, struct topic **topics, int tc);
extern int subscribe_topics(struct mosquitto **mosq, struct topic **topics, int tc);
static void on_message_cb(struct mosquitto *mosq, void *obj, struct mosquitto_message *msg);
static void on_connect_cb(struct mosquitto *mosq, void *obj, int rc);