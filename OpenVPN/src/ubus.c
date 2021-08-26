#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include "main.h"
#include "ubus.h"
#include "helpers.h"

static int counter_add(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

static int pid_get(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

static int status_get(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

/*
 * Global variable which will be to store value
 * which will be passed over ubus. * 
 * */
static int count;

/*
 *The enumaration array is used to specifie how much arguments will our 
 * methods accepted. Also to say trough which index which argument will 
 * be reacheble.
 * 
 *  */

enum {
	COUNTER_VALUE,
	__COUNTER_MAX
};

/*
 * This policy structure is used to determine the type of the arguments
 * that can be passed to some kind of method. 
 * This structure will be used in another structure applying this policy
 * to our selected method.
 * */

static const struct blobmsg_policy counter_policy[] = {
	[COUNTER_VALUE] = { .name = "value", .type = BLOBMSG_TYPE_INT32 },
};

/*
 * This structure is used to register available methods.
 * If a method accepts arguments, the method should have a policy.
 * */

static const struct ubus_method counter_methods[] = {
	UBUS_METHOD("add", counter_add, counter_policy),
    UBUS_METHOD_NOARG("status", status_get),
    UBUS_METHOD_NOARG("pid", pid_get)
};

/*
 * This structure is used to define the type of our object with methods.
 * */
 
static struct ubus_object_type counter_object_type =
	UBUS_OBJECT_TYPE("counter", counter_methods);

/*
 * This structure is used to register our program as an ubus object
 * with our methods and other neccessary data. 
 * */

static struct ubus_object counter_object = {
	.name = "counter",
	.type = &counter_object_type,
	.methods = counter_methods,
	.n_methods = ARRAY_SIZE(counter_methods),
};


/*
 * This method is used as a callback function to return the PID
 * First we send command (pid) to server
 * When we receive response we parse message for using it with ubus
 * */
static int pid_get(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_buf b = {};
    char *received_message;
    char *message;
    int len;

    recv_all(); //!< receive unnecessary messages (example - new client connect)
	blob_buf_init(&b, 0);

    message = malloc_message("pid\n", 5, &len);
    if (message == NULL) goto cleanup_1;

    send_all(message, &len);
    received_message = recv_all();      
    received_message = parse_pid(received_message);

    if (received_message == NULL) goto cleanup_2;

	blobmsg_add_string(&b, "PID", received_message);
	ubus_send_reply(ctx, req, b.head);

    cleanup_2:
            free(message);
    cleanup_1:
            blob_buf_free(&b);
            
	return 0;
}

/*
 * This method is used as a callback function to return the status
 * First we send command (status) to server
 * When we receive response we parse message for using it with ubus
 * */
static int status_get(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_buf b = {};

    struct Clients *clients = NULL;
    char *received_message;
    char *message;
    int len;
    int clients_count = 0;
    int client_number = 0;

    recv_all(); //!< receive unnecessary messages (example - new client connect)

    message = malloc_message("status\n", 8, &len);

    if (message == NULL) return 1;

    send_all(message, &len);
    received_message = recv_all();

    if (parse_status(received_message, &clients_count, &clients) != 0)
            goto cleanup_2;

    for(int i = 0; i < clients_count; i++) {
        	blob_buf_init(&b, 0);
            blobmsg_add_string(&b, "Common name", clients[i].name);
            blobmsg_add_string(&b, "Real address", clients[i].address);
            blobmsg_add_string(&b, "Bytes received", clients[i].bytes_received);
            blobmsg_add_string(&b, "Bytes sent", clients[i].bytes_sent);
            blobmsg_add_string(&b, "Connected since", clients[i].connected);
            ubus_send_reply(ctx, req, b.head);
            blob_buf_free(&b);
    }

    cleanup_2:
            free(clients);
            free(message);
    cleanup_1:
          // blob_buf_free(&b);

	return 0;
}


/*
 * This method is used to read the argument value which is passed over ubus
 * and append that value to our global variable.
 * All the arguments are neccessary.
 * */
static int counter_add(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	/*
	 * This structure is used to store the arguments which are passed
	 * through ubus.
	 * __COUNTER_MAX in this scenario is equal to 1.
	 * So this structure will hold only one variable.
	 * */
	struct blob_attr *tb[__COUNTER_MAX];
	struct blob_buf b = {};
	
	blobmsg_parse(counter_policy, __COUNTER_MAX, tb, blob_data(msg), blob_len(msg));
	
	if (!tb[COUNTER_VALUE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	/*
	 * This is the place where the value is extracted and appended to our
	 * variable.
	 * COUNTER_VALUE in this scenario is equal to 0. 0 indicates the first
	 * array element.
	 * blogmsg_get_u32 parses the value which is appended to the variable.
	 * */
	count += blobmsg_get_u32(tb[COUNTER_VALUE]);

	/*
	 * This part of the method returns a messaged through ubus.
	 * */
	blob_buf_init(&b, 0);

	blobmsg_add_u32(&b, "count", count);
	ubus_send_reply(ctx, req, b.head);
	blob_buf_free(&b);

	return 0;
}

int process_ubus()
{
	struct ubus_context *ctx;

	uloop_init();

	ctx = ubus_connect(NULL);
	if (!ctx) {
            fprintf(stderr, "Failed to connect to ubus\n");
            return -1;
	}

	ubus_add_uloop(ctx);
	ubus_add_object(ctx, &counter_object);
	uloop_run();

	ubus_free(ctx);
	uloop_done();

	return 0;
}

