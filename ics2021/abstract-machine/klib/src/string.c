#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s)
{
  size_t i = 0;
  while (s[i] != '\0')
    i++;
  return i;
}

char *strcpy(char *dst, const char *src)
{
  int i;
  for (i = 0; src[i] != '\0'; i++)
    dst[i] = src[i];
  dst[i] = '\0';
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n)
{
  int i;
  for (i = 0; i < n && src[i] != '\0'; i++)
    dst[i] = src[i];
  while (i < n)
    dst[i] = '\0';
  return dst;
}

char *strcat(char *dst, const char *src)
{
  char *r = dst;
  while (*r)
    r++;
  while (*src)
  {
    *r = *src;
    r++;
    src++;
  }
  *r = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2)
{
  while (*s1 && *s2 && (*s1 == *s2))
  {
    s1++;
    s2++;
  }
  return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
  while (n-- && *s1 && (*s1 == *s2))
  {
    s1++;
    s2++;
  }
  if (n == 0 || *s1 == *s2)
    return 0;
  else
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

void *memset(void *s, int c, size_t n)
{
  unsigned char *p = s;
  while (n--)
    *p++ = (unsigned char)c;
  return s;
}

void *memmove(void *dst, const void *src, size_t n)
{
  char *s1 = dst;
  const char *s2 = src;

  if (s1 < s2) // ·ÀÖ¹ÖØµþ
    for (size_t i = 0; i < n; i++)
      s1[i] = s2[i];
  else
    for (size_t i = n; i > 0; i--)
      s1[i - 1] = s2[i - 1];

  return dst;
}

void *memcpy(void *out, const void *in, size_t n)
{
  char *d = out;
  const char *s = in;
  for (size_t i = 0; i < n; i++)
    d[i] = s[i];
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
  const unsigned char *p1 = (const unsigned char *)s1, *p2 = (const unsigned char *)s2;
  while (n && (*p1 == *p2))
  {
    n--;
    p1++;
    p2++;
  }
  if (n == 0)
    return 0;
  else
    return *p1 - *p2;
}

#endif
