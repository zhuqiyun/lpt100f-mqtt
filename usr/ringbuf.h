#ifndef _RING_BUF_H_
#define _RING_BUF_H_

#include <hsf.h>
#include <stdlib.h>


typedef __packed struct {
    size_t rb_capacity;
    uint8_t  *rb_head;
    uint8_t  *rb_tail;
    uint8_t  *rb_buff;
}RingBuffer;

// RingBuffer* rb_new(size_t capacity);
void       rb_new(RingBuffer* rb,uint8_t *rbuff,uint16_t len);
void       rb_free(RingBuffer *rb);

size_t     rb_capacity(RingBuffer *rb);
size_t     rb_can_read(RingBuffer *rb);
size_t     rb_can_write(RingBuffer *rb);

size_t     rb_read(RingBuffer *rb, void *data, size_t count);
size_t     rb_write(RingBuffer *rb, const void *data, size_t count);

#endif
