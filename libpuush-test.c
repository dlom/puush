#include <libpuush.h>
#include <stdio.h>

int main() {
    struct puush *puush = puush_init();
    if (puush_auth_password(puush, "goodwork@nicetry.com", "hmmmmmmmmmm")) {
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

    puush_delete(puush, upload->id);
    puush_object_free(upload);
    printf("deleted!\n");
    printf("hopefully\n");

    puush_free(puush);

    return 0;
}
