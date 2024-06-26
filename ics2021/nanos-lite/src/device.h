#ifndef _DEVICE_H_
#define _DEVICE_H_

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_write(const void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
#endif