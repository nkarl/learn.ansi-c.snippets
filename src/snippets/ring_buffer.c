#include "ring_buffer.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * There are 3 cases where front == back:
 *  1. cap == cap (at the back  , cannot push)
 *  2.   0 == 0   (at the front , cannot pop)
 *  3.   _ == _   (anywhere else, cannot pop)
 *
 *  for popping, case 2 and 3 collapse into one.
 *  for pushing, case 1 must be check and reset if necessary.
 */

enum
{
    SUCCESS = 0,
    FAILURE = -1
};

typedef RingBuffer Ring;

Ring *new(u32 size) {
    Ring *ring         = malloc(sizeof(Ring));
    i32  *buffer       = malloc(size * sizeof(i32));
    *(i32 **)ring->buf = buffer;
    *(u32 *)&ring->cap = size;
    return ring;
}

void release(Ring *ring) {
    free(ring);
}

/*
 * NOTE: because the buffer is an array, it's possible to access by index.
 * To do that we calculate the offset using modulo.
 * However, this is a ring queue, so we should stick to the constraints of
 * a queue. Overriding constraints opens up complexity and potential bugs. KISS.
 */

/*
 * NOTE: at each push, the front is more important. We check:
 *  - where the next `front` will land.
 *    - if it lands on cap, then reset it to 0.
 *  - if `front == back`, which means full ring.
 */
i32 ring_push(Ring *ring, i32 in) {
    u32 next_front = ring->front + 1;

    if (next_front >= ring->cap) next_front = 0;
    if (next_front == ring->back) return FAILURE;

    ring->buf[ring->front] = in;
    ring->front            = next_front;
    return SUCCESS;
}

/*
 * NOTE: at each pop, the back is more important. We check
 *  - if `front == back`, which means empty ring.
 *  - where the next `back` will land.
 *    - if it lands on cap, reset it to 0.
 */
i32 ring_pop(Ring *ring, i32 *out) {
    if (ring->front == ring->back) return FAILURE;  // NOTE: check for empty ring

    u32 next_back = ring->back + 1;
    if (next_back >= ring->cap) next_back = 0;

    *out       = ring->buf[ring->back];
    ring->back = next_back;
    return SUCCESS;
}

/*
 * NOTE: will allocate a new ring on the stack (fast);
 * no need for `malloc` and `free` if live and die inside the context
 */
#define MAKE_RING(name, size)                          \
    i32  name##_data_space[size] = {0};                \
    Ring name                    = {                   \
                           .buf   = name##_data_space, \
                           .front = 0,                 \
                           .back  = 0,                 \
                           .cap   = size}

#define MAKE_RING_WITH_PTR(name, size, buffer_ptr) \
    Ring name = {                                  \
        .buf   = buffer_ptr,                       \
        .front = 0,                                \
        .back  = 0,                                \
        .cap   = size}

void ring_struct_info(Ring *ring) {
    printf("sizeof Ring  =%ld\n", sizeof(Ring));
    printf("sizeof .buf  =%ld\n", sizeof(ring->buf));
    printf("sizeof .front=%ld\n", sizeof(ring->front));
    printf("sizeof .back =%ld\n", sizeof(ring->back));
    printf("sizeof .cap  =%ld\n", sizeof(ring->cap));
    printf("\n");
}

void print_ring_hex(Ring *ring) {
    printf("&ring =%p\n", (void *)ring);
    printf("&buf  =%p %d\n", (void *)ring->buf, *ring->buf);
    printf("&front=%p %d\n", (void *)&ring->front, ring->front);
    printf("&back =%p %d\n", (void *)&ring->back, ring->back);
    printf("&cap  =%p %d\n", (void *)&ring->cap, ring->cap);
    printf("\n");
}

void print_ring_dec(Ring *ring) {
    printf("&ring =%ld\n", (long)(void *)ring);
    printf("&buf  =%ld %d\n", (long)(void *)ring->buf, *ring->buf);
    printf("&front=%ld %d\n", (long)(void *)&ring->front, ring->front);
    printf("&back =%ld %d\n", (long)(void *)&ring->back, ring->back);
    printf("&cap  =%ld %d\n", (long)(void *)&ring->cap, ring->cap);
    printf("\n");
}

void test_pushing_ring_with_zero_actual_cap(void) {
    MAKE_RING(ring, 1);
    i32 in     = 0x55;
    i32 signal = ring_push(&ring, in);
    print_ring_dec(&ring);
    assert(signal == FAILURE);
    assert(ring.front == ring.cap - 1);
}

