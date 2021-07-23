#include "mqtt_events.h"
#include "mqtt_mail.h"

/**
* decimal operators:
* 0 - <
* 1 - >
* 2 - <=
* 3 - >=
* 4 - ==
* 5 - !=
* string operators:
* 0 - Equal
* 1 - Not equal
*/
static int process_dec_val(char *value, int dec_operator, char *event_value)
{
    int sender_value = atoi(value);
    int event_value_converted = atoi(event_value);
    switch(dec_operator) {
        case 0:
                if(sender_value < event_value_converted)
                        return 0;
                break;
        case 1:
                if(sender_value > event_value_converted)
                        return 0;        
                break;
        case 2:
                if(sender_value <= event_value_converted)
                        return 0;        
                break;
        case 3:
                if(sender_value >= event_value_converted)
                        return 0;           
                break;
        case 4:
                if(sender_value == event_value_converted)
                        return 0;           
                break;
        case 5:
                if(sender_value != event_value_converted)
                        return 0;           
                break;
        default:
                fprintf(stderr, "decimal operator unknown\n");
    }
    return 1;
}
/**
* string operators:
* 0 - Equal
* 1 - Not equal
*/
static int process_str_val(char *value, int dec_operator, char *event_value)
{
    switch(dec_operator) {
        case 0:
                if(strcmp(value, event_value) == 0)
                        return 0;
                break;
        case 1:
                if(strcmp(value, event_value) != 0)
                        return 0;        
                break;
        default:
                fprintf(stderr, "string operator unknown\n");
    }
    return 1;
}

static char *get_value_from_jobj(struct json_object *val)
{
    enum json_type type;
    char *output = (char *) malloc(sizeof(char) * 200);
    if(output == NULL)
            return NULL;
    
    type = json_object_get_type(val);
    switch (type) {
            case json_type_null:
                    output = NULL;
                    break;
            case json_type_double:
                    sprintf(output, "%f", json_object_get_double(val));
                    break;
            case json_type_int:
                    sprintf(output, "%d", json_object_get_int(val));
                    break;
            case json_type_string:
                    output = (char *) json_object_get_string(val);
                    break;
            default:
                    output = NULL;
    }
    // https://stackoverflow.com/questions/2279379/how-to-convert-integer-to-char-in-c
    return output;
}

extern int process_events(char *topic, char *payload, struct topic *topics)
{
    struct json_object *jobj = NULL;
    char *value = NULL;
    int ec = 0;
    int rc = 1;

    jobj = json_tokener_parse(payload);
    if (jobj == NULL) {
            fprintf(stderr, "event value has to be in JSON format\n");
            return -1;
    }
    //int k = sizeof(topics)/sizeof(topics[0]);
    int k=2;

    for(int i = 0; i<k; i++) {
            if (strcmp(topic, topics[i].name) != 0 || topics[i].ec == 0)
                    continue;
            
            ec = topics[i].ec;

            json_object_object_foreach(jobj, key, val) {
                    if (key == NULL || val == NULL)
                            continue;

                    value = get_value_from_jobj(val);
                    if (value == NULL) {
                            fprintf(stderr, "JSON value parsing failed\n");
                            return -1;
                    }
                    for(int j=0; j<ec; j++) {
                            rc = 1;
                            if (strcmp(topics[i].event[j].type, "decimal") == 0)
                                    rc = process_dec_val(value, topics[i].event[j].dec_operator, topics[i].event[j].opt_value);

                            if (strcmp(topics[i].event[j].type, "string") == 0)
                                    rc = process_str_val(value, topics[i].event[j].dec_operator, topics[i].event[j].opt_value);
                                    
                            if (rc == 0)
                                    send_mail(value, topics[i]);
                    }        
                    free(value);
            }
    }

   json_object_put(jobj);

    return 0;
}