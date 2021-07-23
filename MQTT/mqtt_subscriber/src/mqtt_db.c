#include "mqtt_db.h"
/**
* checking if database table exists, if not creates one
*/
static int create_table_if_not_exist()
{
    sqlite3_stmt *stmt = NULL;
    int rc = 0;
    char *sql = NULL;
 
    sql = (char *) malloc(sizeof(char) * 100);
    if (sql == NULL) {
            fprintf(stderr, "sql malloc failed\n");
            return -1;
    }

    sprintf(sql, "SELECT COUNT(TYPE) FROM sqlite_master WHERE TYPE='table' AND NAME='%s';", DB_TABLE);

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
            fprintf(stderr, "error preparing sql statement\n");
            goto cleanup;
    }

    sqlite3_step(stmt);
    rc = sqlite3_column_int(stmt, 0);

    if (rc == SQLITE_OK) {
            rc = create_table();
            if (rc != SQLITE_OK) {
                    fprintf(stderr, "creating table failed\n");
                    goto cleanup;
            }
    }
    cleanup:
            sqlite3_finalize(stmt);
            free(sql);
            return rc;
}
static char *time_stamp()
{
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    return asctime(timeinfo);
}
/**
* adds messages to database
*/
static int create_table()
{
    int rc = 0;
    char *sql = NULL;
    char *err_msg = NULL;

    sql = (char *) malloc(sizeof(char) * 120);
    if (sql == NULL) {
            return -1;
    }

    sprintf(sql, "CREATE TABLE '%s' (topic varchar(255), payload varchar(255), time varchar(100));", DB_TABLE);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error while creating table: %s\n", err_msg);
            sqlite3_free(err_msg);
            
            return rc;
    }
    if(sql != NULL) {
            free(sql);
            sql = NULL;
    }
    return rc;
}
/**
* Open a connection to a new or existing SQLite database (creates new if not exist)
*/
static int open_db()
{
    int rc = 0;

    rc = sqlite3_open(DB_FILE, &db);
    if (rc != SQLITE_OK) {
            fprintf(stderr, "could not open database file: %s\n", sqlite3_errmsg(db));
            return -1;
    }
    return 0;
}
/**
* adds messages to database
*/
extern int add_message_to_db(char *topic, char *payload)
{
    char *message = NULL;
    char *zErrMsg = NULL;
    int rc = 0;

    rc = open_db();
    if (rc != SQLITE_OK) {
            return -1;
    }
    rc = create_table_if_not_exist();

    if (rc == 0) {
            fprintf(stdout, "database table was created\n");
    } else if (rc == 1) {
            fprintf(stdout, "database table exist\n");
    } else {
            goto cleanup;
    }

    message = (char *) malloc (sizeof(char) * (strlen(topic) + strlen(payload)+100));
    if (message == NULL) {
            rc = -1;
            goto cleanup;
    }
    
    sprintf(message, "INSERT INTO %s VALUES ('%s', '%s', '%s');", DB_TABLE, topic, payload, time_stamp());

    rc = sqlite3_exec(db, message, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error while adding message to DB: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
    }

    if (message != NULL) {
            free(message);
            message = NULL;
    }
    cleanup:
            sqlite3_close(db);
            return rc;
}