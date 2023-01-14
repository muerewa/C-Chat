#include "stdio.h"
#include <unistd.h>
#include "../../include/handlers.h"
#include "../../include/structures.h"

void serverKeyHandler(struct users *user, struct keys *key, int fd) {
    while (1) {
        int buffer;

        if(read(fd, &buffer, sizeof(buffer)) != 0) {
            if (user->msgCount == 0) {
                user->e = buffer;
                write(fd, &key->e, sizeof(key->e));
                user->msgCount += 1;
            }
            else if (user->msgCount == 1) {
                user->n = buffer;
                write(fd, &key->n, sizeof(key->n));
                break;
            }
        }
    }
}
