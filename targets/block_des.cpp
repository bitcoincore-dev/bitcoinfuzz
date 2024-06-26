#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include <iostream>
#include <stdio.h>

#include "block_des.h"
#include "../bitcoin/primitives/block.h"
#include "../bitcoin/streams.h"

extern "C" char* rust_bitcoin_des_block(const uint8_t *data, size_t len);

bool BlockDesCore(Span<const uint8_t> buffer)
{
    DataStream ds{buffer};
    CBlock block;
    try {
        ds >> TX_WITH_WITNESS(block);
        //block.GetHash();
    } catch (const std::ios_base::failure&) {
        return false;
    }
    return true;
}

// This target is expected to crash, needs some verification (e.g. segwit version).
void BlockDes(FuzzedDataProvider& provider) 
{
    std::vector<uint8_t> buffer{provider.ConsumeRemainingBytes<uint8_t>()};
    // TODO: Compare the `block hash`
    bool core{BlockDesCore(buffer)};
    std::string rust_bitcoin{rust_bitcoin_des_block(buffer.data(), buffer.size())};
    if (core) assert(rust_bitcoin != "");
    else assert(rust_bitcoin == "" || rust_bitcoin == "1");
}