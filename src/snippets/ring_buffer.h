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

typedef struct ring {
    u32 *const buf;  // const pointer to a memory location of size u32
    i32        head;
    i32        tail;
    i32 const  cap;
} Ring;

Ring *new(i32 cap);
void release(Ring *buf);
i32  ring_push(Ring *, u32);
i32  ring_pop(Ring *, u32 *);
i32  test_ring(void);

#endif
