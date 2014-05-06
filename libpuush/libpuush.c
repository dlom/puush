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
    curl_formcreate(fields);
    curl_formadd_field(fields, "k", api_key);
    curl_formadd_field(fields, "z", "poop"); // you can't make this stuff up
    int status = puush_auth_generic(this, fields);
    curl_formfree(fields);
    return status;
}

int puush_auth_p(struct puush *this, char *email, char *password) {
    curl_formcreate(fields);
    curl_formadd_field(fields, "e", email);
    curl_formadd_field(fields, "p", password);
    curl_formadd_field(fields, "z", "poop"); // really
    int status = puush_auth_generic(this, fields);
    curl_formfree(fields);
    return status;
}

int puush_reauth(struct puush *this) {
    if (this->api_key == NULL) return PUUSHE_NOT_AUTHED;
    return puush_auth(this, this->api_key);
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
    if (puush_auth_p(puush, "email@example.com", "password")) {
        printf("fail!\n");
    } else {
        printf("Your API key is [%s]\n", puush->api_key);
    }
    puush_free(puush);

    return 0;
}
