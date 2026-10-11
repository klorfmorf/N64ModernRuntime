#include <ultramodern/ultra64.h>
#include <ultramodern/ultramodern.hpp>
#include "recomp.h"

extern "C" void osMapTLB_recomp(uint8_t * rdram, recomp_context * ctx) {
    // TODO this will need to be implemented in the future for any games that actually use the TLB
}

extern "C" void osUnmapTLB_recomp(uint8_t * rdram, recomp_context * ctx) {
    // TODO this will need to be implemented in the future for any games that actually use the TLB
}

extern "C" void osSetTLBASID_recomp(uint8_t * rdram, recomp_context * ctx) {
    // TODO this will need to be implemented in the future for any games that actually use the TLB
}

extern "C" void osUnmapTLBAll_recomp(uint8_t * rdram, recomp_context * ctx) {
    // TODO this will need to be implemented in the future for any games that actually use the TLB
}
