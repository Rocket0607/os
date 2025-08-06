#include <stdint.h>

#define NUM_GDT_ENTRIES 5

struct GDT {
    uint32_t base;         // 32-bit base address
    uint32_t limit;        // up to 20-bit limit
    uint8_t access_byte;   // access flags (present, ring, type, etc.)
    uint8_t flags;         // high 4 bits of flags (granularity & size)
};

void encodeGdtEntry(uint8_t *target, struct GDT source);
void fill_gdt(uint8_t *target);