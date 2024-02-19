#ifndef BUFFER_H
#define BUFFER_H

#include <stdlib.h>

#include "util.h"

struct buffer {
    size_t count;
    u8 *data;
};

#define STACK_BUFFER(buf_name, N)\
    u8 _##buf_name[N]; \
    struct buffer buf_name = { .data = &_##buf_name[0], .count = N };

int
is_in_bounds(struct buffer source, size_t at);

int
are_equal(struct buffer a, struct buffer b);

struct buffer
xallocate_buffer(size_t count);

void
free_buffer(struct buffer *buf);


#endif
