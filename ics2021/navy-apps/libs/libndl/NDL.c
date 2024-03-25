#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>
#include "/home/breeze/ics2021/nanos-lite/src/syscall.h"
#include <fcntl.h>

static int evtdev = -1;
static int fbdev = -1;
// static int dispinfo_dev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_x = 0, canvas_y = 0;
static int canvas_w = 0, canvas_h = 0;
typedef struct size
{
  int w;
  int h;
} Size;
Size disp_size;

// extern int my_gettimeofday(timeval *tv, timezone *tz);
uint32_t NDL_GetTicks()
{
  // static timeval timevals;
  // static timezone timezones;
  // //int ret = my_gettimeofday(&timevals, &timezones);
  // return timevals.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len)
{
  int fd = open("/dev/events", 0, 0);
  int ret = read(fd, buf, len);
  assert(close(fd) == 0);
  return ret == 0 ? 0 : 1;
}

// 打开一张(*w) X (*h)的画布
// 如果*w和*h均为0, 则将系统全屏幕作为画布, 并将*w和*h分别设为系统屏幕的大小
void NDL_OpenCanvas(int *w, int *h)
{
  if (*w == 0 && *h == 0)
  {
    *w = disp_size.w;
    *h = disp_size.h;
  }
  if (getenv("NWM_APP"))
  {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w;
    screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1)
    {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0)
        continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0)
        break;
    }

    close(fbctl);
  }
}

// 向画布`(x, y)`坐标处绘制`w*h`的矩形图像, 并将该绘制区域同步到屏幕上
// 图像像素按行优先方式存储在`pixels`中, 每个像素用32位整数以`00RRGGBB`的方式描述颜色
void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h)
{
  fbdev = open("/dev/fb", 0, 0);
  
  canvas_w = w;
  canvas_h = h;
  canvas_x = (screen_w - canvas_w) / 2;
  canvas_y = (screen_h - canvas_h) / 2;

  for (int i = 0; i < h && y + i < canvas_h; ++i) {
    //lseek(fbdev, ((y + canvas_y + i) * screen_w + (x + canvas_x)) * 4, SEEK_SET);
    //write(fbdev, pixels + i * w, 4 * (w < canvas_w - x ? w : canvas_w - x));
  }
  assert(close(fbdev) == 0);
}

void NDL_OpenAudio(int freq, int channels, int samples)
{
}

void NDL_CloseAudio()
{
}

int NDL_PlayAudio(void *buf, int len)
{
  return 0;
}

int NDL_QueryAudio()
{
  return 0;
}

void init_dispinof()
{
  int buf_size = 1024;
  char *buf = (char *)malloc(buf_size * sizeof(char));
  int fd = open("/proc/dispinfo", 0, 0);
  int ret = read(fd, buf, buf_size);
  assert(ret < buf_size); // to be cautious...
  assert(close(fd) == 0);

  int i = 0;
  int width = 0, height = 0;
  // 使用 strncmp 函数检查字符串 "WIDTH" 是否位于 buf 中 i 处开始的位置，以确保文件内容的格式正确。
  assert(strncmp(buf + i, "WIDTH", 5) == 0);
  // 这一行将 i 增加 5，以跳过字符串 "WIDTH"。
  i += 5;
  for (; i < buf_size; ++i)
  {
    if (buf[i] == ':')
    {
      i++;
      break;
    }
    assert(buf[i] == ' ');
  }
  for (; i < buf_size; ++i)
  {
    // 检查当前字符是否是数字字符。如果是，它跳出循环以开始解析宽度值。
    if (buf[i] >= '0' && buf[i] <= '9')
      break;
    assert(buf[i] == ' ');
  }
  for (; i < buf_size; ++i)
  {

    if (buf[i] >= '0' && buf[i] <= '9')
    {
      // 检查当前字符是否是数字字符。如果是，它将当前字符的数字值添加到 width 变量中。
      width = width * 10 + buf[i] - '0';
    }
    else
    {
      break;
    }
  }
  assert(buf[i++] == '\n');

  assert(strncmp(buf + i, "HEIGHT", 6) == 0);
  i += 6;
  for (; i < buf_size; ++i)
  {
    if (buf[i] == ':')
    {
      i++;
      break;
    }
    assert(buf[i] == ' ');
  }
  for (; i < buf_size; ++i)
  {
    if (buf[i] >= '0' && buf[i] <= '9')
      break;
    assert(buf[i] == ' ');
  }
  for (; i < buf_size; ++i)
  {
    if (buf[i] >= '0' && buf[i] <= '9')
    {
      height = height * 10 + buf[i] - '0';
    }
    else
    {
      break;
    }
  }

  free(buf);

  screen_w = width;
  screen_h = height;
}

int NDL_Init(uint32_t flags)
{
  printf("NDL_Init\n");
  if (getenv("NWM_APP"))
  {
    evtdev = 3;
  }
  init_dispinof();
  return 0;
}

void NDL_Quit()
{
}
