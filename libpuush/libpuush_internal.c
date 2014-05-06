#include "libpuush_internal.h"

size_t puush_concatenator(char *data, size_t size, size_t amount, void *raw_total) {
    /* initialize total if NULL */
    char *total = *((char **)raw_total);
    if (total == NULL) {
        total = malloc(sizeof(char));
        if (total == NULL) return 0;
        total[0] = '\0';
    }
    size_t total_length = strlen(total);

    /* resize total to new length */
    char *buf = realloc(total, total_length + (size * amount) + 1);
    if (buf == NULL) {
        free(total);
        return 0;
    }
    total = buf;

    /* add data to end of total */
    buf = strndup(data, size * amount); // reuse buf
    if (buf == NULL) {
        free(total);
        return 0;
    }
    memcpy(total + total_length, buf, (size * amount));
    free(buf);

    /* finish up */
    total[total_length + (size * amount)] = '\0';
    *((char **)raw_total) = total;
    return size * amount;
}

char *puush_raw_request(struct puush *this, const char *url, struct curl_httppost *post_data) {
    char *recieved_data = NULL;

    /* setup curl */
    curl_easy_setopt(this->curl_handle, CURLOPT_URL,           url);
    curl_easy_setopt(this->curl_handle, CURLOPT_HTTPPOST,      post_data);
    curl_easy_setopt(this->curl_handle, CURLOPT_WRITEDATA,    &recieved_data);
    curl_easy_setopt(this->curl_handle, CURLOPT_WRITEFUNCTION, puush_concatenator);
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
    this->expiry_date = puush_extract_int(data);
    this->quota_used  = puush_extract_long(data);

    /* finish up */
    free(raw);
    return PUUSHE_SUCCESS;
}
