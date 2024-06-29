#include "ring_buffer.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

enum
{
    SUCCESS = 0,
    FAILURE = -1
};

Ring *new(i32 cap) {
    Ring *buf           = malloc(sizeof(Ring));
    *(i32 *)(&buf->cap) = cap;
    return buf;
}

void release(Ring *ring) {
    free(ring);
}

i32 ring_push(Ring *ring, u32 in) {
    i32 next;
    next = ring->head + 1;
    if (next >= ring->cap) next = 0;         // NOTE: when head at cap
    if (next == ring->tail) return FAILURE;  // NOTE: and tail already reset before head (tail == 0)

    ring->buf[ring->head] = in;
    ring->head            = next;
    return SUCCESS;
}

i32 ring_pop(Ring *ring, u32 *out) {
    i32 next;
    if (ring->head == ring->tail) return FAILURE;  // NOTE: check for empty ring

    next = ring->tail + 1;
    if (next >= ring->cap) next = 0;
    *out       = ring->buf[ring->tail];
    ring->tail = next;
    return SUCCESS;
}

#define MAKE_RING(name, size)                         \
    u32  name##_data_space[size] = {0};               \
    Ring name                    = {                  \
                           .buf  = name##_data_space, \
                           .head = 0,                 \
                           .tail = 0,                 \
                           .cap  = size}

void ring_struct_info(Ring *ring) {
    printf("sizeof Ring =%ld\n", sizeof(Ring));
    printf("sizeof .buf =%ld\n", sizeof(ring->buf));
    printf("sizeof .head=%ld\n", sizeof(ring->head));
    printf("sizeof .tail=%ld\n", sizeof(ring->tail));
    printf("sizeof .cap =%ld\n", sizeof(ring->cap));
    printf("\n");
}

void print_ring_hex(Ring *ring) {
    printf("&ring=%p\n", (void *)ring);
    printf("&buf =%p %d\n", (void *)ring->buf, *ring->buf);
    printf("&buf =%p %d\n", (void *)&ring->head, ring->head);
    printf("&buf =%p %d\n", (void *)&ring->tail, ring->tail);
    printf("&buf =%p %d\n", (void *)&ring->cap, ring->cap);
    printf("\n");
}

void print_ring_dec(Ring *ring) {
    printf("&ring=%ld\n", (long)(void *)ring);
    printf("&buf =%ld %d\n", (long)(void *)&ring->buf, *ring->buf);
    printf("&head=%ld %d\n", (long)(void *)&ring->head, ring->head);
    printf("&tail=%ld %d\n", (long)(void *)&ring->tail, ring->tail);
    printf("&cap =%ld %d\n", (long)(void *)&ring->cap, ring->cap);
    printf("\n");
}

void test_pushing_empty_ring_at_maxed_cap(void) {
    MAKE_RING(ring, 1);
    u32 in     = 0x55;
    i32 signal = ring_push(&ring, in);
    print_ring_dec(&ring);
    assert(signal == FAILURE);
    assert(ring.head == ring.cap - 1);
}

void test_pushing_nonempty_ring_at_maxed_cap(void) {
    MAKE_RING(ring, 2);
    u32 in     = 0x55;
    i32 signal = ring_push(&ring, in);
    signal     = ring_push(&ring, in << 1);
    print_ring_dec(&ring);
    assert(signal == FAILURE);
    assert(ring.head == ring.cap - 1);
}

void test_pushing_nonempty_ring_at_near_cap(void) {
    MAKE_RING(ring, 2);
    u32 in     = 0x55;
    i32 signal = ring_push(&ring, in);
    print_ring_dec(&ring);
    assert(signal == SUCCESS);
    assert(ring.head == ring.cap - 1);
}

void test_popping_empty_ring_with_zero_actual_cap(void) {
    MAKE_RING(ring, 1);
    u32 out    = 0;
    i32 signal = ring_pop(&ring, &out);
    print_ring_dec(&ring);
    assert(signal == FAILURE);
    assert(ring.tail == ring.cap - 1);
}

void test_popping_empty_ring(void) {
    MAKE_RING(ring, 2);
    u32 out    = 0;
    i32 signal = ring_pop(&ring, &out);
    print_ring_dec(&ring);
    assert(signal == FAILURE);
    assert(ring.head == ring.tail);
}

void test_popping_nonempty_ring_at_cap(void) {
    MAKE_RING(ring, 2);
    u32 in = 0x55, out = 0;
    i32 signal = ring_push(&ring, in);
    assert(signal == SUCCESS);
    signal = ring_pop(&ring, &out);
    assert(signal == SUCCESS);
    print_ring_dec(&ring);
    assert(ring.head == ring.cap - 1);
}

void test_empty_ring(void) {
    MAKE_RING(ring, 3);
    u32 in = 0x55, out = 0;
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
    assert(ring.head == ring.tail);
    print_ring_dec(&ring);
}

void test_full_ring(void) {
    MAKE_RING(ring, 3);
    u32 in = 0x55, out = 0;
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
    assert(ring.head == ring.tail);
    print_ring_dec(&ring);
}

i32 test_ring(void) {
    test_pushing_empty_ring_at_maxed_cap();
    test_pushing_nonempty_ring_at_maxed_cap();
    test_pushing_nonempty_ring_at_near_cap();

    test_popping_empty_ring_with_zero_actual_cap();
    test_popping_empty_ring();
    test_popping_nonempty_ring_at_cap();

    test_empty_ring();
    test_full_ring();

    return SUCCESS;
}
