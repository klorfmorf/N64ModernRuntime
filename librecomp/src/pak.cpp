#include "ultramodern/ultra64.h"
#include "ultramodern/ultramodern.hpp"

#include "recomp.h"
#include "helpers.hpp"

void save_write(RDRAM_ARG PTR(void) rdram_address, uint32_t offset, uint32_t count);
void save_read(RDRAM_ARG PTR(void) rdram_address, uint32_t offset, uint32_t count);

extern "C" void osPfsInit_recomp(uint8_t* rdram, recomp_context* ctx) {
    ctx->r2 = 0; // PFS_OK
}

extern "C" void osPfsInitPak_recomp(uint8_t * rdram, recomp_context* ctx) {
    ctx->r2 = 0; // PFS_OK
}

extern "C" void osPfsFreeBlocks_recomp(uint8_t * rdram, recomp_context * ctx) {
    ctx->r2 = 0; // PFS_OK
}

extern "C" void osPfsAllocateFile_recomp(uint8_t * rdram, recomp_context * ctx) {
    ctx->r2 = 0; // PFS_OK
}

extern "C" void osPfsDeleteFile_recomp(uint8_t * rdram, recomp_context * ctx) {
    ctx->r2 = 5; // PFS_ERR_INVALID
}

extern "C" void osPfsFileState_recomp(uint8_t * rdram, recomp_context * ctx) {
    PTR(OSPfs) pfs = _arg<0, PTR(OSPfs)>(rdram, ctx);
    s32 file_no = _arg<1, s32>(rdram, ctx);
    PTR(void) state = _arg<2, PTR(void)>(rdram, ctx);

    // Only the first file is valid.
    if (file_no == 0) {
        MEM_W(0, state) = 0x1000; // file_size
        MEM_W(4, state) = 0x4E473545; // game_code
        MEM_H(8, state) = 0x4A; // company_code

        for (s32 i = 0; i < 4; i++) {
            MEM_B(10 + i, state) = 0; // ext_name
        }

        for (s32 i = 0; i < 16; i++) {
            MEM_B(14 + i, state) = 0; // game_name
        }

        ctx->r2 = 0;
    } else {
        ctx->r2 = 5; // PFS_ERR_INVALID
    }
}

extern "C" void osPfsFindFile_recomp(uint8_t * rdram, recomp_context * ctx) {
    MEM_W(20, ctx->r29) = 0;
    ctx->r2 = 0; // PFS_OK
}

extern "C" void osPfsReadWriteFile_recomp(uint8_t * rdram, recomp_context * ctx) {
    PTR(OSPfs) pfs = _arg<0, PTR(OSPfs)>(rdram, ctx);
    s32 file_no = _arg<1, s32>(rdram, ctx);
    u8 flag = _arg<2, u8>(rdram, ctx);
    s32 offset = _arg<3, s32>(rdram, ctx);
    s32 size_in_bytes = MEM_W(16, ctx->r29);
    PTR(u8) data = MEM_W(20, ctx->r29);

    assert(file_no == 0);

    if (flag == 0) {
        save_read(rdram, data, offset, size_in_bytes);
    } else if (flag == 1) {
        save_write(rdram, data, offset, size_in_bytes);
    }

    ctx->r2 = 0; // PFS_OK
}

extern "C" void osPfsChecker_recomp(uint8_t * rdram, recomp_context * ctx) {
    ctx->r2 = 0; // PFS_OK
}

extern "C" void osPfsIsPlug_recomp(uint8_t* rdram, recomp_context* ctx) {
    MEM_B(0, ctx->r5) = 1;
    ctx->r2 = 0; // PFS_OK
}

extern "C" void osPfsNumFiles_recomp(uint8_t * rdram, recomp_context * ctx) {
    s32* max_files = _arg<1, s32*>(rdram, ctx);
    s32* files_used = _arg<2, s32*>(rdram, ctx);

    *max_files = 1;
    *files_used = 1;

    _return<s32>(ctx, 0); // PFS_OK
}

extern "C" void osPfsRepairId_recomp(uint8_t * rdram, recomp_context * ctx) {
    _return<s32>(ctx, 0); // PFS_OK
}
