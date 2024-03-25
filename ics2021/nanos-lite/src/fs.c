#include <fs.h>
#include <klib.h>
#include "ramdisk.h"
#include "syscall.h"
#include "device.h"

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);

#define NR_FILE (sizeof(file_table) / sizeof(file_table[0]))
#define min(x, y) ((x < y) ? x : y)

typedef struct
{
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset; // 进行读写操作时的偏移
  ReadFn read;
  WriteFn write;
} Finfo;

enum
{
  FD_STDIN,
  FD_STDOUT,
  FD_STDERR,
  FD_FB
};

size_t invalid_read(void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

size_t file_read(void *buf, size_t offset, size_t len)
{
  return ramdisk_read(buf, offset, len);
}

size_t file_write(const void *buf, size_t offset, size_t len)
{
  return ramdisk_write(buf, offset, len);
}

/* This is the information about all files in disk. */

static Finfo file_table[] __attribute__((used)) = {
    [FD_STDIN] = {"stdin", 0, 0, 0, invalid_read, invalid_write},
    [FD_STDOUT] = {"stdout", 0, 0, 0, invalid_read, serial_write},
    [FD_STDERR] = {"stderr", 0, 0, 0, invalid_read, serial_write},
    {"/dev/events", 0, 0, 0, events_read, invalid_write},
    {"/proc/dispinfo", 50, 0, 0, dispinfo_read, invalid_write},
    {"/dev/fb", 0, 0, 0, invalid_read, fb_write},
#include "files.h"
};

void init_fs()
{
  // TODO: initialize the size of /dev/fb
  AM_GPU_CONFIG_T ev = io_read(AM_GPU_CONFIG);
  int width = ev.width;
  int height = ev.height;
  file_table[FD_FB].size = width * height * sizeof(uint32_t);
}

int fs_open(const char *pathname, int flag, int mode)
{
  for (size_t fd = 0; fd < NR_FILE; fd++)
  {
    if (strcmp(file_table[fd].name, pathname) == 0)
    {
      if (fd < 3)
      {
        Log("ignore open %s", pathname);
        return fd;
      }
      file_table[fd].open_offset = 0;
      Log("file name %s , fd=%d", file_table[fd].name, fd);
      return fd;
    }
  }
  Log("not found the path : %s", pathname);
  assert(0);
}

size_t fs_read(int fd, void *buf, size_t len)
{
  ReadFn readfn = file_table[fd].read;
  if (readfn != NULL)
    return readfn(buf, 0, len);

  size_t read_len = len;
  size_t open_offset = file_table[fd].open_offset;
  size_t size = file_table[fd].size;
  size_t disk_offset = file_table[fd].disk_offset;

  if (open_offset > size)
    assert(0);

  if (open_offset + len > size)
    read_len = size - open_offset;
  // Log("open_offset = %d, disk_offset = %d", open_offset, disk_offset);
  read_len = ramdisk_read(buf, disk_offset + open_offset, read_len);
  file_table[fd].open_offset += read_len;
  return read_len;
}

size_t fs_lseek(int fd, size_t offset, int which)
{
  if (fd <= 2)
  {
    Log("ignore lseek %s", file_table[fd].name);
    return 0;
  }

  size_t size = file_table[fd].size;
  switch (which)
  {
  case SEEK_SET:
    if (offset > size)
      file_table[fd].open_offset = size;
    else
      file_table[fd].open_offset = offset;
    break;

  case SEEK_CUR:
    if (offset + file_table[fd].open_offset > size)
      file_table[fd].open_offset = size;
    else
      file_table[fd].open_offset += offset;
    break;

  case SEEK_END:
    if (offset + size > size)
      file_table[fd].open_offset = size;
    else
      file_table[fd].open_offset = file_table[fd].size + offset;
    break;

  default:
    Log("lseek failed");
    return -1;
  }
  // Log("lseek = %d", file_table[fd].open_offset);
  return file_table[fd].open_offset;
}

size_t fs_write(int fd, const void *buf, size_t len)
{
  WriteFn writefn = file_table[fd].write;
  if (writefn != NULL)
    return writefn(buf, 0, len);
  size_t write_len = len;
  size_t open_offset = file_table[fd].open_offset;
  size_t size = file_table[fd].size;
  size_t disk_offset = file_table[fd].disk_offset;

  if (open_offset > size)
    return 0;

  if (open_offset + len > size)
    write_len = size - open_offset;
  ramdisk_write(buf, disk_offset + open_offset, write_len);
  file_table[fd].open_offset += write_len;
  Log("writelen = %d", write_len);
  return write_len;
}

int fs_close(int fd)
{
  if (fd >= 3 && fd < NR_FILE)
  {
    file_table[fd].open_offset = 0;
    return 0;
  }
  else
    return -1;
}