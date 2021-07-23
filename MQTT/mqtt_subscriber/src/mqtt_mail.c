#include "mqtt_mail.h"

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
  struct upload_status *upload_ctx = (struct upload_status *)userp;
  const char *data;

  size_t bytes_to_copy = upload_ctx->sizeleft < size*nmemb ? upload_ctx->sizeleft : size*nmemb;
  bytes_to_copy = upload_ctx->sizeleft < 10 ? upload_ctx->sizeleft : 10;

  if(size*nmemb < 1)
        return 0;

  if(upload_ctx->sizeleft) {
        memcpy((char*)ptr, (char*)upload_ctx->readptr, bytes_to_copy);
        upload_ctx->readptr+=bytes_to_copy;
        upload_ctx->sizeleft-=bytes_to_copy;
        return bytes_to_copy;
  }

  return 0;
}

extern int send_mail(char *value, struct topic topics)
{
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    struct upload_status upload_ctx = { 0 };
    char *smtp_server_url;
    
    char payload_template[] =
    "To: %s\r\n"
    "From: %s\r\n"
    "Subject: MQTT Subscriber\r\n"
    "\r\n"
    "An event from topic '%s' was observed. Event value: %s, publisher value: %s."
    "\r\n\r\n";

    size_t payload_text_len = strlen(payload_template) +
                    strlen(topics.event->user_email) +
                    strlen(topics.event->sender_email) +
                    strlen(topics.event->topic) +
                    strlen(topics.event->opt_value) + strlen(value)+1;

    char* payload_text = malloc(payload_text_len);
    memset(payload_text, 0, payload_text_len);
    sprintf(payload_text, payload_template, topics.event->user_email, topics.event->sender_email,
                topics.event->topic, topics.event->opt_value, value);


    smtp_server_url = (char *) malloc(sizeof(char) * (strlen("smtp://") + strlen(topics.event->smtp_ip) + strlen(topics.event->smtp_port))+5);
    sprintf(smtp_server_url, "smtp://%s:%s", topics.event->smtp_ip, topics.event->smtp_port);

    curl = curl_easy_init();
    if(curl) {
            upload_ctx.readptr = payload_text;
            upload_ctx.sizeleft = (long)strlen(payload_text);

            recipients = curl_slist_append(recipients, topics.event->user_email);

            curl_easy_setopt(curl, CURLOPT_USERNAME, topics.event->username);
            curl_easy_setopt(curl, CURLOPT_PASSWORD, topics.event->password);
            curl_easy_setopt(curl, CURLOPT_URL, smtp_server_url);

            if(topics.event->secure == 1) {
                    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
                    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
                    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);                    
            }

            curl_easy_setopt(curl, CURLOPT_MAIL_FROM, topics.event->sender_email);
            curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
            curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);

            curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    
            res = curl_easy_perform(curl);

            if(res != CURLE_OK) {
                    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                    return -1;
            }
    
            printf("paėjo\n");
            curl_slist_free_all(recipients);
            curl_easy_cleanup(curl);
    }
    free(payload_text);
    free(smtp_server_url);
}