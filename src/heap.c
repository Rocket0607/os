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

#define HEAP_CHUNK_STATUS_MASK 0x80
#define HEAP_CHUNK_SIZE_MASK 0x7F

#define MASK_SIZE(size) ((size) & HEAP_CHUNK_SIZE_MASK)
#define GET_SIZE(info) ((info) & HEAP_CHUNK_SIZE_MASK)
#define SET_ALLOC_FLAG(info) ((info) | HEAP_CHUNK_STATUS_MASK)
#define UNSET_ALLOC_FLAG(info) ((info) & ~HEAP_CHUNK_STATUS_MASK)
#define GET_ALLOC_FLAG(info) ((info) & HEAP_CHUNK_STATUS_MASK)

#define ALIGN_UP(ptr, alignment) (((ptr) + ((alignment)-1)) & ~((alignment)-1))

void init_heap()
{
    ((Chunk *)heap_ptr)[0].next = 0;
    // sets left most bit to 0, while keep the rest of the bits for an accurate size
    // due to the size of the heap and chunk, the value will never actually be large enough to take up that last bit, this just ensures it is 0
    ((Chunk *)heap_ptr)[0].info = MASK_SIZE(HEAP_SIZE-sizeof(Chunk));
}

void* alloc(int requested_size)
{
    // align requested_size to ensure alignment of heap
    requested_size = ALIGN_UP(requested_size, 8);
    // traverse heap, checking for appropriate chunk
    Chunk *curr_chunk = heap_ptr;
    while (curr_chunk != 0) {
        if (!(GET_ALLOC_FLAG(curr_chunk->info)) && (GET_SIZE(curr_chunk->info)) >= requested_size) {
            break;
        }
        curr_chunk = curr_chunk->next;
    }
    if (curr_chunk->info & HEAP_CHUNK_STATUS_MASK) {
        return 0;
    }
    // modify curr_chunk and create new chunk to store remaining data
    Chunk *new_chunk = (Chunk *)((char *)curr_chunk + sizeof(Chunk) + requested_size);
    new_chunk[0].next = curr_chunk->next;
    new_chunk[0].info = MASK_SIZE(GET_SIZE(curr_chunk->info) - requested_size);
    curr_chunk->next = new_chunk;
    curr_chunk->info = requested_size;
    curr_chunk->info = SET_ALLOC_FLAG(curr_chunk->info);
    return (void *)(curr_chunk + 1);
}

void free(void *data)
{
    Chunk *chunk = ((Chunk *)data) - 1;
    chunk->info = UNSET_ALLOC_FLAG(chunk->info);
    // TODO: merge contiguous free chunks
}