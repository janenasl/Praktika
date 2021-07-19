#include <sqlite3.h>

#include "mqtt_sub.h"
#define DB_FILE "/etc/sub_messages.db"
#define DB_TABLE "messages"

volatile sig_atomic_t deamonize = 1;
int add_message_to_db(char *topic, char *payload);
void term_proc(int sigterm)
{
        deamonize = 0;
}