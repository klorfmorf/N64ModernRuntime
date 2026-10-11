#include "ultramodern/ultramodern.hpp"

struct TLBPage {
    uint32_t vpn_masked;
    uint32_t paddr_base;
    uint32_t mask;
    uint8_t  asid;
    bool     global;
    bool     valid;
};

struct TLB {
    TLBPage pages[64];
    uint8_t current_asid;
};

static TLB tlb;

extern "C" void osMapTLB(RDRAM_ARG s32 index, OSPageMask pm, PTR(void) vaddr, u32 evenpaddr, u32 oddpaddr, s32 asid) {
    if (index < 0 || index >= 32) return;

    uint32_t page_size = (pm + 0x2000) >> 1;
    uint32_t page_mask = ~(page_size - 1);

    uint32_t dual_page_mask = ~((page_size << 1) - 1);
    uint32_t vaddr_even = vaddr & dual_page_mask;
    uint32_t vaddr_odd  = vaddr_even + page_size;

    bool is_global = (asid == -1);
    uint8_t entry_asid = is_global ? 0 : (uint8_t)asid;

    tlb.pages[index * 2 + 0] = {
        .vpn_masked = vaddr_even & page_mask,
        .paddr_base = (evenpaddr != (uint32_t)-1) ? (evenpaddr & page_mask) : 0,
        .mask       = page_mask,
        .asid       = entry_asid,
        .global     = is_global,
        .valid      = (evenpaddr != (uint32_t)-1)
    };

    tlb.pages[index * 2 + 1] = {
        .vpn_masked = vaddr_odd & page_mask,
        .paddr_base = (oddpaddr != (uint32_t)-1) ? (oddpaddr & page_mask) : 0,
        .mask       = page_mask,
        .asid       = entry_asid,
        .global     = is_global,
        .valid      = (oddpaddr != (uint32_t)-1)
    };
}

extern "C" void osUnmapTLB(RDRAM_ARG s32 index) {
    if (index < 0 || index >= 32) return;

    tlb.pages[index * 2 + 0].valid = false;
    tlb.pages[index * 2 + 1].valid = false;
}

extern "C" void osUnmapTLBAll(RDRAM_ARG1) {
    // Unmap entries 0 to 30.
    // Entry 31 is preserved for the RDB.
    for (size_t i = 0; i < 31 * 2; i++) {
        tlb.pages[i].valid = false;
    }
}

extern "C" void osSetTLBASID(RDRAM_ARG s32 asid) {
    tlb.current_asid = (uint8_t)asid;
}
