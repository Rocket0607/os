#include "../include/heap.h"

/*
Init heap with one large node:
    next = nullptr;
    size = heap_size;

When allocated:
    traverse until you find chunk big enough
    create new chunk remaining unneeded memory, set next to remaining piece of original chunk
    return pointer to original chunk (not including header)
*/

void init_heap()
{
    ((Chunk *)heap_ptr)[0].next = 0;
    ((Chunk *)heap_ptr)[0].info = HEAP_SIZE-sizeof(Chunk) & 0x7F;
}

void* alloc(int requested_size)
{
    // traverse heap, checking for appropriate chunk
    Chunk *curr_chunk = heap_ptr;
    while (curr_chunk != 0) {
        if (!(curr_chunk->info & 0x80) && (curr_chunk->info & 0x7F) >= requested_size) {
            break;
        }
        curr_chunk = curr_chunk->next;
    }
    if (curr_chunk->info & 0x80) {
        return 0;
    }
    // modify curr_chunk and create new chunk to store remaining data
    Chunk *new_chunk = (char *)curr_chunk + requested_size;
    new_chunk[0].next = curr_chunk->next;
    new_chunk[0].info = ((curr_chunk->info & 0x7F) - requested_size) & 0x7F;
    curr_chunk->next = new_chunk;
    curr_chunk->info = requested_size;
    curr_chunk->info = curr_chunk->info | 0x80;
    return (void *)(curr_chunk + 1);
}

void free(void *data)
{
    Chunk *chunk = ((Chunk *)data) - 1;
    chunk->info = chunk->info & 0x7F;
    // TODO: merge contiguous free chunks
}