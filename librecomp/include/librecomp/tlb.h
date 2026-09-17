#ifndef __TLB_H__
#define __TLB_H__

#include <assert.h>
#include <cstdint>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define INVALID_PADDR 0xFFFFFFFF

extern uint32_t page_table[1048576];

static inline int64_t resolve_to_kseg0(int64_t vaddr) {
    if (((((uint64_t)vaddr) >> 28) & 0xF) == 0x8) {
        return vaddr;
    }

    uint32_t page_index = vaddr >> 12;
    uint32_t paddr_base = page_table[page_index];

    if (paddr_base == INVALID_PADDR) {
        return 0;
    }

    uint32_t offset = vaddr & 0xFFF;
    uint32_t physical = paddr_base + offset;
    
    return 0xFFFFFFFF80000000 + physical;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __TLB_H__
