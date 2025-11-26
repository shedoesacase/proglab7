#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <boost/crc.hpp>
#include <openssl/evp.h>

class FileHasher {
private:
    std::string algorithm_name_;
    EVP_MD_CTX* md5_ctx_;

public:
    explicit FileHasher(const std::string& algorithm);
    ~FileHasher();
    
    std::vector<uint8_t> hash_block(const void* data, size_t size);
    
private:
    std::vector<uint8_t> hash_crc32(const void* data, size_t size);
    std::vector<uint8_t> hash_md5(const void* data, size_t size);
};