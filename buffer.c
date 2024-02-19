#include "buffer.h"

#include "util.h"

/*
int
is_in_bounds(struct buffer source, size_t at)
{
   return (int)(at < source.count); 
}

int
are_equal(struct buffer a, struct buffer b)
{
    if (a.count != b.count) 
        return false;

    for (size_t i = 0; i < a.count; i++) {
        if (a.data[i] != b.data[i]) 
            return false;
    }

    return true;
}

struct buffer
xallocate_buffer(size_t count)
{
    struct buffer result = { .count = count };

    result.data = (u8*)malloc(count);
    if (result.data == NULL) {
        FAIL("could't allocate `struct buffer`");
    }

    return result;
}

void
free_buffer(struct buffer *buf)
{
    if (buf.data != NULL)
        free(buf.data);

    *buf = {0};
}
*/
