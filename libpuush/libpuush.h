#ifndef LIB_PUUSH_H__
#define LIB_PUUSH_H__

#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <curl/curl.h>

/* error codes */
enum puush_error_code {
    PUUSHE_SUCCESS = 0,         // 0 success
    PUUSHE_FAILED_REQUEST,      // 1 raw request failed (check errno)
    PUUSHE_INVALID_CREDENTIALS, // 2 your user/pass combo was incorrect
    PUUSHE_INVALID_API_KEY,     // 3 your api key was invalid
    PUUSHE_NOT_AUTHED,          // 4 you haven't run puush_auth(_p) yet
    PUUSHE_NOT_INITIALZED,      // 5 you haven't run puush_init yet
};

/* types */
struct puush {
    CURL *curl_handle;
    int is_premium;
    char *api_key;
    long quota_used; // bytes
};

struct puush_upload {
    char *url;
    char *id;
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
struct puush_upload *puush_upload(struct puush *this, char *name, FILE *fd); // the name of the file on the server
struct puush_upload *puush_upload_path(struct puush *this, char *file_path);
void puush_upload_free(struct puush_upload *upload);
#endif
