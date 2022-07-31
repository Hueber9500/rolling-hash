#pragma once

#include <string>

struct ChunkInfo{
    int length;
    int offset;  
    long long rolling_hash;
    std::string data;
    char sha1_hex[40];

    ChunkInfo(int length, int offset, long long rolling_hash, std::string data, const char* hex_hash):
        length(length), offset(offset), data(data), rolling_hash(rolling_hash)
    {
        strncpy(sha1_hex, hex_hash, sizeof(sha1_hex));
    }

    ChunkInfo(){
        length = 0;
        offset = 0;
        memset(sha1_hex, 0, sizeof(sha1_hex));
        rolling_hash = 0;
    }
};