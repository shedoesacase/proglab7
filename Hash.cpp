#include "Hash.hpp"
#include <cstring>

FileHasher::FileHasher(const std::string& algorithm) : algorithm_name_(algorithm), md5_ctx_(nullptr) {
    if (algorithm == "md5") {
        md5_ctx_ = EVP_MD_CTX_new();
        EVP_DigestInit_ex(md5_ctx_, EVP_md5(), nullptr);
    }
}

FileHasher::~FileHasher() {
    if (md5_ctx_) {
        EVP_MD_CTX_free(md5_ctx_);
    }
}

std::vector<uint8_t> FileHasher::hash_block(const void* data, size_t size) {
    if (algorithm_name_ == "crc32") {
        return hash_crc32(data, size);
    } else {
        return hash_md5(data, size);
    }
}

std::vector<uint8_t> FileHasher::hash_crc32(const void* data, size_t size) {
    boost::crc_32_type crc;
    crc.process_bytes(data, size);
    uint32_t result = crc.checksum();
    
    std::vector<uint8_t> hash(4);
    hash[0] = (result >> 24) & 0xFF;
    hash[1] = (result >> 16) & 0xFF;
    hash[2] = (result >> 8) & 0xFF;
    hash[3] = result & 0xFF;
    
    return hash;
}

std::vector<uint8_t> FileHasher::hash_md5(const void* data, size_t size) {
    std::vector<uint8_t> hash(16);
    unsigned int digest_len;

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_md5(), nullptr);
    EVP_DigestUpdate(ctx, data, size);
    EVP_DigestFinal_ex(ctx, hash.data(), &digest_len);
    EVP_MD_CTX_free(ctx);

    return hash;
}