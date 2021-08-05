#ifndef MQTT_EVENTS_H
#define MQTT_EVENTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mqtt_config.h"
#include <json-c/json.h>

extern int process_events(char *topic, char *payload, struct topic_node *topics);
static int process_dec_val(char *value, int dec_operator, char *event_value);
static int process_str_val(char *value, int dec_operator, char *event_value);

#endif