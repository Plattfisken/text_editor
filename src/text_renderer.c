#pragma once
#include "root.h"
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

typedef struct {
    u32 id;
    u32 checksum;
    u32 offset;
    u32 length;
} TTF_Table;

u16 swap_bytes_u16(u16 num) {
    return num << 8 | num >> 8;
}

u32 swap_bytes_u32(u32 num) {
    return ((num>>24) & 0xff)     |
           ((num>>8)  & 0xff00)   |
           ((num<<8)  & 0xff0000) |
           ((num<<24) & 0xff000000);
}

u16 read_u16_no_swap(FILE *f) {
    u16 result = 0;
    size_t items_read = fread(&result, sizeof result, 1, f);
    assert(items_read == 1 && "Failed to read u16");
    return result;
}

u32 read_u32_no_swap(FILE *f) {
    u32 result = 0;
    size_t items_read = fread(&result, sizeof result, 1, f);
    assert(items_read == 1 && "Failed to read u32");
    return result;
}

// Reads an unsigned integer from the file. Assumes the bytes should be swapped.
u16 read_u16(FILE *f) {
    return swap_bytes_u16(read_u16_no_swap(f));
}

u32 read_u32(FILE *f) {
    return swap_bytes_u32(read_u32_no_swap(f));
}

void load_ttf(char *file_path) {
    FILE *f = fopen(file_path, "r");
    assert(f && "Failed to open file");

    // offset subtable
    u32 scaler_type = read_u32(f);

    u16 num_tables = read_u16(f);
    u16 search_range = read_u16(f);

    u16 entry_selector = read_u16(f);
    u16 range_shift = read_u16(f);

    printf("scaler_type: %u\n", scaler_type);
    printf("num tables: %hu\n", num_tables);
    printf("search range: %hu\n", search_range);
    printf("entry selector: %hu\n", entry_selector);
    printf("range shift: %hu\n", range_shift);

    TTF_Table *tables = malloc(num_tables * sizeof tables[0]);
    memset(tables, 0, num_tables * sizeof tables[0]);

    for(int i = 0; i < num_tables; ++i) {
        // read as a string so we don't wanna swap the bytes
        tables[i].id = read_u32_no_swap(f);

        tables[i].checksum = read_u32(f);
        tables[i].offset = read_u32(f);
        tables[i].length = read_u32(f);

        printf("id: %.4s\n", (char *)(&tables[i].id));
        printf("checksum: %u\n", tables[i].checksum);
        printf("offset: %u\n", tables[i].offset);
        printf("length: %u\n", tables[i].length);
        printf("\n");
    }

    free(tables);

    fclose(f);
}
