/**
* \file
*		Ring Buffer library
*/

#include "ringbuf.h"
#include <stdlib.h>
#include <string.h>

#define min(a, b) (a)<(b)?(a):(b)

//static char buff[512] = {0};
void rb_new(RingBuffer* rb,uint8_t *rbuff,uint16_t len)
{
    rb->rb_capacity = len;//-sizeof(RingBuffer);//capacity;
	  rb->rb_buff     = rbuff;    
	  rb->rb_head     = rb->rb_buff;
    rb->rb_tail     = rb->rb_buff;
};

void  rb_free(RingBuffer *rb)
{
		rb->rb_capacity = 0;		
	  rb->rb_buff     = NULL;
		rb->rb_head     = NULL;
		rb->rb_tail     = NULL;	
}

size_t     rb_capacity(RingBuffer *rb)
{
	  if(rb==NULL)  return 0;
    return rb->rb_capacity;
}
size_t     rb_can_read(RingBuffer *rb)
{
	  if(rb==NULL)  return 0;
    if (rb->rb_head == rb->rb_tail) return 0;
    if (rb->rb_head < rb->rb_tail) return rb->rb_tail - rb->rb_head;
    return rb_capacity(rb) - (rb->rb_head - rb->rb_tail);
}
size_t     rb_can_write(RingBuffer *rb)
{
	  if(rb==NULL)  return 0;
    return rb_capacity(rb) - rb_can_read(rb);
}

size_t     rb_read(RingBuffer *rb, void *data, size_t count)
{
	  if(rb==NULL)  return 0;
    if (rb->rb_head < rb->rb_tail)
    {
        int copy_sz = min(count, rb_can_read(rb));
        memcpy(data, rb->rb_head, copy_sz);
        rb->rb_head += copy_sz;
        return copy_sz;
    }
    else
    {
        if (count < rb_capacity(rb)-(rb->rb_head - rb->rb_buff))
        {
            int copy_sz = count;
            memcpy(data, rb->rb_head, copy_sz);
            rb->rb_head += copy_sz;
            return copy_sz;
        }
        else
        {
            int copy_sz = rb_capacity(rb) - (rb->rb_head - rb->rb_buff);
            memcpy(data, rb->rb_head, copy_sz);
            rb->rb_head = rb->rb_buff;
            copy_sz += rb_read(rb, (char*)data+copy_sz, count-copy_sz);
            return copy_sz;
        }
    }
}

size_t     rb_write(RingBuffer *rb, const void *data, size_t count)
{
	  if(rb==NULL)  return 0;
    if (count >= rb_can_write(rb)) 
			return 0;
    
    if (rb->rb_head <= rb->rb_tail)
    {
        int tail_avail_sz = rb_capacity(rb) - (rb->rb_tail - rb->rb_buff);
        if (count <= tail_avail_sz)
        {
            memcpy(rb->rb_tail, data, count);
            rb->rb_tail += count;
            if (rb->rb_tail == rb->rb_buff+rb_capacity(rb))
                rb->rb_tail = rb->rb_buff;
            return count;
        }
        else
        {
            memcpy(rb->rb_tail, data, tail_avail_sz);
            rb->rb_tail = rb->rb_buff;
            
            return tail_avail_sz + rb_write(rb, (char*)data+tail_avail_sz, count-tail_avail_sz);
        }
    }
    else
    {
        memcpy(rb->rb_tail, data, count);
        rb->rb_tail += count;
        return count;
    }
}
