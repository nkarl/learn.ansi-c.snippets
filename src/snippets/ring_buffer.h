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
    i32 *const buf;  // const pointer to a memory location of size u32
    u32        front;
    u32        back;
    u32 const  cap;
} RingBuffer;

RingBuffer *new(u32);
void release(RingBuffer *);
i32  ring_push(RingBuffer *, i32);
i32  ring_pop(RingBuffer *, i32 *);
i32  test_ring(void);

#endif
