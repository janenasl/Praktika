#include "mqtt.h"

int main(void)
{
    struct mosquitto *mosq = NULL;
    struct settings *settings = NULL;
    struct topic *topics = NULL;
    int tc = 0;

    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term_proc;
    sigaction(SIGTERM, &action, NULL);

    tc = get_topics_and_settings(&topics, &settings);
    if (tc <= 1) {
            fprintf(stderr, "no topics to subscribe\n");
            return 1;
    }

    if (connect_to_broker(&mosq, &settings, &topics, tc) != 0) {
            goto cleanup_2;
    }
    if (subscribe_topics(&mosq, &topics, tc) != 0) {
            goto cleanup_1;
    }
    printf("bb\n");
    //while (deamonize) {
            if (mosquitto_loop_forever(mosq, -1, 1) != MOSQ_ERR_SUCCESS) {
                    fprintf(stderr, "Communications between the client and broker stopped");
                    goto cleanup_2;
            }
    //}
    printf("baigiu\n");
    cleanup_1:
            mosquitto_disconnect(mosq);
    cleanup_2:
            free(topics);
            free(settings);
            mosquitto_destroy(mosq);
            mosquitto_lib_cleanup();
            return 0;
}

int add_message_to_db(char *topic, char *payload)
{
printf("%\n", __TIMESTAMP__);
    sqlite3 *db;
    char *message = NULL;
    char *zErrMsg = 0;
    int rc = 0;
printf("%\n", __TIMESTAMP__);
    rc = sqlite3_open("/etc/sub_messages.db", &db);
    if (rc != SQLITE_OK) {
            fprintf(stderr, "could not open database file: %s\n", sqlite3_errmsg(db));
            return -1;
    }
    message = (char *) malloc (sizeof(char) * (strlen(topic) + strlen(payload)+100));
    if (message == NULL) {
            return -1;
    }
    printf("%\n", __TIMESTAMP__);
    sprintf(message, "INSERT INTO messages VALUES ('%s', '%s', '%s');", topic, payload, __TIMESTAMP__);
    rc = sqlite3_exec(db, message, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
    }
printf("%\n", __TIMESTAMP__);
    sqlite3_close(db);
    free(message);
    return rc;
}