#include <am.h>
#include <nemu.h>
#include <stdio.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init()
{
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg)
{
  uint32_t vga_wh = inl(VGACTL_ADDR);
  uint32_t vga_h = vga_wh & 0xff;
  uint32_t vga_w = vga_wh >> 16;
  *cfg = (AM_GPU_CONFIG_T){
      .present = true, .has_accel = false, .width = vga_w, .height = vga_h, .vmemsz = 0};
}

/*
  AM帧缓冲控制器, 可写入绘图信息,
  向屏幕(x, y)坐标处绘制w*h的矩形图像.
  图像像素按行优先方式存储在pixels中,
  每个像素用32位整数以00RRGGBB的方式描述颜色.
  若sync为true, 则马上将帧缓冲中的内容同步到屏幕上.
*/
void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl)
{
  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
  if (!ctl->sync && (w == 0 || h == 0))
    return;

  uint32_t *pixels = ctl->pixels;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  uint32_t screen_w = inl(VGACTL_ADDR) >> 16;
  for (int i = y; i < y + h; i++)
  {
    for (int j = x; j < x + w; j++)
    {
      fb[screen_w * i + j] = pixels[w * (i - y) + (j - x)];
    }
  }
  if (ctl->sync)
  {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status)
{
  status->ready = true;
}
