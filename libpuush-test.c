#include <libpuush.h>
#include <stdio.h>

int print_info(struct puush_object *object) {
    // ugh ctime
    char upload_time_s[25];
    struct tm *upload_time = localtime(&(object->timestamp));
    strftime(upload_time_s, 25, "%c", upload_time);
    printf("%s [%s] (%s) - %d views\n", object->url, object->filename, upload_time_s, object->views);
    return 0;
}

int main() {
    struct puush *puush = puush_init();
    if (puush_auth_password(puush, "yeahright@comeonman.com", "wowzers")) {
        printf("auth_p failed\n");
        puush_free(puush);
        return 1;
    }

    struct puush_object *upload = puush_upload_path(puush, "image.png");
    if (upload == NULL) {
        printf("something messed up\n");
        puush_free(puush);
        return 1;
    }
    printf("uploaded to [%s] with id '%s'\n", upload->url, upload->id);

    int puushes_to_get = 10;
    struct puush_object *history = puush_history(puush, puushes_to_get, 0);
    printf("wanted [%d] history objects, but really got [%d]\n", puushes_to_get, history->remaining + 1);
    printf("listing the last %d puushes...\n", history->remaining + 1);
    puush_object_each(history, print_info);
    puush_object_free(history);

    puush_delete(puush, upload->id);
    puush_object_free(upload);
    printf("deleted!\n");

    puush_free(puush);

    return 0;
}
