#ifndef LIB_PUUSH_H__
#define LIB_PUUSH_H__

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

/* error codes */

enum puush_error_code {
    PUUSHE_FAILED_REQUEST,      /* 1 raw request failed (check errno)   */
    PUUSHE_INVALID_CREDENTIALS, /* 2 your user/pass combo was incorrect */
    PUUSHE_INVALID_API_KEY      /* 3 your api key was invalid           */
};

/* types */
struct puush {
    CURL *curl_handle;
    int is_premium;
    char *api_key;
    int expiry_date; // no purpose
    long quota_used; // bytes
};

/* static defines */
#define PUUSH_BASE_URL "http://puush.me"
#define PUUSH_EXPAND_ENDPOINT(endpoint) PUUSH_BASE_URL endpoint

/* methods */
struct puush *puush_init();
int puush_auth(struct puush *this, char *api_key);
void puush_free(struct puush *this);

#endif
