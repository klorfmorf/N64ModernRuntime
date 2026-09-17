#include <cstdint>
#include <cstring>

#include "recomp.h"
#include "helpers.hpp"
#include "tlb.h"

enum OSPageMask : uint32_t {
    OS_PM_4K   = 0x00000000,
    OS_PM_16K  = 0x00006000,
    OS_PM_64K  = 0x0001E000,
    OS_PM_256K = 0x0007E000,
    OS_PM_1M   = 0x001FE000,
    OS_PM_4M   = 0x007FE000,
    OS_PM_16M  = 0x01FFE000
};

struct TLBEntry {
    bool     active;
    uint32_t page_size;
    uint32_t vaddr;
    uint32_t evenpaddr;
    uint32_t oddpaddr;
    int32_t  asid;
};

TLBEntry entries[32];
uint32_t page_table[1048576]; //! TODO: Use a proper virtual memory mapping for this.

static inline uint32_t page_mask_to_page_size(OSPageMask pm) {
    return ((static_cast<uint32_t>(pm) >> 13) + 1) << 12;
}

static void commit_to_page_table(const TLBEntry& entry) {
    //! TODO: No support for ASID yet.

    uint32_t slices_per_page = entry.page_size >> 12;
    uint32_t even_page_index = entry.vaddr >> 12;
    uint32_t odd_page_index = even_page_index + slices_per_page;

    if (entry.evenpaddr != INVALID_PADDR) {
        for (size_t i = 0; i < slices_per_page; ++i) {
            page_table[even_page_index + i] = entry.evenpaddr + (i << 12);
        }
    }

    if (entry.oddpaddr != INVALID_PADDR) {
        for (size_t i = 0; i < slices_per_page; ++i) {
            page_table[odd_page_index + i] = entry.oddpaddr + (i << 12);
        }
    }
}

extern "C" void osMapTLB_recomp(uint8_t* rdram, recomp_context* ctx) {
    int32_t index = _arg<0, int32_t>(rdram, ctx);
    OSPageMask pm = _arg<1, OSPageMask>(rdram, ctx);
    PTR(void) vaddr = _arg<2, PTR(void)>(rdram, ctx);
    uint32_t evenpaddr = _arg<3, uint32_t>(rdram, ctx);
    uint32_t oddpaddr = static_cast<uint32_t>(MEM_W(0x10, ctx->r29));
    uint32_t asid = static_cast<uint32_t>(MEM_W(0x14, ctx->r29));

    uint32_t page_size = page_mask_to_page_size(pm);
    uint32_t dual_page_size = page_size << 1;

    vaddr &= ~(dual_page_size - 1);

    entries[index].active = true;
    entries[index].page_size = page_size;
    entries[index].vaddr = vaddr;
    entries[index].evenpaddr = evenpaddr;
    entries[index].oddpaddr = oddpaddr;
    entries[index].asid = asid;

    commit_to_page_table(entries[index]);
}

extern "C" void osUnmapTLB_recomp(uint8_t* rdram, recomp_context* ctx) {
    int32_t index = _arg<0, int32_t>(rdram, ctx);

    uint32_t dual_page_size = entries[index].page_size << 1;
    uint32_t start_page = entries[index].vaddr >> 12;
    uint32_t page_count = dual_page_size >> 12;

    for (size_t i = 0; i < page_count; ++i) {
        page_table[start_page + i] = INVALID_PADDR;
    }

    entries[index].active = false;
}

extern "C" void osUnmapTLBAll_recomp(uint8_t * rdram, recomp_context * ctx) {
    for (size_t i = 0; i < 32; ++i) {
        entries[i].active = false;
    }

    std::memset(page_table, 0xFF, sizeof(page_table));
}

extern "C" void osSetTLBASID(uint8_t * rdram, recomp_context * ctx) {
    int32_t asid = _arg<0, int32_t>(rdram, ctx);
}