#include <sys/stat.h>

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "utils.h"

void *file_contents(const char *path, size_t *ret_len) {
    assert(path != NULL);

    if (path == NULL)  return NULL;

    int fd;
    struct stat fd_stat;
    size_t len, pos;
    ssize_t bytes;
    char *buf;

    if ((fd = open(path, O_RDONLY)) < 0) {
        perror(path);
        return NULL;
    }

    if (fstat(fd, &fd_stat) < 0) {
        perror(path);
        close(fd);
        return NULL;
    }

    len = fd_stat.st_size;

    if (NULL == (buf = (char *) malloc(len + 1))) {
        perror("malloc");
        close(fd);
        return NULL;
    }

    pos = 0;
    do {
        bytes = read(fd, &buf[pos], len - pos);
        pos += bytes;
    } while (bytes > 0);

    if (bytes < 0) {
        perror(path);
        free(buf);
        close(fd);
        return NULL;
    }

    close(fd);

    buf[len] = '\0';
    if (ret_len) *ret_len = len;
    return buf;
}
