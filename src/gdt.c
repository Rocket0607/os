#include "../include/gdt.h"

void encodeGdtEntry(uint8_t *target, struct GDT source)
{
    // Check the limit to make sure that it can be encoded
    // if (source.limit > 0xFFFFF) {kerror("GDT cannot encode limits larger than 0xFFFFF");}
    
    // Encode the limit
    target[0] = source.limit & 0xFF;
    target[1] = (source.limit >> 8) & 0xFF;
    target[6] = (source.limit >> 16) & 0x0F;
    
    // Encode the base
    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[7] = (source.base >> 24) & 0xFF;
    
    // Encode the access byte
    target[5] = source.access_byte;
    
    // Encode the flags
    target[6] |= (source.flags << 4);
}

void fill_gdt(uint8_t *target)
{
    struct GDT null_descriptor = {0, 0, 0, 0};
    struct GDT kernel_code_seg = {0x00000000, 0x000FFFFF, 0x9A, 0xC};
    struct GDT kernel_data_seg = {0x00000000, 0x000FFFFF, 0x92, 0xC};
    struct GDT user_code_seg = {0x00000000, 0x000FFFFF, 0xFA, 0xC};
    struct GDT user_data_seg = {0x00000000, 0x000FFFFF, 0xF2, 0xC};

    int limit = 0;

    encodeGdtEntry(target, null_descriptor);   target += 8; ++limit;
    encodeGdtEntry(target, kernel_code_seg);   target += 8; ++limit;
    encodeGdtEntry(target, kernel_data_seg);   target += 8; ++limit;
    encodeGdtEntry(target, user_code_seg);    target += 8; ++limit;
    encodeGdtEntry(target, kernel_code_seg);    target += 8; ++limit;
}