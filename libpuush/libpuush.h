#ifndef LIB_PUUSH_H__
#define LIB_PUUSH_H__

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

/* error codes */

enum puush_error_code {
    PUUSHE_SUCCESS = 0,         /* 0 success                            */
    PUUSHE_FAILED_REQUEST,      /* 1 raw request failed (check errno)   */
    PUUSHE_INVALID_CREDENTIALS, /* 2 your user/pass combo was incorrect */
    PUUSHE_INVALID_API_KEY,     /* 3 your api key was invalid           */
    PUUSHE_NOT_AUTHED           /* 4 you haven't run puush_auth(_p) yet */
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
void puush_free(struct puush *this);
int puush_auth(struct puush *this, char *api_key);
int puush_auth_p(struct puush *this, char *email, char *password);
int puush_reauth(struct puush *this);

#endif
