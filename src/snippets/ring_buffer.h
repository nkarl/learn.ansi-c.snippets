#ifndef RING_BUFFER_H
#define RING_BUFFER_H

typedef char           i8;
typedef short          i16;
typedef int            i32;
typedef long           i64;
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef unsigned long  u64;

typedef struct ring_buffer {
    u32 *const buf;  // const pointer to a memory location of size u32
    i32        front;
    i32        back;
    i32 const  cap;
} RingBuffer;

RingBuffer *new(i32 cap);
void release(RingBuffer *buf);
i32  ring_push(RingBuffer *, u32);
i32  ring_pop(RingBuffer *, u32 *);
i32  test_ring(void);

#endif
