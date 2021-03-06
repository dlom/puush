#include "libpuush.h"
#include "libpuush_internal.h"

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
    free(this->api_key); this->api_key = NULL;
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

struct puush_object *puush_upload(struct puush *this, char *name, FILE *fd) {
    /* pre-upload checks */
    if (puush_reauth(this)) return NULL; // bad api_key/not initialized
    if (fd == NULL) return NULL; // bad file

    /* create form fields */
    char *hash = puush_md5_file(fd);
    curl_formcreate(fields);
    curl_formadd_field(fields, "k", this->api_key);
    curl_formadd_field(fields, "c", hash);
    curl_formadd_field(fields, "z", "poop"); // yup
    curl_formadd_file(fields, "f", name, fd);
    free(hash);

    /* send request */
    char *raw = puush_raw_request(this, PUUSH_EXPAND_ENDPOINT("/api/up"), fields);
    curl_formfree(fields);
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
    struct puush_object *upload = malloc(sizeof(struct puush_object));
    char *data = raw;
    /* always 0      */ puush_extract_skip(data);
    upload->url       = puush_extract_string(data);
    upload->id        = puush_extract_string(data);
    upload->filename  = strdup(name);
    upload->timestamp = time(NULL);
    upload->views     = 0;
    upload->remaining = 0;
    upload->next      = NULL;
    this->quota_used  = puush_extract_long(data);

    /* finish up */
    free(raw);
    return upload;
}

struct puush_object *puush_upload_path(struct puush *this, char *file_path) {
    FILE *fd = fopen(file_path, "rb");
    struct puush_object *upload = puush_upload(this, basename(file_path), fd);
    if (fd != NULL) fclose(fd);
    return upload;
}

struct puush_object *puush_history(struct puush *this, int amount, int offset) {
    /* check if authed */
    if (puush_reauth(this)) return NULL; // bad api_key/not initialized

    /* create form fields */
    puush_create_string_from_number(amount);
    puush_create_string_from_number(offset);
    curl_formcreate(fields);
    curl_formadd_field(fields, "k", this->api_key);
    curl_formadd_field(fields, "l", amount_s);
    curl_formadd_field(fields, "o", offset_s);

    /* send request */
    char *raw = puush_raw_request(this, PUUSH_EXPAND_ENDPOINT("/api/hist"), fields);
    curl_formfree(fields);
    if (raw == NULL) return NULL;
#ifdef PUUSH_VERBOSE
    fprintf(stderr, "puush_history got: [%s]\n", raw);
#endif

    /* error check */
    if (raw[0] == '-') {
        free(raw);
        return NULL;
    }

    /* complex extraction */
    struct puush_object *prev = NULL;
    struct puush_object *head = malloc(sizeof(struct puush_object));
    char *data = raw;
    puush_extract_next_line(data); // skip first line (status only)
    while (strlen(data) != 0) {
        /* linked list setup */
        struct puush_object *next;
        if (prev == NULL) {
            next = head;
        } else {
            next = malloc(sizeof(struct puush_object));
        }

        /* extract individual puush */
        next->id         = puush_extract_string(data);
        char *timestring = puush_extract_string(data);
        next->timestamp  = puush_convert_time(timestring);
        next->url        = puush_extract_string(data);
        next->filename   = puush_extract_string(data);
        next->views      = puush_extract_int(data);
        next->remaining  = 0;
        next->next       = NULL; // temporary so we can loop over
        puush_object_each(head, puush_object_iterate_remaining); // set the "remaining" variable

        /* finish this node */
        if (prev != NULL) prev->next = next;
        prev = next;
        free(timestring);
        puush_extract_next_line(data); // next line
    }

    /* finish the list */
    prev->next = NULL;
    head->remaining -= 1; // idiosyncrasy

    /* finish up */
    free(raw);
    return head;
}

int puush_delete(struct puush *this, char *id) {
    /* check if authed */
    int auth_result = puush_reauth(this);
    if (auth_result) return auth_result;

    curl_formcreate(fields);
    curl_formadd_field(fields, "k", this->api_key);
    curl_formadd_field(fields, "i", id);
    curl_formadd_field(fields, "z", "poop"); // really
    curl_formadd_field(fields, "l", "0");
    curl_formadd_field(fields, "o", "0");

    /* send request */
    char *raw = puush_raw_request(this, PUUSH_EXPAND_ENDPOINT("/api/del"), fields);
    curl_formfree(fields);
    if (raw == NULL) return PUUSHE_FAILED_REQUEST;
#ifdef PUUSH_VERBOSE
    fprintf(stderr, "puush_delete got: [%s]\n", raw);
#endif

    /* error check */
    if (raw[0] == '-') {
        free(raw);
        return PUUSHE_UNKNOWN_ERROR; // TODO real error handling everywhere
    }

    /* finish up */
    free(raw);
    return PUUSHE_SUCCESS;
}

int puush_object_each(struct puush_object *head, puush_object_each_callback callback) {
    if (head == NULL) return 0;
    struct puush_object *changing_head = head;
    while (changing_head != NULL) {
        struct puush_object *next = changing_head->next;
        int result = callback(changing_head);
        if (result) return result;
        changing_head = next;
    }
    return 0;
}

void puush_object_free(struct puush_object *object) {
    if (object == NULL) return;
    puush_object_each(object, puush_object_free_single);
}
