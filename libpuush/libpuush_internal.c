#include "libpuush_internal.h"

size_t puush_concatenator(char *data, size_t size, size_t amount, void *raw_total) {
    /* initialize total if NULL */
    char *total = *((char **)raw_total); // raw_total points to a char pointer
    if (total == NULL) {
        total = malloc(1 * sizeof(char));
        if (total == NULL) return -1;
        total[0] = '\0';
    }
    size_t total_length = strlen(total);

    /* resize total to new length */
    char *buf = realloc(total, total_length + (size * amount) + (1 * sizeof(char)));
    if (buf == NULL) {
        free(total);
        total = NULL;
        return -1;
    }
    total = buf;

    /* add data to end of total */
    buf = strndup(data, size * amount); // reuse buf
    if (buf == NULL) {
        free(total);
        return -1;
    }
    memcpy(total + total_length, buf, (size * amount));
    free(buf);

    /* finish up */
    total[total_length + (size * amount)] = '\0';
    *((char **)raw_total) = total;
    return size * amount;
}

#define PUUSH_MD5_BUF_SIZE 1024
char *puush_md5_file(FILE *fd) {
    /* setup */
    char *hash = malloc(((MD5_DIGEST_LENGTH * 2) + 1) * sizeof(char)); // hexadecimal string + terminator
    hash[MD5_DIGEST_LENGTH * 2] = '\0'; // set early so that we can...
    if (fd == NULL) return hash; // ...return blank string if null file
    char *buf = malloc(PUUSH_MD5_BUF_SIZE * sizeof(char));
    int bytes_read;
    MD5_CTX raw_md5; // can't be a pointer?
    unsigned char digest[MD5_DIGEST_LENGTH];

    /* run the hash */
    MD5_Init(&raw_md5);
    while ((bytes_read = fread(buf, sizeof(char), PUUSH_MD5_BUF_SIZE, fd)) != 0) {
        MD5_Update(&raw_md5, buf, bytes_read);
    }
    MD5_Final(digest, &raw_md5);
    int i;
    for (i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        sprintf(hash + (i * (2 * sizeof(char))), "%02x", (unsigned int)digest[i]);
    }

    /* finish up */
    free(buf);
    rewind(fd);
    return hash;
}

off_t puush_file_size(FILE *fd) {
    fseek(fd, 0, SEEK_END);
    off_t size = ftell(fd);
    rewind(fd);
    return size;
}

char *puush_raw_request(struct puush *this, const char *url, struct curl_httppost *post_data) {
    /* create the response body */
    char *recieved_data = NULL;

    /* setup curl */
    curl_easy_setopt(this->curl_handle, CURLOPT_URL,           url);
    curl_easy_setopt(this->curl_handle, CURLOPT_HTTPPOST,      post_data);
    curl_easy_setopt(this->curl_handle, CURLOPT_WRITEDATA,     &recieved_data);
    curl_easy_setopt(this->curl_handle, CURLOPT_WRITEFUNCTION, puush_concatenator);
    curl_easy_setopt(this->curl_handle, CURLOPT_READFUNCTION,  NULL);
#ifdef PUUSH_VERBOSE
    curl_easy_setopt(this->curl_handle, CURLOPT_VERBOSE, 1);
#endif

    /* run request */
    CURLcode res = curl_easy_perform(this->curl_handle);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        free(recieved_data);
        recieved_data = NULL;
    }

    /* finish up */
    return recieved_data;
}

int puush_auth_generic(struct puush *this, struct curl_httppost *post_data) {
    /* preparations */
    if (this == NULL) return PUUSHE_NOT_INITIALZED;
    free(this->api_key);
    this->api_key = NULL; // wipe out the old stuff first

    /* send the request */
    char *raw = puush_raw_request(this, PUUSH_EXPAND_ENDPOINT("/api/auth"), post_data);
    if (raw == NULL) return PUUSHE_FAILED_REQUEST;
#ifdef PUUSH_VERBOSE
    fprintf(stderr, "puush_auth got: [%s]\n", raw);
#endif

    /* test for errors */
    if (strcmp(raw, "-1") == 0) {
        free(raw);
        return PUUSHE_INVALID_API_KEY;
    }

    /* extract data */
    char *data = raw;
    this->is_premium  = puush_extract_int(data);
    this->api_key     = puush_extract_string(data);
    /*  expiry_date  */ puush_extract_skip(data); // expiry_date is unused
    this->quota_used  = puush_extract_long(data);

    /* finish up */
    free(raw);
    return PUUSHE_SUCCESS;
}

int puush_object_free_single(struct puush_object *object) {
    free(object->url); object->url = NULL;
    free(object->id); object->id = NULL;
    free(object->filename); object->filename = NULL;
    free(object);
    return 0;
}

int puush_object_iterate_remaining(struct puush_object *object) {
    object->remaining += 1;
    return 0;
}

// re-implement strsep to split on multi-char tokens
// otherwise, should work exactly the same, maybe slower
char *puush_strsep(char **stringp, const char *delim) {
    if (*stringp == NULL) return NULL;
    char *begin = *stringp;
    char *end = strstr(*stringp, delim);
    if (end == NULL) {
        *stringp = NULL;
    } else {
        *end = '\0';
        *stringp = end + strlen(delim);
    }
    return begin;
}
