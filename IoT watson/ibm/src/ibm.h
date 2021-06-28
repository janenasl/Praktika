#include "logger.h"
int rc = 0;

volatile sig_atomic_t deamonize = 1;

enum {
        TOTAL_MEMORY,
        FREE_MEMORY,
        SHARED_MEMORY,
        BUFFERED_MEMORY,
        AVAILABLE_MEMORY,
        CACHED_MEMORY,
        __MEMORY_MAX,
};

enum {
        MEMORY_DATA,
        __INFO_MAX,
};

static const struct blobmsg_policy memory_policy[__MEMORY_MAX] = {
        [TOTAL_MEMORY] = { .name = "total", .type = BLOBMSG_TYPE_INT64 },
        [FREE_MEMORY] = { .name = "free", .type = BLOBMSG_TYPE_INT64 },
        [SHARED_MEMORY] = { .name = "shared", .type = BLOBMSG_TYPE_INT64 },
        [BUFFERED_MEMORY] = { .name = "buffered", .type = BLOBMSG_TYPE_INT64 },
        [AVAILABLE_MEMORY] = { .name = "available", .type = BLOBMSG_TYPE_INT64 },
        [CACHED_MEMORY] = { .name = "cached", .type = BLOBMSG_TYPE_INT64 },
};

static const struct blobmsg_policy info_policy[__INFO_MAX] = {
        [MEMORY_DATA] = { .name = "memory", .type = BLOBMSG_TYPE_TABLE },
};



int load_config(char *orgId, char *typeId, char *deviceId, char *auth);
int disconnect_from_ibm(IoTPConfig **config, IoTPDevice **device);
int send_data(IoTPDevice *device, char *data, MQTTProperties *properties);
int connect_to_ibm(IoTPConfig **config, IoTPDevice **device);
int ubus_method(struct ubus_context **ctx, IoTPDevice **device);
void term_proc(int sigterm);
int subscribe_to_ibm_commands(IoTPDevice **device);
static void board_cb(struct ubus_request *req, int type, struct blob_attr *msg);
