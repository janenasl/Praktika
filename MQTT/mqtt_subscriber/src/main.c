#include <signal.h>
#include "mqtt_sub.h"

volatile sig_atomic_t deamonize = 1;
void term_proc(int sigterm)
{
        deamonize = 0;
}

int main(void)
{
    struct mosquitto *mosq = NULL;
    struct settings *settings = NULL;
    struct topic *topics = NULL;
    int tc;

    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term_proc;
    sigaction(SIGTERM, &action, NULL);

    tc = iniciate_config_read(&topics, &settings);
    if (tc == -1)
            return tc;

    if (tc == -2)
            goto cleanup_3;

    if (connect_to_broker(&mosq, &settings, &topics, tc) != 0) {
            goto cleanup_2;
    }
    if (subscribe_topics(&mosq, &topics, tc) != 0) {
            goto cleanup_1;
    }

    while (deamonize) {
            if (mosquitto_loop(mosq, -1, 1) != MOSQ_ERR_SUCCESS) {
                    fprintf(stderr, "Communications between the client and broker stopped");
                    goto cleanup_2;
            }
    }

    cleanup_1:
            mosquitto_disconnect(mosq);
    cleanup_2:
            mosquitto_destroy(mosq);
            mosquitto_lib_cleanup();
    cleanup_3:
            if (topics != NULL) 
                    free(topics);
            if (settings != NULL)
                    free(settings);
            if (topics->event != NULL)
                    free(topics->event);
            return 0;
}