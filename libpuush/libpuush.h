#ifndef LIB_PUUSH_H__
#define LIB_PUUSH_H__

#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <time.h>
#include <curl/curl.h>

/* error codes */
enum puush_error_code {
    PUUSHE_SUCCESS = 0,         // 0 success
    PUUSHE_FAILED_REQUEST,      // 1 raw request failed (check errno)
    PUUSHE_INVALID_CREDENTIALS, // 2 your user/pass combo was incorrect
    PUUSHE_INVALID_API_KEY,     // 3 your api key was invalid
    PUUSHE_NOT_AUTHED,          // 4 you haven't run puush_auth(_p) yet
    PUUSHE_NOT_INITIALZED,      // 5 you haven't run puush_init yet
    PUUSHE_BAD_DATA,            // 6 libpuush somehow sent invalid data
    PUUSHE_BAD_HASH,            // 7 libpuush somehow sent invalid checksum
    PUUSHE_UNKNOWN_ERROR,       // 8 puush gave unparseable results
};

/* types */
struct puush {
    CURL *curl_handle;
    int is_premium;
    char *api_key;
    long quota_used; // bytes
};

struct puush_object {
    char *url;
    char *id;
    char *filename;
    time_t timestamp;
    int views;
    int remaining; // length - 1
    struct puush_object *next;
};

typedef int (*puush_object_each_callback)(struct puush_object *object); // return 0 for success, anything else for failure

/* static defines */
#define PUUSH_BASE_URL "http://puush.me"
#define PUUSH_EXPAND_ENDPOINT(endpoint) (PUUSH_BASE_URL endpoint)

/* methods */
struct puush *puush_init();
void puush_free(struct puush *this);
int puush_auth(struct puush *this, char *api_key);
int puush_auth_password(struct puush *this, char *email, char *password);
int puush_reauth(struct puush *this);
struct puush_object *puush_upload(struct puush *this, char *name, FILE *fd); // the name of the file on the server
struct puush_object *puush_upload_path(struct puush *this, char *file_path);
struct puush_object *puush_history(struct puush *this, int amount, int offset); // objects returned NOT guarenteed to be same as amount
int puush_delete(struct puush *this, char *id);
int puush_object_each(struct puush_object *head, puush_object_each_callback callback);
void puush_object_free(struct puush_object *head);

#endif
