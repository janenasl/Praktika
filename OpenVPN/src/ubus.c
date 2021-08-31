#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <unistd.h>
#include "main.h"
#include "ubus.h"
#include "helpers.h"

static int log_info(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

static int pkcs_get(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

static int pkcs_index_get(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

static int status_get(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

static int set_state(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

static int set_verb(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

static int set_hold(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

static int set_mute(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

static int set_auth_retry(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

static int set_kill(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

static int set_signal(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

static void end_ubus();

static struct uloop_timeout my_event_timer;
struct ubus_context *ctx;
/**
 * The enumaration array is used to specifie how much arguments will our 
 * methods accepted. Also to say trough which index which argument will 
 * be reacheble.
 */

enum {
	COUNTER_VALUE,
	__COUNTER_MAX
};

/**
 * This policy structure is used to determine the type of the arguments
 * that can be passed to some kind of method. 
 * This structure will be used in another structure applying this policy
 * to our selected method.
 */

static const struct blobmsg_policy log_policy[] = {
	[COUNTER_VALUE] = { .name = "value", .type = BLOBMSG_TYPE_INT32 },
};

static const struct blobmsg_policy hold_policy[] = {
	[COUNTER_VALUE] = { .name = "hold", .type = BLOBMSG_TYPE_STRING },
};

static const struct blobmsg_policy mute_policy[] = {
	[COUNTER_VALUE] = { .name = "mute", .type = BLOBMSG_TYPE_INT32 },
};

static const struct blobmsg_policy auth_policy[] = {
	[COUNTER_VALUE] = { .name = "auth", .type = BLOBMSG_TYPE_STRING },
};

static const struct blobmsg_policy kill_policy[] = {
	[COUNTER_VALUE] = { .name = "kill", .type = BLOBMSG_TYPE_STRING },
};

static const struct blobmsg_policy signal_policy[] = {
	[COUNTER_VALUE] = { .name = "signal", .type = BLOBMSG_TYPE_STRING },
};

static const struct blobmsg_policy state_policy[] = {
	[COUNTER_VALUE] = { .name = "state", .type = BLOBMSG_TYPE_INT32 },
};

static const struct blobmsg_policy verb_policy[] = {
	[COUNTER_VALUE] = { .name = "verb", .type = BLOBMSG_TYPE_INT32 },
};

static const struct blobmsg_policy pkcs_policy[] = {
	[COUNTER_VALUE] = { .name = "index", .type = BLOBMSG_TYPE_INT32 },
};

/**
 * This structure is used to register available methods.
 * If a method accepts arguments, the method should have a policy.
 */

static const struct ubus_method telnet_methods[] = {
    #if WORKING
    UBUS_METHOD("log", log_info, log_policy),
    UBUS_METHOD("hold", set_hold, hold_policy),
    UBUS_METHOD("mute", set_mute, mute_policy),
    UBUS_METHOD("auth", set_auth_retry, auth_policy),
    UBUS_METHOD("signal", set_signal, signal_policy),
    UBUS_METHOD("state", set_state, state_policy),
    UBUS_METHOD("verb", set_verb, state_policy),
    #endif
    UBUS_METHOD("dis_client", set_kill, kill_policy),
    #if ENABLE_PKCS
    UBUS_METHOD("pkcs_index", pkcs_index_get, pkcs_policy),
    UBUS_METHOD_NOARG("pkcs", pkcs_get),
    #endif
    UBUS_METHOD_NOARG("clients", status_get)
};

/**
 * This structure is used to define the type of our object with methods.
 */
 
static struct ubus_object_type telnet_object_type =
	UBUS_OBJECT_TYPE("telnet", telnet_methods);

/**
 * This structure is used to register our program as an ubus object
 * with our methods and other neccessary data. 
 */

static struct ubus_object telnet_object = {
	.name = "telnet",
	.type = &telnet_object_type,
	.methods = telnet_methods,
	.n_methods = ARRAY_SIZE(telnet_methods),
};

/**
 * This method is used as a callback function to return the certificates count
 * First we send command (pkcs-id-count) to server
 * When we receive response we parse message to use it with ubus
 * @return 0 - on success; 1 - allocation problems
 */
static int pkcs_get(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_buf b = {};
    char *received_message = NULL;
    char *send_message = NULL;
    int len = 0;

    recv_all(); //!< receive unnecessary messages (example - new client connect)

    send_message = malloc_message("pkcs11-id-count\n", &len);
    if (send_message == NULL) return 1;

    send_all(send_message, &len);
    received_message = recv_all();

    if(received_message == NULL) goto cleanup_1;

    parse_message(received_message, ':');

    blob_buf_init(&b, 0);
    blobmsg_add_string(&b, "pkcs", received_message);
    ubus_send_reply(ctx, req, b.head);
    blob_buf_free(&b);

    //free(received_message);
    cleanup_1:
            free(send_message);

	return 0;
}

/**
 * This method is used as a callback function to return the status
 * First we send command (status) to server
 * When we receive response we parse message to use it with ubus
 * @return 0 - on success; 1 - allocation problems
 */
static int status_get(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_buf b = {};
    recv_all(); //!< receive unnecessary messages (example - new client connect)
    int gs = 1; //!< gathering status return code

    if (clients == NULL)
            gs = gather_status();

    while(clients != NULL && strlen(clients->name) > 0) {
            gs = 0;
        	blob_buf_init(&b, 0);
            blobmsg_add_string(&b, "Common name", clients->name);
            blobmsg_add_string(&b, "Real address", clients->address);
            blobmsg_add_string(&b, "Bytes received", clients->bytes_received);
            blobmsg_add_string(&b, "Bytes sent", clients->bytes_sent);
            blobmsg_add_string(&b, "Connected since", clients->connected);
            ubus_send_reply(ctx, req, b.head);
            blob_buf_free(&b);
            clients = clients->next;
    }

    if (gs == 1) {
            blob_buf_init(&b, 0);
            blobmsg_add_string(&b, "information", "No clients are connected this moment");
            ubus_send_reply(ctx, req, b.head);
            blob_buf_free(&b);
            return 0;
    }

	return 0;
}
/**
 * This method is used as a callback function to return the log information
 * First we send command (log) to server
 * When we receive response we parse message to use it with ubus
 * @return 0 - on success; 1 - allocation problems
 */
static int log_info(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__COUNTER_MAX];
	struct blob_buf b = {};

    struct Log_messages *logs = NULL;
    char *received_message = NULL;
    char *send_message = NULL;
    int len = 0;

    char command[10]; //!< command that will be send to server
    int number = 0;

    recv_all(); //!< receive unnecessary messages (example - new client connect)
	
	blobmsg_parse(log_policy, __COUNTER_MAX, tb, blob_data(msg), blob_len(msg));
	
	if (!tb[COUNTER_VALUE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	number = blobmsg_get_u32(tb[COUNTER_VALUE]);
    if (number < 0) goto cleanup;
    (number == 0) ? strcpy(command, "log all\n") : sprintf(command, "log %d\n", number);
 
    send_message = malloc_message(command, &len);
    if (send_message == NULL) goto cleanup;

    send_all(send_message, &len);
    received_message = recv_all();  
    if (received_message == NULL) goto cleanup;

    parse_logs(received_message, &number, &logs);

	blob_buf_init(&b, 0);

    for(int i = 0; i < number; i++)
            blobmsg_add_string(&b, "Log message", logs[i].message);
    
	ubus_send_reply(ctx, req, b.head);

    free(logs);
    free(send_message);
    free(received_message);
    cleanup:
	        blob_buf_free(&b);

	return 0;
}

/**
 * This method is used as a callback function to return the state information
 * First we send command (state) to server
 * When we receive response we parse message to use it with ubus
 * note: state acts the same as logging function
 * @return 0 - on success; 1 - allocation problems
 */
static int set_state(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__COUNTER_MAX];
	struct blob_buf b = {};

    struct Log_messages *logs = NULL;
    char *received_message = NULL;
    char *send_message = NULL;
    int len = 0;

    char command[10]; //!< command that will be send to server
    int number = 0;

    recv_all(); //!< receive unnecessary messages (example - new client connect)
	
	blobmsg_parse(state_policy, __COUNTER_MAX, tb, blob_data(msg), blob_len(msg));
	
	if (!tb[COUNTER_VALUE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	number = blobmsg_get_u32(tb[COUNTER_VALUE]);
    if (number < 0) goto cleanup;
    (number == 0) ? strcpy(command, "state all\n") : sprintf(command, "state %d\n", number);
 
    send_message = malloc_message(command, &len);
    if (send_message == NULL) goto cleanup;

    send_all(send_message, &len);
    received_message = recv_all();  

    if (received_message == NULL) goto cleanup;

    parse_logs(received_message, &number, &logs);

	blob_buf_init(&b, 0);

    for(int i = 0; i < number; i++)
            blobmsg_add_string(&b, "state", logs[i].message);
    
	ubus_send_reply(ctx, req, b.head);

    free(logs);
    free(send_message);
    free(received_message);
    cleanup:
	        blob_buf_free(&b);

	return 0;
}

/**
 * This method is used as a callback function to return verb information
 * First we send command (verb) to server
 * When we receive response we parse message to use it with ubus
 * @return 0 - on success; 1 - allocation problems
 */
static int set_verb(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__COUNTER_MAX];
	struct blob_buf b = {};

    char *received_message = NULL;
    char *send_message = NULL;
    int len = 0;

    char command[10]; //!< command that will be send to server
    int number = 0;

    recv_all(); //!< receive unnecessary messages (example - new client connect)
	
	blobmsg_parse(verb_policy, __COUNTER_MAX, tb, blob_data(msg), blob_len(msg));
	
	if (!tb[COUNTER_VALUE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	number = blobmsg_get_u32(tb[COUNTER_VALUE]);
    if (number < 0) goto cleanup;
    
    sprintf(command, "verb %d\n", number);
 
    send_message = malloc_message(command, &len);
    if (send_message == NULL) goto cleanup;

    send_all(send_message, &len);
    received_message = recv_all();  

    if (received_message == NULL) goto cleanup;

	blob_buf_init(&b, 0);
    remove_char(received_message);
    blobmsg_add_string(&b, "verb", received_message);
	ubus_send_reply(ctx, req, b.head);

    free(send_message);
    free(received_message);
    cleanup:
	        blob_buf_free(&b);

	return 0;
}

/**
 * This method is used as a callback function to return pkcs information
 * First we send command (pkcs11-id-get) to server
 * When we receive response we parse message to use it with ubus
 * @return 0 - on success; 1 - allocation problems
 */
static int pkcs_index_get(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__COUNTER_MAX];
	struct blob_buf b = {};

    char *received_message = NULL;
    char *send_message = NULL;
    int len = 0;

    char command[10]; //!< command that will be send to server
    int number = 0;

    recv_all(); //!< receive unnecessary messages (example - new client connect)
	
	blobmsg_parse(pkcs_policy, __COUNTER_MAX, tb, blob_data(msg), blob_len(msg));
	
	if (!tb[COUNTER_VALUE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	number = blobmsg_get_u32(tb[COUNTER_VALUE]);
    if (number < 0) goto cleanup;
    
    sprintf(command, "pkcs11-id-get %d\n", number);
 
    send_message = malloc_message(command, &len);
    if (send_message == NULL) goto cleanup;

    send_all(send_message, &len);
    received_message = recv_all();  

    if (received_message == NULL) goto cleanup;

	blob_buf_init(&b, 0);
    remove_char(received_message);
    blobmsg_add_string(&b, "pkcs", received_message);
	ubus_send_reply(ctx, req, b.head);

    free(send_message);
    free(received_message);
    cleanup:
	        blob_buf_free(&b);

	return 0;
}

/**
 * This method is used as a callback function to return the hold information
 * First we send command (hold) to server
 * When we receive response we parse message to use it with ubus
 * @return 0 - on success; 1 - allocation problems
 */
static int set_hold(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__COUNTER_MAX];
	struct blob_buf b = {};

    char *received_message = NULL;
    char *send_message = NULL;
    int len = 0;

    char command[20]; //!< command that will be send to server
    char argument[10];

    recv_all(); //!< receive unnecessary messages (example - new client connect)
	blobmsg_parse(hold_policy, __COUNTER_MAX, tb, blob_data(msg), blob_len(msg));
	
	if (!tb[COUNTER_VALUE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	strcpy(argument, blobmsg_get_string(tb[COUNTER_VALUE]));
    sprintf(command, "hold %s\n", argument);
 
    send_message = malloc_message(command, &len);
    if (send_message == NULL) goto cleanup;

    send_all(send_message, &len);
    received_message = recv_all();  

    if (received_message == NULL) goto cleanup;

    remove_char(received_message);
    blob_buf_init(&b, 0);
    blobmsg_add_string(&b, "hold", received_message);
	ubus_send_reply(ctx, req, b.head);

    free(send_message);
    free(received_message);
    cleanup:
	        blob_buf_free(&b);

	return 0;
}




/**
 * This method is used as a callback function to return mute information
 * First we send command (mute) to server
 * When we receive response we parse message to use it with ubus
 * @return 0 - on success; 1 - allocation problems
 */
static int set_mute(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__COUNTER_MAX];
	struct blob_buf b = {};

    char *received_message = NULL;
    char *send_message = NULL;
    int len = 0;

    char command[10]; //!< command that will be send to server
    int number = 0;

    recv_all(); //!< receive unnecessary messages (example - new client connect)
	
	blobmsg_parse(mute_policy, __COUNTER_MAX, tb, blob_data(msg), blob_len(msg));
	
	if (!tb[COUNTER_VALUE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	number = blobmsg_get_u32(tb[COUNTER_VALUE]);
    if (number < 0) goto cleanup;
    (number == 0) ? strcpy(command, "mute\n") : sprintf(command, "mute %d\n", number);
 
    send_message = malloc_message(command, &len);
    if (send_message == NULL) goto cleanup;

    send_all(send_message, &len);
    received_message = recv_all();  

    if (received_message == NULL) goto cleanup;

    remove_char(received_message);
    blob_buf_init(&b, 0);
    blobmsg_add_string(&b, "mute", received_message);
	ubus_send_reply(ctx, req, b.head);

    free(send_message);
    free(received_message);
    cleanup:
	        blob_buf_free(&b);

	return 0;
}

/**
 * This method is used as a callback function to return auth-retry information
 * First we send command (auth-retry) to server
 * When we receive response we parse message to use it with ubus
 * @return 0 - on success; 1 - allocation problems
 */
static int set_auth_retry(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__COUNTER_MAX];
	struct blob_buf b = {};

    char *received_message = NULL;
    char *send_message = NULL;
    int len = 0;

    char command[20]; //!< command that will be send to server
    char argument[10];

    recv_all(); //!< receive unnecessary messages (example - new client connect)
	blobmsg_parse(auth_policy, __COUNTER_MAX, tb, blob_data(msg), blob_len(msg));
	
	if (!tb[COUNTER_VALUE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	strcpy(argument, blobmsg_get_string(tb[COUNTER_VALUE]));
    sprintf(command, "auth-retry %s\n", argument);
 
    send_message = malloc_message(command, &len);
    if (send_message == NULL) goto cleanup;

    send_all(send_message, &len);
    received_message = recv_all();  

    if (received_message == NULL) goto cleanup;

    remove_char(received_message);
    blob_buf_init(&b, 0);
    blobmsg_add_string(&b, "auth", received_message);
	ubus_send_reply(ctx, req, b.head);

    free(send_message);
    free(received_message);
    cleanup:
	        blob_buf_free(&b);

	return 0;
}

/**
 * This method is used as a callback function to return kill information
 * First we send command (kill) to server
 * When we receive response we parse message to use it with ubus
 * @return 0 - on success; 1 - allocation problems
 */
static int set_kill(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__COUNTER_MAX];
	struct blob_buf b = {};

    char *received_message = NULL;
    char *send_message = NULL;
    int len = 0;

    char command[20]; //!< command that will be send to server
    char argument[10];

    recv_all(); //!< receive unnecessary messages (example - new client connect)
	blobmsg_parse(kill_policy, __COUNTER_MAX, tb, blob_data(msg), blob_len(msg));
	
	if (!tb[COUNTER_VALUE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	strcpy(argument, blobmsg_get_string(tb[COUNTER_VALUE]));
    sprintf(command, "kill %s\n", argument);
 
    send_message = malloc_message(command, &len);
    if (send_message == NULL) return 1;

    send_all(send_message, &len);
    received_message = recv_all();  

    if (received_message == NULL) goto cleanup;

    remove_char(received_message);
    blob_buf_init(&b, 0);
    blobmsg_add_string(&b, "kill", received_message);
	ubus_send_reply(ctx, req, b.head);

    free(received_message);
    blob_buf_free(&b);
    cleanup:
            free(send_message);

	return 0;
}

/**
 * This method is used as a callback function to return signal information
 * First we send command (signal) to server
 * When we receive response we parse message to use it with ubus
 * @return 0 - on success; 1 - allocation problems
 */
static int set_signal(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__COUNTER_MAX];
	struct blob_buf b = {};

    char *received_message = NULL;
    char *send_message = NULL;
    int len = 0;

    char command[20]; //!< command that will be send to server
    char argument[10];

    recv_all(); //!< receive unnecessary messages (example - new client connect)
	blobmsg_parse(signal_policy, __COUNTER_MAX, tb, blob_data(msg), blob_len(msg));
	
	if (!tb[COUNTER_VALUE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	strcpy(argument, blobmsg_get_string(tb[COUNTER_VALUE]));
    sprintf(command, "signal %s\n", argument);
 
    send_message = malloc_message(command, &len);
    if (send_message == NULL) goto cleanup;

    send_all(send_message, &len);
    received_message = recv_all();  

    if (received_message == NULL) goto cleanup;

    remove_char(received_message);
    blob_buf_init(&b, 0);
    blobmsg_add_string(&b, "signal", received_message);
	ubus_send_reply(ctx, req, b.head);

    

    free(send_message);
    free(received_message);
    cleanup:
	        blob_buf_free(&b);

	return 0;
}

void end_ubus(void)
{
	ubus_free(ctx);
	uloop_done();
    exit(1);
}
/**
 * event handler callback method.
 * checking if connection is still active
 * then gather information to fill status structure
 * if everything is okay we repeat this method every 3s
 */
static void event_handler(struct uloop_timeout *timeout)
{

    if (is_socket_alive() != 0) {
            end_ubus();
            return ;
    }

    gather_status();

    uloop_timeout_set(&my_event_timer, 3000);
}
/**
 * setting event every 1s
 */
static void set_event(void)
{
    my_event_timer.cb = event_handler;
    uloop_timeout_set(&my_event_timer, 1000);
}
/**
 * initiate ubus loop for process communication
 */
int process_ubus()
{
	uloop_init();
    set_event();

	ctx = ubus_connect(NULL);
	if (!ctx) {
            fprintf(stderr, "Failed to connect to ubus\n");
            return -1;
	}

	ubus_add_uloop(ctx);
	ubus_add_object(ctx, &telnet_object);
	uloop_run();

	return 0;
}

