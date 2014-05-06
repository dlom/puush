#include "libpuush.h"
#include "libpuush_internal.h"

/* for main() */
#include <stdio.h>

struct puush *puush_init() {
    curl_global_init(CURL_GLOBAL_ALL); // yolo
    CURL *curl = curl_easy_init();
    if (curl == NULL) return NULL;
    struct puush *this = malloc(sizeof(struct puush));
    this->curl_handle = curl;
    this->api_key = NULL;
    return this;
}

int puush_auth(struct puush *this, char *api_key) {
    /* prepare the request */
    curl_formcreate(fields);
    curl_formadd_field(fields, "k", api_key);
    curl_formadd_field(fields, "z", "poop"); // you can't make this stuff up

    /* send the request */
    char *raw = puush_raw_request(this, PUUSH_EXPAND_ENDPOINT("/api/auth"), fields);
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
    free(raw);

    return 0;
}

void puush_free(struct puush *this) {
    curl_easy_cleanup(this->curl_handle);
    this->curl_handle = NULL;
    curl_global_cleanup(); // yolo
    free(this->api_key);
    free(this);
}

int main(int argc, char *argv[]) {
    struct puush *puush = puush_init();
    if (puush_auth(puush, "apikeyhere")) {
        printf("fail!\n");
    } else {
        printf("Your API key is [%s]\n", puush->api_key);
    }
    puush_free(puush);

    return 0;
}