void test_popping_ring_with_zero_actual_cap(void) {
    MAKE_RING(ring, 1);
    i32 out    = 0;
    i32 signal = ring_pop(&ring, &out);
    print_ring_dec(&ring);
    assert(signal == FAILURE);
    assert(ring.back == ring.cap - 1);
}

void test_pushing_nonempty_ring_at_cap(void) {
    MAKE_RING(ring, 2);
    i32 in     = 0x55;
    i32 signal = ring_push(&ring, in);
    signal     = ring_push(&ring, in << 1);
    print_ring_dec(&ring);
    assert(signal == FAILURE);
    assert(ring.front == ring.cap - 1);
}

void test_pushing_nonempty_ring_near_cap(void) {
    MAKE_RING(ring, 2);
    i32 in     = 0x55;
    i32 signal = ring_push(&ring, in);
    print_ring_dec(&ring);
    assert(signal == SUCCESS);
    assert(ring.front == ring.cap - 1);
}

void test_popping_empty_ring(void) {
    MAKE_RING(ring, 2);
    i32 out    = 0;
    i32 signal = ring_pop(&ring, &out);
    print_ring_dec(&ring);
    assert(signal == FAILURE);
    assert(ring.front == ring.back);
}

void test_popping_nonempty_ring_at_cap(void) {
    MAKE_RING(ring, 2);
    i32 in = 0x55, out = 0;
    i32 signal = ring_push(&ring, in);
    assert(signal == SUCCESS);
    signal = ring_pop(&ring, &out);
    assert(signal == SUCCESS);
    print_ring_dec(&ring);
    assert(ring.front == ring.cap - 1);
}

void test_empty_ring(void) {
    MAKE_RING(ring, 3);
    i32 in = 0x55, out = 0;
    i32 signal = ring_push(&ring, in);
    assert(signal == SUCCESS);
    signal = ring_push(&ring, in << 1);
    assert(signal == SUCCESS);
    signal = ring_pop(&ring, &out);
    assert(signal == SUCCESS);
    signal = ring_push(&ring, in << 1);
    assert(signal == SUCCESS);
    signal = ring_push(&ring, in << 1);
    assert(signal == FAILURE);
    signal = ring_pop(&ring, &out);
    assert(signal == SUCCESS);
    signal = ring_pop(&ring, &out);
    assert(signal == SUCCESS);
    assert(ring.front == ring.back);
    print_ring_dec(&ring);
}

void test_full_ring(void) {
    MAKE_RING(ring, 3);
    i32 in = 0x55, out = 0;
    i32 signal = ring_push(&ring, in);
    assert(signal == SUCCESS);
    signal = ring_push(&ring, in << 1);
    assert(signal == SUCCESS);
    signal = ring_pop(&ring, &out);
    assert(signal == SUCCESS);
    signal = ring_pop(&ring, &out);
    assert(signal == SUCCESS);
    signal = ring_push(&ring, in << 1);
    assert(signal == SUCCESS);
    signal = ring_pop(&ring, &out);
    assert(signal == SUCCESS);
    signal = ring_push(&ring, in << 1);
    assert(signal == SUCCESS);
    signal = ring_push(&ring, in << 1);
    assert(signal == SUCCESS);
    signal = ring_push(&ring, in << 1);
    assert(signal == FAILURE);
    signal = ring_pop(&ring, &out);
    assert(signal == SUCCESS);
    signal = ring_pop(&ring, &out);
    assert(signal == SUCCESS);
    assert(ring.front == ring.back);
    print_ring_dec(&ring);
}

void test_heap_ring(void) {
    u32  size   = 2;
    i32 *buffer = malloc(size * sizeof(i32));

    MAKE_RING_WITH_PTR(ring, size, buffer);

    i32 in = 0x55, out = 0;
    i32 signal = ring_push(&ring, in);
    assert(signal == SUCCESS);
    signal = ring_pop(&ring, &out);
    assert(signal == SUCCESS);
    print_ring_dec(&ring);
}

i32 test_ring(void) {
    test_pushing_ring_with_zero_actual_cap();
    test_popping_ring_with_zero_actual_cap();

    test_pushing_nonempty_ring_at_cap();
    test_pushing_nonempty_ring_near_cap();

    test_popping_empty_ring();
    test_popping_nonempty_ring_at_cap();

    test_empty_ring();
    test_full_ring();

    test_heap_ring();

    return SUCCESS;
}
