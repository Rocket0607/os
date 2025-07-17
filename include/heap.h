typedef struct {
    Chunk *next;
    // 2 bytes; status stored in left most bit, size stored in rest
    unsigned short info;
} Chunk;

#define HEAP_SIZE 8192

extern void* heap_ptr;

void init_heap();

void* alloc(int requested_size);