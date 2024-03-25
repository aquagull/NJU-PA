#include <common.h>
#include "fs.h"
#include "ramdisk.h"
#include "/home/breeze/ics2021/abstract-machine/am/include/amdev.h"

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
#define MULTIPROGRAM_YIELD() yield()
#else
#define MULTIPROGRAM_YIELD()
#endif

#define NUM_LEN 64
static AM_GPU_CONFIG_T gpu_config;
static AM_GPU_FBDRAW_T gpu_fbdraw;

// static char *__itoa(int num, char *buff)
// {
//   char tmp[NUM_LEN];
//   if (num == 0)
//   {
//     strcat(buff, "0");
//     return buff;
//   }

//   uint8_t i = 0;
//   while (num != 0)
//   {
//     tmp[i] = num % 10 + '0';
//     num /= 10;
//     i++;
//   }

//   for (int j = i - 1; j >= 0; --j)
//     buff[i - 1 - j] = tmp[j];
//   buff[i] = '\0';

//   return buff;
// }

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
    [AM_KEY_NONE] = "NONE",
    AM_KEYS(NAME)};

size_t serial_write(const void *buf, size_t offset, size_t len)
{
  char *str = (char *)buf;
  for (size_t i = 0; i < len; ++i)
  {
    putch(str[i]);
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len)
{
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if (ev.keycode == AM_KEY_NONE)
  {
    *(char *)buf = '\0';
    return 0;
  }
  int ret = snprintf(buf, len, "%s %s\n", ev.keydown ? "kd" : "ku", keyname[ev.keycode]);
  printf("%s\n", buf);
  return ret;
}

size_t f_read(void *buf, size_t offset, size_t len)
{
  panic("should not run here");
  return 0;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len)
{
  AM_GPU_CONFIG_T t = io_read(AM_GPU_CONFIG);
  return snprintf((char *)buf, len, "WIDTH:%d\nHEIGHT:%d\n", t.width, t.height);
}

size_t fb_write(const void *buf, size_t offset, size_t len)
{
  if (len == 0)
  {
    gpu_fbdraw.sync = 1;
    gpu_fbdraw.w = 0;
    gpu_fbdraw.h = 0;
    ioe_write(AM_GPU_FBDRAW, &gpu_fbdraw);
    return 0;
  }

  int width = gpu_config.width;

  gpu_fbdraw.pixels = (void *)buf;
  gpu_fbdraw.w = len + 200;
  gpu_fbdraw.h = 300;
  gpu_fbdraw.x = offset % width;
  gpu_fbdraw.y = offset / width;
  gpu_fbdraw.sync = 0;
  ioe_write(AM_GPU_FBDRAW, &gpu_fbdraw);
  return len;
}

void init_device()
{
  Log("Initializing devices...");
  ioe_init();

  ioe_read(AM_GPU_CONFIG, &gpu_config);
  printf("%d %d\n", gpu_config.width, gpu_config.height);
}
