#ifndef LIB_PUUSH_INTERNAL_H__
#define LIB_PUUSH_INTERNAL_H__

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <openssl/md5.h>
#include "libpuush.h"

/* don't use ANYTHING in this file unless you REALLY know what you're doing */

/* internal types */
struct puush_form_data {
    char *name;
    char *value;
    int is_file;
};

/* curl necesities/helpers */
#define curl_formadd_field(httppost, name, value)        curl_formadd(&httppost, &httppost ## _last, CURLFORM_COPYNAME, name, CURLFORM_COPYCONTENTS, value, CURLFORM_END)
#define curl_formadd_file(httppost, name, file_name, fd) curl_formadd(&httppost, &httppost ## _last, CURLFORM_COPYNAME, name, CURLFORM_STREAM, fd, CURLFORM_FILENAME, file_name, CURLFORM_CONTENTSLENGTH, puush_file_size(fd), CURLFORM_END)
#define curl_formcreate(httppost)                        struct curl_httppost *httppost = NULL; struct curl_httppost *httppost ## _last = NULL

size_t puush_concatenator(char *data, size_t size, size_t amount, void *raw_total);

/* parsing response data from puush */
#define puush_extract_string(data) strdup(strsep(&data, ","))
#define puush_extract_long(data)   strtol(strsep(&data, ","), NULL, 10)
#define puush_extract_skip(data)   strsep(&data, ",")
#define puush_extract_int(data)    (int)puush_extract_long(data);

/* file utils */
char *puush_md5_file(FILE *fd);  // rewinds file
off_t puush_file_size(FILE *fd); // rewinds file

/* internal methods */
char *puush_raw_request(struct puush *this, const char *url, struct curl_httppost *post_data);
int puush_auth_generic(struct puush *this, struct curl_httppost *post_data);

#endif
