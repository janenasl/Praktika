#include "main.h"

int main(void)
{
    struct mosquitto *mosq = NULL;
    struct settings *settings = NULL;
    struct topic *topics = NULL;
    struct events *events = NULL;
    int tc = 0;

    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term_proc;
    sigaction(SIGTERM, &action, NULL);

    tc = get_parameters(&topics, &settings, &events);
    if (tc < 1) {
            fprintf(stderr, "no topics to subscribe\n");
            goto cleanup_3;
    } else if (tc == -3 || tc == -2) {
            fprintf(stderr, "settings or topics struct malloc failed\n");
            return 1;
    }

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
            free(topics);
            free(settings);
            return 0;
}

int check_if_table_exists(sqlite3 *db)
{
    int rc = 0;
    sqlite3_stmt *stmt = NULL;
    char *sql = NULL;

    sql = (char *) malloc(sizeof(char) * 100);

    sprintf(sql, "SELECT COUNT(TYPE) FROM sqlite_master WHERE TYPE='table' AND NAME='%s';", DB_TABLE);

    rc = sqlite3_prepare16_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
            fprintf(stderr, "error making sql statement");
            goto cleanup;
    }

    sqlite3_step(stmt);
    rc = sqlite3_column_int(stmt);

    cleanup:
            sqlite3_finalize(stmt);
            free(sql);
            return rc;
}

int create_table(sqlite3 *db)
{
    sqlite3_stmt *stmt = NULL;
    int rc = 0;
    char *sql = NULL;
    char *err_msg = NULL;

    sql = (char *) malloc(sizeof(char) * 100);

    sprintf(sql, "CREATE TABLE '%s' (topic varchar(255), payload varchar(255), time varchar(100));", DB_TABLE);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg)

    if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s", err_msg);
            sqlite3_free(err_msg);
            
            return rc;
    }
    
    return rc;
}
/**
* adds messages to database
*/
int add_message_to_db(char *topic, char *payload)
{
    sqlite3 *db;
    char *message = NULL;
    char *zErrMsg = 0;
    int rc = 0;

    rc = sqlite3_open(DB_FILE, &db);
    if (rc != SQLITE_OK) {
            fprintf(stderr, "could not open database file: %s\n", sqlite3_errmsg(db));
            return -1;
    }

    message = (char *) malloc (sizeof(char) * (strlen(topic) + strlen(payload)+100));
    if (message == NULL) {
            return -1;
    }

    sprintf(message, "INSERT INTO %s VALUES ('%s', '%s', '%s');", DB_TABLE, topic, payload, __TIMESTAMP__);
    rc = sqlite3_exec(db, message, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
    }

    sqlite3_close(db);
    if (message != NULL) {
            free(message);
            message = NULL;
    }
    return rc;
}