#ifndef LIB_PUUSH_INTERNAL_H__
#define LIB_PUUSH_INTERNAL_H__

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "libpuush.h"

/* don't use ANYTHING in this file unless you REALLY know what you're doing */

/* internal types */
struct puush_form_data {
    char *name;
    char *value;
    int is_file;
};

/* curl necesities/helpers */
#define curl_formadd_field(httppost, name, value) curl_formadd(&httppost, &httppost ## _last, CURLFORM_COPYNAME, name, CURLFORM_COPYCONTENTS, value, CURLFORM_END)
#define curl_formadd_file(httppost, name, filename) curl_formadd(&httppost, &httppost ## _last, CURLFORM_COPYNAME, name, CURLFORM_FILE, filename, CURLFORM_END)
#define curl_formcreate(httppost) struct curl_httppost *httppost = NULL; struct curl_httppost *httppost ## _last = NULL
size_t puush_concatenator(char *data, size_t size, size_t amount, void *raw_total);

/* parsing response data from puush */
#define puush_extract_string(data) strdup(strsep(&data, ","))
#define puush_extract_long(data) strtol(strsep(&data, ","), NULL, 10)
#define puush_extract_int(data) (int)puush_extract_long(data);

/* internal methods */
char *puush_raw_request(struct puush *this, const char *url, struct curl_httppost *post_data);

#endif
