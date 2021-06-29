#include <stdio.h>
#include <signal.h>
#include <uci.h>
#include <iotp_device.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include "ibm.h"

#define UCI_CONFIG_FILE "ibm"

int main(void)
{
    struct ubus_context *ctx = NULL;
    IoTPConfig *config = NULL;
    IoTPDevice *device = NULL;

    rc = IOTPRC_SUCCESS;

    if (init_logger() != 0)
        return 1;

    write_to_log(LOG_INFO, "Program started");
    if (connect_to_ibm(&config, &device) != 0)
            goto cleanup_1;
    if (subscribe_to_ibm_commands(&device) != 0)
            goto cleanup_2;

    if (ubus_method(&ctx, &device) != 0)
            goto cleanup_3;

    cleanup_3:
        ubus_free(ctx);
    cleanup_2:
        disconnect_from_ibm(&config, &device);
    cleanup_1:
        cleanup();
	    return 0;
}

static void board_cb(struct ubus_request *req, int type, struct blob_attr *msg) 
{
    char *memories = (char*)req->priv;

    struct blob_attr *tb[__INFO_MAX];
    struct blob_attr *memory[__MEMORY_MAX];
    static struct blob_buf buf;
    char *json = NULL;

    blobmsg_buf_init(&buf);
    blobmsg_parse(info_policy, __INFO_MAX, tb, blob_data(msg), blob_len(msg));

    if (!tb[MEMORY_DATA]) {
            write_to_log(LOG_ERROR, "No memory data received");
            rc=-1;
            return;
    }
    
    blobmsg_parse(memory_policy, __MEMORY_MAX, memory,
                    blobmsg_data(tb[MEMORY_DATA]), blobmsg_data_len(tb[MEMORY_DATA]));

    blobmsg_add_u64(&buf, "total memory:", blobmsg_get_u64(memory[TOTAL_MEMORY]));
    blobmsg_add_u64(&buf, "free memory:", blobmsg_get_u64(memory[FREE_MEMORY]));
    blobmsg_add_u64(&buf, "shared memory:", blobmsg_get_u64(memory[SHARED_MEMORY]));
    blobmsg_add_u64(&buf, "buffered memory:", blobmsg_get_u64(memory[BUFFERED_MEMORY]));
    blobmsg_add_u64(&buf, "available memory:", blobmsg_get_u64(memory[AVAILABLE_MEMORY]));
    blobmsg_add_u64(&buf, "cached memory:", blobmsg_get_u64(memory[CACHED_MEMORY]));

    json = blobmsg_format_json(buf.head, true);

    strcpy(memories, json);
    free(json);
}

int ubus_method(struct ubus_context **ctx, IoTPDevice **device)
{
    MQTTProperties *properties = (MQTTProperties *)malloc(sizeof(MQTTProperties));
    char *memories = (char *) malloc (sizeof(char *)+500);
    uint32_t id;
    
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term_proc;
    sigaction(SIGTERM, &action, NULL);

    *ctx = ubus_connect(NULL);
    if (!*ctx) {
            write_to_log(LOG_ERROR, "Failed connect to ubus");
            return -1;
    }

    if (ubus_lookup_id(*ctx, "system", &id))
            write_to_log(LOG_ERROR, "cannot request memory info from procd");
    
    while(deamonize) {
            ubus_invoke(*ctx, id, "info", NULL, board_cb, memories, 3000);
            
            if (send_data(*device, memories, properties) != 0 || !deamonize) {
                    free(memories);
                    return -1;
            }
            
            sleep(5);
    }

    free(memories);

    return 0;
}

int send_data(IoTPDevice *device, char *data, MQTTProperties *properties)
{
    rc = IoTPDevice_sendEvent(device, "status", data, "json", QoS0, properties);

    if (rc != IOTPRC_SUCCESS) {
            write_to_log(LOG_ERROR, "send data failed");
            return rc;
    }

    return rc;
}


int subscribe_to_ibm_commands(IoTPDevice **device)
{
    char *message;
    if ( rc == IOTPRC_SUCCESS ) {
            rc = IoTPDevice_subscribeToCommands(*device, "+", "+");
            return 0;
    } else {
            sprintf(message, "Failed to set command handler or subscribe to commands. rc=%d reason:%s\n", rc, IOTPRC_toString(rc));
            write_to_log(LOG_ERROR, message);
            return 1;
    }
}


int connect_to_ibm(IoTPConfig **config, IoTPDevice **device)
{
    rc = IOTPRC_SUCCESS;
    char orgId[100], typeId[100], deviceId[100], auth[100];
    char *message;

    if (load_config(&orgId, &typeId, &deviceId, &auth) != 0) {
            write_to_log(LOG_ERROR, "Bad config file");
            return 1;
    }

    rc = IoTPConfig_create(config, NULL);
    IoTPConfig_setProperty(*config, "identity.orgId", orgId);
    IoTPConfig_setProperty(*config, "identity.typeId", typeId);
    IoTPConfig_setProperty(*config, "identity.deviceId", deviceId);
    IoTPConfig_setProperty(*config, "auth.token", auth);
    
    if (rc == IOTPRC_SUCCESS) {
            rc = IoTPDevice_create(device, *config);
            if (rc == IOTPRC_SUCCESS) {
                    rc = IoTPDevice_connect(*device);
            }
    }
    if (rc != IOTPRC_SUCCESS) {
            sprintf(message, "Failed to create device client: rc=%d reason:%s\n", rc, IOTPRC_toString(rc));
            write_to_log(LOG_ERROR, message);
            return rc;
    }
}

int load_config(char *orgId, char *typeId, char *deviceId, char *auth)
{
    struct uci_context *ctx = NULL;
    struct uci_package *p = NULL;
    struct uci_element *e = NULL;
    const char *Value = NULL;

    ctx = uci_alloc_context();			
    if (UCI_OK != uci_load(ctx, UCI_CONFIG_FILE, &p)) {
            write_to_log(LOG_ERROR, "Error with uci load");
            uci_free_context(ctx);
            return -1;
    }
    uci_foreach_element(&p->sections, e) {
            struct uci_section *s = uci_to_section(e);
            if (NULL != (Value = uci_lookup_option_string(ctx, s, "orgId"))) {
                    strcpy(orgId, Value);
            }
            if (NULL != (Value = uci_lookup_option_string(ctx, s, "typeId"))) {
                    strcpy(typeId, Value);
            }
            if (NULL != (Value = uci_lookup_option_string(ctx, s, "deviceId"))) {
                    strcpy(deviceId, Value);
            }
            if (NULL != (Value = uci_lookup_option_string(ctx, s, "auth"))) {
                    strcpy(auth, Value);
            }
    }
    uci_unload(ctx, p);
    uci_free_context(ctx);
    ctx = NULL;

    return 0;
}

void term_proc(int sigterm) 
{
	deamonize = 0;
}

int disconnect_from_ibm(IoTPConfig **config, IoTPDevice **device)
{
    char *message;
    rc = IOTPRC_SUCCESS;
    rc = IoTPDevice_disconnect(*device);

    if (rc == IOTPRC_SUCCESS) {
            rc = IoTPDevice_destroy(*device);
            if (rc == IOTPRC_SUCCESS) {
                    rc = IoTPConfig_clear(*config);
        }
    }
    if (rc != IOTPRC_SUCCESS) {
            sprintf(message, "Failed to disconnect from device client: rc=%d reason:%s\n", rc, IOTPRC_toString(rc));
            write_to_log(LOG_ERROR, message);
            return 1;
    }
}
