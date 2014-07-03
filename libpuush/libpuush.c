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

void puush_free(struct puush *this) {
    if (this == NULL) return;
    curl_easy_cleanup(this->curl_handle);
    this->curl_handle = NULL;
    curl_global_cleanup(); // yolo
    free(this->api_key);
    free(this);
}

int puush_auth(struct puush *this, char *api_key) {
    curl_formcreate(fields);
    curl_formadd_field(fields, "k", api_key);
    curl_formadd_field(fields, "z", "poop"); // you can't make this stuff up

    int status = puush_auth_generic(this, fields);
    curl_formfree(fields);
    return status;
}

int puush_auth_password(struct puush *this, char *email, char *password) {
    curl_formcreate(fields);
    curl_formadd_field(fields, "e", email);
    curl_formadd_field(fields, "p", password);
    curl_formadd_field(fields, "z", "poop"); // really

    int status = puush_auth_generic(this, fields);
    curl_formfree(fields);
    return status;
}

int puush_reauth(struct puush *this) {
    if (this == NULL) return PUUSHE_NOT_INITIALZED;
    if (this->api_key == NULL) return PUUSHE_NOT_AUTHED;
    char *api_key = strdup(this->api_key); // jenk
    int status = puush_auth(this, api_key);
    free(api_key);
    return status;
}

struct puush_upload *puush_upload(struct puush *this, char *name, FILE *fd) {
    /* pre-upload checks */
    if (puush_reauth(this)) return NULL; // bad api_key/not initialized
    if (fd == NULL) return NULL; // bad file

    /* prep post data */
    char *hash = puush_md5_file(fd);
    curl_formcreate(fields);
    curl_formadd_field(fields, "k", this->api_key);
    curl_formadd_field(fields, "c", hash);
    curl_formadd_field(fields, "z", "poop"); // yup
    curl_formadd_file(fields, "f", name, fd);
    free(hash);

    /* send request */
    char *raw = puush_raw_request(this, PUUSH_EXPAND_ENDPOINT("/api/up"), fields);
    if (raw == NULL) return NULL;
#ifdef PUUSH_VERBOSE
    fprintf(stderr, "puush_upload got: [%s]\n", raw);
#endif

    /* test for errors, JUST IN CASE. */
    if (raw[0] == '-') {
        free(raw);
        return NULL;
    }

    /* extract data */
    struct puush_upload *upload = malloc(sizeof(struct puush_upload));
    char *data = raw;
    /* blank        */ puush_extract_skip(data); // always 0 for successful upload
    upload->url      = puush_extract_string(data);
    upload->id       = puush_extract_string(data);
    this->quota_used = puush_extract_long(data);

    /* finish up */
    free(raw);
    curl_formfree(fields);
    return upload;
}

struct puush_upload *puush_upload_path(struct puush *this, char *file_path) {
    FILE *fd = fopen(file_path, "rb");
    struct puush_upload *upload = puush_upload(this, basename(file_path), fd);
    if (fd != NULL) fclose(fd);
    return upload;
}

void puush_upload_free(struct puush_upload *upload) {
    if (upload == NULL) return;
    free(upload->url);
    free(upload->id);
    free(upload);
}

int main(int argc, char *argv[]) {
    struct puush *puush = puush_init();
    if (puush_auth_password(puush, "email@example.com", "try again")) {
        printf("auth_p failed\n");
        puush_free(puush);
        return 1;
    }
    if (puush_auth(puush, "get a real key")) {
        printf("auth failed\n");
        puush_free(puush);
        return 1;
    }
    if (puush_reauth(puush)) {
        printf("reauth failed\n");
        puush_free(puush);
        return 1;
    }

    struct puush_upload *upload = puush_upload_path(puush, "image.png");
    if (upload != NULL) {
        printf("go to [%s]\n", upload->url);
        puush_upload_free(upload);
    } else {
        printf("upload failed\n");
    }

    puush_free(puush);

    return 0;
}
