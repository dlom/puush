#ifndef LIB_PUUSH_INTERNAL_H__
#define LIB_PUUSH_INTERNAL_H__

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <openssl/md5.h>
#include <math.h>
#include "libpuush.h"

/* don't use ANYTHING in this file unless you REALLY know what you're doing */
/* BEWARE: macros that look like functions */

/* curl necesities/helpers */
#define curl_formadd_field(httppost, name, value)        curl_formadd(&httppost, &httppost ## _last, CURLFORM_COPYNAME, name, CURLFORM_COPYCONTENTS, value, CURLFORM_END)
#define curl_formadd_file(httppost, name, file_name, fd) curl_formadd(&httppost, &httppost ## _last, CURLFORM_COPYNAME, name, CURLFORM_STREAM, fd, CURLFORM_FILENAME, file_name, CURLFORM_CONTENTSLENGTH, puush_file_size(fd), CURLFORM_END)
#define curl_formcreate(httppost)                        struct curl_httppost *httppost = NULL; struct curl_httppost *httppost ## _last = NULL

size_t puush_concatenator(char *data, size_t size, size_t amount, void *raw_total);

/* parsing response data from puush */
#define puush_extract_string(data)    strdup(strsep(&data, ","))
#define puush_extract_long(data)      strtol(strsep(&data, ","), NULL, 10)
#define puush_extract_int(data)       (int)puush_extract_long(data);
#define puush_extract_skip(data)      strsep(&data, ",")
#define puush_extract_next_line(data) puush_strsep(&data, "0\n");

/* file utils */
char *puush_md5_file(FILE *fd);  // rewinds file
off_t puush_file_size(FILE *fd); // rewinds file

/* internal methods */
char *puush_raw_request(struct puush *this, const char *url, struct curl_httppost *post_data);
int puush_auth_generic(struct puush *this, struct curl_httppost *post_data);
int puush_object_free_single(struct puush_object *obj);
int puush_object_iterate_remaining(struct puush_object *object);

/* random utilities */
#define puush_num_digits(number) (number == 0 ? 1 : (int)floor(log10(number))+1) /* positive numbers only */
#define puush_create_string_from_number(number) char number ## _s[(puush_num_digits(number) + 1) * sizeof(char)]; sprintf(number ## _s, "%d", number)
char *puush_strsep(char **stringp, const char *delim); // splits on multi-char tokens

#endif
