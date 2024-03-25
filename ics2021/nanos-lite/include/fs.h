#ifndef __FS_H__
#define __FS_H__

#include <common.h>

#ifndef SEEK_SET
enum
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};
#endif
void init_fs();
int fs_open(const char *pathname, int flag, int mode);

size_t fs_read(int fd, void *buf, size_t len);

size_t fs_lseek(int fd, size_t offset, int which);

size_t fs_write(int fd, const void *buf, size_t len);

int fs_close(int fd);

#endif
