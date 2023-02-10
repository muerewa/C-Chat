#include "stdio.h"
#include <unistd.h>
#include "../../include/handlers.h"
#include "../../include/structures.h"


/**
 * @brief 
 * 
 * @param user 
 * @param key 
 * @param fd 
 */
void serverKeyHandler(struct users *user, struct keys *key, int fd) {
    while (1) {
        int buffer = 0;

        if(read(fd, &buffer, sizeof(buffer)) != 0) {
            if (user->msgCount == 0) {
                user->exp = buffer;
                write(fd, &key->exp, sizeof(key->exp));
                user->msgCount = 1;
            } else if (user->msgCount == 1) {
                user->mod = buffer;
                write(fd, &key->mod, sizeof(key->mod));
                break;
            }
        }
    }
}