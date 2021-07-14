#include "mqtt_sub.h"

extern int subscribe_topics(struct mosquitto **mosq, struct topic **topics, int tc)
{
    for (int i = 0; i<tc; i++) {
            if (mosquitto_subscribe(*mosq, NULL, (*topics)[i].name, (*topics)[i].qos) != MOSQ_ERR_SUCCESS) {
                    fprintf(stderr, "Can't subscribe to topic: %s\n", (*topics)[i].name);
                    return -1;
            }
    }
    return 0;
}

extern int connect_to_broker(struct mosquitto **mosq, struct settings **settings, struct topic **topics, int tc)
{
    int rc;
    
    mosquitto_lib_init();
    
    *mosq = mosquitto_new(NULL, true, NULL);
    if (*mosq == NULL) {
            fprintf(stderr, "Can't create mosquitto broker");
            return -1;
    }
    
    mosquitto_connect_callback_set(*mosq, on_connect_cb);
    mosquitto_message_callback_set(*mosq, on_message_cb);

    if (strlen((*settings)->username) != 0 || strlen((*settings)->password) != 0)
            mosquitto_username_pw_set(*mosq, (*settings)->username, (*settings)->password);
    

    if ((*settings)->ssl_enabled = 1 && strcmp((*settings)->tls_type, "cert") == 0) {
            if (strlen((*settings)->ca_cert) != 0 || strlen((*settings)->key_file) != 0 || strlen((*settings)->cert_file) != 0) {
                    mosquitto_tls_set(*mosq, (*settings)->ca_cert, NULL, (*settings)->cert_file, (*settings)->key_file, NULL);
            }
    }

    if ((*settings)->ssl_enabled = 1 && strcmp((*settings)->tls_type, "psk") == 0) {
            if (strlen((*settings)->psk) != 0 || strlen((*settings)->identity) != 0) {
                    mosquitto_tls_psk_set(*mosq, (*settings)->psk, (*settings)->identity, NULL);
            }
    }
    
    rc = mosquitto_connect(*mosq, (*settings)->remote_address, atoi((*settings)->remote_port), 60);
    if (rc) {
            fprintf(stdout, "Could not connect to broker, return code: %d \n", rc);
            return -1;
    }
    return 0;
}

static void on_connect_cb(struct mosquitto *mosq, void *obj, int rc) 
{
    if(rc) {
            fprintf(stderr, "Error with result code: %d\n", rc);
            exit(1);
    }
}

static void on_message_cb(struct mosquitto *mosq, void *obj, struct mosquitto_message *msg)
{
    if (msg->payloadlen != 0) {
        printf("siunčiu!");
            add_message_to_db(msg->topic, msg->payload);
            printf("išsiunčiau\n");
    }
}